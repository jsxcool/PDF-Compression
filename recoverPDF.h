#ifndef PDF_RECOVERPDF_H
#define PDF_RECOVERPDF_H

// !!! postScript is a one page file

#include <iostream>
#include <fstream>
#include "PDFObject.h"
using namespace std;

class Postscript {
private:
    ofstream file;
public:
    Postscript(const string& name){
        file.open (name);    //equals ofstream file(name)
        file << "%!\n";  // indicate it's ps file
    }

    ~Postscript() {file.close();}

    // the default is 0,0,0 (black)
    void setColor(float r=0, float g=0, float b=0){
        file << r <<' '<<g <<' '<<b<<" setrgbcolor\n";
    }

    void setFont(const string& type, float size){
        file << type << " findfont " << size << " scalefont setfont newpath\n";
    }

    void writeText(int x, int y, const string &t) {
        file << x << " " << y << " moveto " << "(" << t << ") show\n";
    }

};

// ignored intervals
// font type is not converted
void PDFFile::generatePs(const char* filename){
    // only do the first page
    for(int i=0; i<1; i++){
        Page page = pages[i];
        Postscript ps(filename);
        ps.setColor();
        for(const Stream& strm : page.streams){
            for(const Paragraph& para : strm.paragraphs){
                ps.setFont(para.fontType, para.fontSize);
                for(const Line& line: para.contents){
                    string temp;
                    for(const string& str : line.words)
                        temp += str + ' ';
                    static int curx = 0, cury = 0;
                    curx += line.x;
                    cury += line.y;
                    ps.writeText(curx, cury, temp);
                }

            }
        }
    }

}

//command: ps2pdf filename.ps
























#endif //PDF_RECOVERPDF_H
