#ifndef PDF_DECOMPRESS_H
#define PDF_DECOMPRESS_H

#include <fstream>
#include <string>
#include "PDFObject.h"
#include "encodeMap.h"

// idea: note which line dynamic zone is, then directly go there
void EncodeMap::loadDynamicCode(const char* encodedFile){
    ifstream f(encodedFile);
    string linebuf;
    while(!f.eof()){
        getline(f, linebuf);
        if(linebuf[0] == 'd'){
            if(linebuf.substr(0,7) == "dynamic"){
                string word, code;
                while(!f.eof()) {
                    f >> word >> code;
                    encodeBook[code] = word;
                }
            }
        }
    }
}

void getDisplacement(const string & linebuf, Line& l){
    istringstream iss2(linebuf);
    string temp3, temp4;
    iss2 >> temp3 >> temp4;
    l.x = stof(temp3);
    l.y = stof(temp4);
}

void getDistance(const string & linebuf, Line& l){
    istringstream iss3(linebuf);
    int basic;
    iss3 >> basic;
    if(basic==0)
        return;
    int distance;
    while (!iss3.eof()) {
        iss3 >> distance;
        l.intervals.push_back(distance + basic);
    }
    // I don't know why the last one is loaded twice
    l.intervals.pop_back();
}


void PDFFile::readCompressedFile(const char* encodedFile,
        unordered_map<string, string>& encodeBook){
    ifstream f(encodedFile);
    string linebuf;

    getline(f, linebuf); // Page
    while(!f.eof()) {
        Page page;
        getline(f, linebuf); // Stream
        while (1) {
            if (linebuf == "Page")
                break;
            Stream stream;
            getline(f, linebuf);  // Paragraph

            while (1) {
                if (linebuf == "Stream" || linebuf == "Page")
                    break;
                Paragraph para;
                getline(f, linebuf);
                istringstream iss(linebuf);
                string temp1, temp2;
                iss >> temp1 >> para.fontType >> temp2;
                para.fontSize = stof(temp2);
                getline(f, linebuf);

                while (1) {
                    // "Paragraph" || "Page"
                    if (linebuf[0] == 'P')
                        break;
                    Line l;
                    getDisplacement(linebuf, l);
                    getline(f, linebuf);

                    // getWords()
                    int len = linebuf.size();
                    for (int i = 0; i < len; i += 3)
                        l.words.push_back(encodeBook[linebuf.substr(i, 3)]);  // can't const ??
                    getline(f, linebuf);

                    getDistance(linebuf, l);
                    para.contents.push_back(l);
                    getline(f, linebuf);

                    // end of file (return condition)
                    if (linebuf == "") {
                        stream.paragraphs.push_back(para);
                        page.streams.push_back(stream);
                        pages.push_back(page);
                        return;
                    }
                }
                stream.paragraphs.push_back(para);
            }
            if (linebuf == "Stream" || linebuf == "Page")
                page.streams.push_back(stream);
        }
        pages.push_back(page);
    }

}



#endif //PDF_DECOMPRESS_H
