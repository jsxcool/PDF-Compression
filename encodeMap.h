#ifndef PDF_ENCODEMAP_H
#define PDF_ENCODEMAP_H

#include <unordered_map>
#include <fstream>
#include <sstream>
using namespace std;

// 62 + 7 + 6 = 85 decimal rule:
// 0~9, :~@, A~Z, [~`, a~z
void encodeNext(string& s){
    if(s[2] == 'z'){
        s[2] = '0';
        if(s[1] == 'z'){
            s[1] = '0';
            s[0]++;
        }
        else
            s[1]++;
    }
    else
        s[2]++;
}

class EncodeMap{
private:
    void To85Decimal(const char* filename, const char* out){
        ifstream fin(filename);
        ofstream fout(out);

        string buffer;
        string word;
        int count;
        string encode = "000";
        getline(fin, buffer); // skip the first line
        while(!fin.eof()){
            getline(fin, buffer);
            istringstream iss(buffer);
            iss >> word;
            iss >> count;
            if(count < 2)
                break;
            // Uppercase, ',', '.'
            fout << word << ' ' << encode << '\n';
            encodeNext(encode);
            fout << word << ", " << encode << '\n';
            encodeNext(encode);
            fout << word << ". " << encode << '\n';
            encodeNext(encode);
            word[0] = word[0] - 32;
            fout << word << ' ' << encode << '\n';
            //encode++
            encodeNext(encode);
        }

        fin.close();
        fout.close();
    }

    // English word -- 85decimal
    void readInDictionary(const char* filename){
        ifstream f(filename);
        string word, code;
        while(!f.eof()){
            f >> word >> code;
            encodeBook[word] = code;
        }
        f.close();
    }

    void readInDictionaryReverse(const char* filename){
        ifstream f(filename);
        string word, code;
        while(!f.eof()){
            f >> word >> code;
            encodeBook[code] = word;
        }
        f.close();
    }

public:
    unordered_map<string, string> encodeBook;
    EncodeMap(){
        readInDictionary("to85Decimal.txt");
    }

    EncodeMap(bool reverse){
        readInDictionaryReverse("to85Decimal.txt");
    }

    void loadDynamicCode(const char* encodedFile);
};


#endif //PDF_ENCODEMAP_H
