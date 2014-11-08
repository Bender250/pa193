#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "tokenizer.h"

#define FILE_NAME "../input.c"

using namespace std;

int main()
{
    //parse input
    ifstream input;
    input.open(FILE_NAME, ios_base::in); //retval!
    string inputString(static_cast< stringstream const& >
                       (stringstream() << input.rdbuf()).str());

    Tokenizer t;
    t.tokenize(inputString);
    cout << "Hello World!" << endl;
    return 0;
}

