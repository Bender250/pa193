#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <iostream>
#include <iterator>
#include <string>
#include <array>
#include <set>
#include <tuple>
#include <cstdint>

enum class Tokens : std::uint8_t {
    all,
        fileHeader,
            headerCommentLine,
                atAuthor,
                atVersion,
                atFile,
                keyName,
        pair,
            comment,
                commentBegin,
                commentEnd,
                commentLine,
                    atBrief,
                    atParam,
                    atReturn,
            function,
                functionHeader,
                    type,
                    functionName,
                    lPar,
                    rPar,
                    param,
                    comma,
        text,
        space,
        newLine,
        tab,
    at,
    doubleQuotes,
    singleQuotes,
    cppComment,
    cCommentBegin,
    cCommentEnd,
    //backslash,
    NonterminalsCount
};

typedef std::pair< Tokens, std::string::size_type > Tokenized;

class TokenizedComparator
{
public:
    bool operator()(const Tokenized &a, const Tokenized &b) const {
        return (a.second < b.second);
    }
};

class Tokenizer
{
public:
    Tokenizer();
    ~Tokenizer();

    std::set<Tokenized, TokenizedComparator> tokenize(std::string &input);

private:
    std::array< std::string, static_cast< std::size_t > (Tokens::NonterminalsCount) > tokenArray;
    std::set< Tokenized, TokenizedComparator > tokenTree;
    void removeBackslashes(std::string &input);
};

#endif // TOKENIZER_H
