#include "tokenizer.h"

/*TODO: solve problem with backslash, according manual (delete all pairs of character \*)*/

Tokenizer::Tokenizer()
{
    //init tokens
    tokenArray[ static_cast< std::size_t > (Tokens::commentBegin) ] = "/**";
    tokenArray[ static_cast< std::size_t > (Tokens::commentEnd) ] = "*/";
    tokenArray[ static_cast< std::size_t > (Tokens::atAuthor) ] = "@author";
    tokenArray[ static_cast< std::size_t > (Tokens::atVersion) ] = "@version";
    tokenArray[ static_cast< std::size_t > (Tokens::atFile) ] = "@file";
    tokenArray[ static_cast< std::size_t > (Tokens::atBrief) ] = "@brief";
    tokenArray[ static_cast< std::size_t > (Tokens::atParam) ] = "@param";
    tokenArray[ static_cast< std::size_t > (Tokens::atReturn) ] = "@return";
    tokenArray[ static_cast< std::size_t > (Tokens::lPar) ] = "(";
    tokenArray[ static_cast< std::size_t > (Tokens::rPar) ] = ")";
    tokenArray[ static_cast< std::size_t > (Tokens::comma) ] = ",";
    tokenArray[ static_cast< std::size_t > (Tokens::space) ] = " ";
    tokenArray[ static_cast< std::size_t > (Tokens::newLine) ] = "\n";
    tokenArray[ static_cast< std::size_t > (Tokens::tab) ] = "\t";

    tokenArray[ static_cast< std::size_t > (Tokens::at) ] = "@";
    tokenArray[ static_cast< std::size_t > (Tokens::cCommentBegin) ] = "/*";
    tokenArray[ static_cast< std::size_t > (Tokens::cCommentEnd) ] = "*/";
    tokenArray[ static_cast< std::size_t > (Tokens::cppComment) ] = "//";
    tokenArray[ static_cast< std::size_t > (Tokens::singleQuotes) ] = "'";
    tokenArray[ static_cast< std::size_t > (Tokens::doubleQuotes) ] = "\"";
    //tokenArray[ static_cast< std::size_t > (Tokens::backslash) ] = "\\";
}

Tokenizer::~Tokenizer()
{
    tokenTree.clear();
}

std::set< Tokenized, TokenizedComparator > Tokenizer::tokenize(std::string &input)
{
    removeBackslashes(input);

    for (Tokens token = Tokens::all;
        token < Tokens::NonterminalsCount;
        token = (Tokens)(std::underlying_type<Tokens>::type(token) + 1)) {

        std::string tokenString = tokenArray[ static_cast< std::size_t > (token) ];
        if (tokenString.empty())
            continue;

        std::string::size_type pos = 0;

        while ((pos = input.find(tokenString, pos)) != std::string::npos) {
            Tokenized tmpToken = std::make_pair(token, pos);
            tokenTree.insert(tmpToken);
            ++pos; //search after found position
        }
    }
    return tokenTree;
}

void Tokenizer::removeBackslashes(std::string &input)
{
    std::string::size_type pos = 0;

    while ((pos = input.find("\\", pos)) != std::string::npos) {
        input.erase(pos, 2);
    }
}
