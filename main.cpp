#include <iostream>
#include <thread>
#include "readin.h"
#include "compressAlgorithm.h"
#include "decompress.h"
#include "recoverPDF.h"
// queue<string> rawData;

using namespace std;

void compress(const char* in, const char* out){
    // main thread: we could also call it thread t3
    //EncodeMap mymap; // 125 ms
    thread t1(loadRawData, in);
    EncodeMap mymap;  // 94 ms
    PDFFile mypdf;
    thread t2(&PDFFile::readIn, &mypdf);
    //EncodeMap mymap; // 105 ms

    t1.join();
    t2.join();
    //EncodeMap mymap; // 117ms
    mypdf.write(out, mymap.encodeBook);
}

// Without Multithreading, 103ms
void compress2(const char* in, const char* out){
    loadRawData(in);
    EncodeMap mymap;
    PDFFile mypdf;
    mypdf.readIn();
    mypdf.write(out, mymap.encodeBook);
}

void decompress(const char* in, PDFFile& mypdf){
    EncodeMap mymap(false);
    mymap.loadDynamicCode(in);
    mypdf.readCompressedFile(in, mymap.encodeBook);
}


int main(int argc, char* argv[]){
    if(argc != 4){
        cout << "please input the correct command\n";
        return -1;
    }

    string opt = argv[1];

    // compress
    if(opt == "-c"){
        compress(argv[2], argv[3]);
        return 0;
    }

    // decompress
    if(opt == "-d"){
        PDFFile mypdf;
        decompress(argv[2], mypdf);
        mypdf.generatePs(argv[3]);
    }
    // ps2pdf gstest.gs
    // got original pdf file

}


// future work:
// (1) dynamic area should be in the front (not able)
// or how to memory the line num
// (2) compressAlgorithm: find bizp2's API
// (3) write PDFObject to ghostScript
// ghostScript has API to convert gs to pdf

// Improvement for Multithread: parse pages parallel


// original: 39 kb
// mine: 7.2 kb     18.46%
// bzip2: 6.9 kb    17.69%
