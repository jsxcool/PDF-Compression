#ifndef PDF_PDFOBJECT_H
#define PDF_PDFOBJECT_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>


using namespace std;

// how to solve distance of space:
// ignore the very small distance; note big distance after every word

// 1 point = 1/72 inch = 0.0139 inch = 0.353 mm   (1/72 in pdf specification)
// So size of font means the width of the widest letter - M
const float Point = 35.3;  // 100 times of the actual size
// of course, there are full space or thin space

// Pattern:  ... Td  ... TJ
struct Line {
    float x, y;   // the coordinate displacement of text, eg: 0 -11.955
    vector<string> words;   // the content of the whole line of words
    vector<int> intervals;  // the size of intervals is one less than words

    void printSeparate(ostream& s) const {
        s << x << ',' << y << "\n";
        for (int i = 0; i < words.size(); i++)
            s << words[i]<< ' ';
        s << '\n';
        for (int i = 0;i < words.size()-1; i++)
            s << intervals[i] << ' ';
        s << '\n';
    }
    friend ostream& operator << (ostream& s, const Line& l){
        s << l.x << ',' << l.y << ":\t";
        int len = l.words.size();
        for(int i=0; i<len-1; i++)
            s << l.words[i] << ' ' << l.intervals[i] << ' ';
        s << l.words[len-1];
        return s;
    }

    int shortenInterval() const {
        int len = intervals.size();
        if(len==0)
            return 0;

        float thre = 0.05;
        int count = 1;
        int basic=intervals[0];
        for(int i=1; i<len; i++){
            if(abs(intervals[i]-basic)/float(intervals[i]) < thre)
                count++;
            else
                basic = intervals[i];
            if(count == 2)
                break;
        }
        return basic;
    }



};

// Pattern: ... Tf + vector<Line>
struct Paragraph{
    string fontType;  // eg: F8
    float fontSize;  // eg: 9.9626
    vector<Line> contents;

    friend ostream& operator << (ostream& s, const Paragraph& para){
        s << "Font Type: " << para.fontType << ' ' << para.fontSize << '\n';
        int len = para.contents.size();
        for(int i=0; i<len; i++)
            //s << para.contents[i] << '\n';
            para.contents[i].printSeparate(s);
        return s;
    }
};

// This can be regarded as Page object if there is no image
// Pattern: BT --- ET
struct Stream{
    float originX = 0;
    float originY = 0;
    vector<Paragraph> paragraphs;

    friend ostream& operator << (ostream& s, const Stream& strm){
        s << "From original coordinate (0, 0) \n\n";

        int len = strm.paragraphs.size();
        for(int i=0; i<len; i++)
            s << strm.paragraphs[i] << '\n';
        s << '\n';
        return s;
    }
};

struct Page{
    vector<Stream> streams;

    friend ostream& operator << (ostream& s, const Page& p){
        int len = p.streams.size();
        int NumOfStream = 0;
        for(int i=0; i<len; i++) {
            s << "stream["<< NumOfStream <<"]:" << "\n";
            s << p.streams[i];
            NumOfStream++;
        }
        return s;
    }
};

class PDFFile {
private:
    vector<Page> pages;
    int calculateDistance(const string& str, int& pos);
    string getText(const string& str, int& pos);
    void extractWords(const string& str, int space, Line& l);

    vector<pair<string, string>> dynamicCode;
public:
    // only works for pure text (ignore images)
    void readIn();

    int compressLine(unordered_map<string, string>& encodeBook, const Line& lines, string wordsCoded[]);
    void write(const char* out, unordered_map<string, string>& encodeBook);

    void generatePs(const char* filename);

    friend ostream& operator << (ostream& s, const PDFFile& f){
        int len = f.pages.size();
        for(int i=0; i<len; i++) {
            s << "Page" << i << ":\n";
            s << f.pages[i] << '\n';
        }
        s << "-PDF File End-\n";
        return s;
    }

    void getRawData(const char* out) const {
        ofstream f(out);
        f << "-PDF File Begin-\n";
        int len = pages.size();
        for(int i=0; i<len; i++) {
            f << "Page" << i << ":\n";
            f << pages[i] << '\n';
        }
        f << "-PDF File End-\n";
        f.close();
    }

    void readCompressedFile(const char* encodedFile, unordered_map<string, string>& encodeBook);

};

#endif //PDF_PDFOBJECT_H
