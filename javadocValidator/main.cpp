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

    //TODO: call tokenizer from parser to share same input string
    Tokenizer t;
    auto tree = t.tokenize(inputString); //this is temporal solution for TODO problem
    Parser p(inputString, FILE_NAME);
    p.initList(tree);
    if (p.parseFile()) {
        cout << "Input is valid" << endl << endl;
    }
    else {
        cout << "Input is invalid" << endl << endl;
    }
    //avare of empty input
    cout << "Hello World!" << endl << endl;
    cout << inputString << endl;

    input.close();
    return 0;
}

