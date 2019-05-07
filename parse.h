// ACKNOWLEDGMENT: Citing from QPDF project

#ifndef PDF_PARSE_H
#define PDF_PARSE_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <queue>
#include <mutex>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFPageObjectHelper.hh>
#include <qpdf/QUtil.hh>

std::queue<std::string> rawData;
bool done = false;
std::mutex mtx;

class ParserCallbacks: public QPDFObjectHandle::ParserCallbacks{
public:
    virtual ~ParserCallbacks() {}

    virtual void handleObject(QPDFObjectHandle);
    virtual void handleEOF();
};

void ParserCallbacks::handleObject(QPDFObjectHandle obj){
    mtx.lock();
    if (obj.isInlineImage()){
        rawData.push(QUtil::hex_encode(obj.getInlineImageValue()));
    }
    else{
        rawData.push(obj.unparse());
    }
    mtx.unlock();
}

// which means this page ends
void ParserCallbacks::handleEOF(){
    mtx.lock();
    rawData.push("EPage");
    mtx.unlock();
}

void loadRawData(char const* filein){
    QPDF pdf;
    pdf.processFile(filein);
    std::vector<QPDFPageObjectHelper> pages =
            QPDFPageDocumentHelper(pdf).getAllPages();

    int len = pages.size();

    ParserCallbacks cb;
    for(int i=0; i<len; i++){
        mtx.lock();
        rawData.push("BPage");
        mtx.unlock();
        QPDFPageObjectHelper& page = pages.at(i);
        page.parsePageContents(&cb);
    }
    done = true;
}



#endif //PDF_PARSE_H
