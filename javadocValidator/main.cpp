/**
 * @author Team A
 * @file main.cpp
 * @brief main function for running the parser
 *
 * @link https://github.com/Bender250/pa193/wiki for more informations
 * about supported things
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "tokenizer.h"
#include "parser.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0]
                  << " fileToCheck" << std::endl
                  << "fileToCheck should be in specified without path, "
                  << "just name of file, so it correspondes with "
                  << "@file in header of file"
                  << std::endl;
        return -1;
    }

    ifstream input;
    input.open(argv[1], ios_base::in);
    if (!input.is_open()) {
        std::cout << "Given file could not be open" << std::endl
                  << argv[1]
                  << " should be valid path."
                  << std::endl;
        return -1;
    }
    string inputString(static_cast< stringstream const& >
                       (stringstream() << input.rdbuf()).str());


    Tokenizer t;
    auto tree = t.tokenize(inputString);
    Parser p(inputString, argv[1]);
    p.initList(tree);
    if (p.parseFile()) {
        cout << "Input is valid" << endl << endl;
    }
    else {
        cout << "Input is invalid" << endl << endl;
    }

    input.close();
    return 0;
}

