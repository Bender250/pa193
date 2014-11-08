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
    commentBegin,
    commentEnd,
    atAuthor,
    atVersion,
    atBrief,
    atParam,
    atReturn,
    lPar,
    rPar,
    comma,
    space,
    newLine,
    count
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

    std::set<Tokenized, TokenizedComparator > tokenize(std::string &input);

private:
    std::array< std::string, static_cast< std::size_t > (Tokens::count) > tokenArray;
    std::set< Tokenized, TokenizedComparator > tokenTree;

};

#endif // TOKENIZER_H
