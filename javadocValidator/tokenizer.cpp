#include "tokenizer.h"

Tokenizer::Tokenizer()
{
    //init tokens
    tokenArray[ static_cast< std::size_t > (Tokens::commentBegin) ] = "/**";
    tokenArray[ static_cast< std::size_t > (Tokens::commentEnd) ] = "*/";
    tokenArray[ static_cast< std::size_t > (Tokens::atAuthor) ] = "@author";
    tokenArray[ static_cast< std::size_t > (Tokens::atVersion) ] = "@version";
    tokenArray[ static_cast< std::size_t > (Tokens::atBrief) ] = "@brief";
    tokenArray[ static_cast< std::size_t > (Tokens::atParam) ] = "@param";
    tokenArray[ static_cast< std::size_t > (Tokens::atReturn) ] = "@return";
    tokenArray[ static_cast< std::size_t > (Tokens::lPar) ] = "(";
    tokenArray[ static_cast< std::size_t > (Tokens::rPar) ] = ")";
    tokenArray[ static_cast< std::size_t > (Tokens::comma) ] = ",";
    tokenArray[ static_cast< std::size_t > (Tokens::space) ] = " ";
    tokenArray[ static_cast< std::size_t > (Tokens::newLine) ] = "\n";
}

std::set< Tokenized, TokenizedComparator > Tokenizer::tokenize(std::string &input)
{
    for (Tokens token = Tokens::commentBegin;
         token < Tokens::count;
         token = (Tokens)(std::underlying_type<Tokens>::type(token) + 1)) {

        std::string tokenString = tokenArray[ static_cast< std::size_t > (token) ];
        //internal loop
        std::string::size_type pos = 0;
        while ((pos = input.find(tokenString, pos)) != std::string::npos) {
            Tokenized tmpToken = std::make_pair(token, pos);
            tokenTree.insert(tmpToken);
            ++pos; //search after found position
        }
    }
    return tokenTree;
}
