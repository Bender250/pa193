#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "tokenizer.h"
#include "parser.h"

#define FILE_NAME "../input.c"

using namespace std;

int main()
{
    //parse input
    ifstream input;
    input.open(FILE_NAME, ios_base::in); //retval!
    string inputString(static_cast< stringstream const& >
                       (stringstream() << input.rdbuf()).str());

    Parser p(inputString, FILE_NAME);
    Tokenizer t;
    p.initList(t.tokenize(inputString));
    p.parseFile();
    //avare of empty input
    cout << "Hello World!" << endl << endl;
    cout << inputString << endl;

    input.close();
    return 0;
}

