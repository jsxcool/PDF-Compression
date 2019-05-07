#ifndef PDF_COMPRESSALGORITHM_H
#define PDF_COMPRESSALGORITHM_H

#include <iostream>
#include <vector>
#include "readin.h"
#include "encodeMap.h"
using namespace std;

int PDFFile::compressLine(unordered_map<string, string>& encodeBook, const Line& lines,
                          string wordsCoded[]){
    string word;
    // dynamic code starts from
    static string code = "a00";
    auto itr = lines.words.begin();
    while(itr != lines.words.end()){
        word = *itr;
        auto got = encodeBook.find(word);
        if(got != encodeBook.end()){
            *wordsCoded = encodeBook[word];
        }
        else{
            dynamicCode.push_back(pair<string, string>(word, code));
            encodeBook[word] = code;
            *wordsCoded = encodeBook[word];
            encodeNext(code);
        }
        wordsCoded++;
        itr++;
    }

    int basic = lines.shortenInterval();
    return basic;
}


void PDFFile::write(const char* out, unordered_map<string, string>& encodeBook){
    ofstream fout(out);
    for(auto itr1=pages.begin(); itr1!=pages.end(); itr1++) {
        fout << "Page\n";
        for (auto itr2 = itr1->streams.begin(); itr2 != itr1->streams.end(); itr2++) {
            fout << "Stream\n";
            for (auto itr3 = itr2->paragraphs.begin(); itr3 != itr2->paragraphs.end(); itr3++){
                fout << "Paragraph" << '\n';
                fout << "FontType " << itr3->fontType << ' ' << itr3->fontSize << '\n';
                for (auto itr4 = itr3->contents.begin(); itr4 != itr3->contents.end(); itr4++) {
                    fout << itr4->x << ' ' << itr4->y << '\n';

                    int len = itr4->words.size();
                    string wordsCoded[len];
                    int basic = compressLine(encodeBook, *itr4, wordsCoded);
                    for(int i=0; i<len; i++)
                        fout << wordsCoded[i];
                    fout << '\n';
                    fout << basic << ' ';
                    //cout << itr4->intervals.size() << ' ' << len-1 << '\n';
                    for(int i=0; i<len-1; i++)
                        fout << itr4->intervals[i]-basic << ' ';
                    fout << '\n';
                }
            }
        }
    }

    fout << "\ndynamic mapping zone:\n";
    for(auto p : dynamicCode)
        // '\n' is not necessary
        fout << p.first << ' ' << p.second << '\n';

}




#endif //PDF_COMPRESSALGORITHM_H
