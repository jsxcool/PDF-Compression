#ifndef PDF_READIN_H
#define PDF_READIN_H
/*
 * read in decompressed pdf file
 */

#include "PDFObject.h"
#include "parse.h"
#include <chrono>

using namespace std;
// queue<string> rawData;
mutex mtx2;

// only works for pure text (ignore images)
void PDFFile::readIn() {
    string lineBuf;
    while (!done || (done && !rawData.empty())) {
        // two few data, sleep for a moment
        if (!done && rawData.size() < 100) {
            this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        mtx2.lock();

        lineBuf = rawData.front();
        rawData.pop();
        if (lineBuf == "BPage") {  // Begin Page
            Page page;
            while (1) {
                // iterating
                lineBuf = rawData.front();
                rawData.pop();
                // break condition
                if (lineBuf == "EPage")
                    break;
                if (lineBuf == "BT") { // Begin Text
                    Stream stream;
                    while (1) {
                        // new stream(only one paragraph in this stream)
                        if(lineBuf[0] != '/') {
                            lineBuf = rawData.front();
                            rawData.pop();
                        }
                        if (lineBuf == "ET")
                            break;
                        Paragraph para;
                        bool gotFont = false;

                        while (1) {
                            // not enough data, sleep for a moment
                            if (!done && rawData.size() < 10) {
                                mtx2.unlock();
                                this_thread::sleep_for(std::chrono::milliseconds(1));
                                mtx2.lock();
                                continue;
                            }

                            if (!gotFont) {
                                para.fontType = lineBuf;
                                lineBuf = rawData.front();
                                rawData.pop();
                                para.fontSize = atof(&lineBuf[0]);
                                lineBuf = rawData.front();  // Tf
                                rawData.pop();
                                lineBuf = rawData.front();  // l.x
                                rawData.pop();
                                gotFont = true;
                            }

                            Line l;
                            l.x = atof(&lineBuf[0]);
                            lineBuf = rawData.front();
                            rawData.pop();
                            l.y = atof(&lineBuf[0]);
                            lineBuf = rawData.front();  // Td
                            rawData.pop();

                            lineBuf = rawData.front();
                            rawData.pop();
                            // the distance of a full space
                            int space = para.fontSize * Point;
                            // get the words and intervals of Line l
                            extractWords(lineBuf, space, l);

                            para.contents.push_back(l);

                            lineBuf = rawData.front();  // TJ
                            rawData.pop();
                            lineBuf = rawData.front();  // '/F..' or l.x or "ET"
                            rawData.pop();
                            if (lineBuf[0] == '/' || lineBuf == "ET")
                                break;
                        }
                        stream.paragraphs.push_back(para);
                        if (lineBuf == "ET")
                            break;
                    }
                    page.streams.push_back(stream);
                }

            }
            pages.push_back(page);
        }
        mtx2.unlock();
    }
}


int PDFFile::calculateDistance(const string& str, int& pos) {
    string num;
    while (str[pos] != ' ') {
        num += str[pos];
        pos++;
    }
    return atof(&num[0]);
}

string PDFFile::getText(const string& str, int& pos) {
    string ret;
    while (str[pos] != ')') {
        ret += str[pos];
        pos++;
    }
    return ret;
}

void PDFFile::extractWords(const string& str, int space, Line& l) {
    int len = str.size();
    // Treat it as a interval. 0.5 suitable ?
    int thre = space * 0.5;

    // solve the first word by hand
    int i = 3;
    string temp;
    while (str[i] != ')' && str[i] != '>') {
        temp += str[i];
        i++;
    }
    l.words.push_back(temp);

    bool wordEnd = false;
    while (i < len) {
        if (str[i] == '-') {
            i++;
            int val = calculateDistance(str, i);
            if (val > thre) {
                l.intervals.push_back(val);
                wordEnd = true;
            }
        } else if (str[i] == '(') {
            i++;
            if (wordEnd) {
                l.words.push_back(getText(str, i));
                wordEnd = false;
            } else
                l.words.back() += getText(str, i);
        } else
            i++;
    }

}



#endif //PDF_READIN_H
