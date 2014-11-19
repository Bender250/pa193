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
    asterix,
    lBra,
    rBra,
    //backslash,
    NonterminalsCount
};

typedef std::pair< Tokens, std::string::size_type > Tokenized;

class TokenizedComparator
{
public:
    /**
     * @brief operator () overloaded operator () as comparator
     *
     * compare given pairs by position = Tokenized.second parameter
     * @return false is a is token with lower or equal position, else true
     */
    bool operator()(const Tokenized &a, const Tokenized &b) const {
        return (a.second < b.second);
    }
};

class Tokenizer
{
public:
    /**
     * @brief Tokenizer ctor - initializes tokenArray
     */
    Tokenizer();
    /**
     * @brief ~Tokenizer dtor - release resources - mainly tokenTree
     */
    ~Tokenizer();

    /**
     * @brief tokenize main fuction for running tokenization
     *
     * take input and finds tokens in it, which are saved in tokenTree
     * complexity is O(sizeof(input)*sizeof(Tokens))
     *
     * @param input reference to string created from given file
     * @return tokenTree with structure of tokens
     */
    std::set<Tokenized, TokenizedComparator> tokenize(std::string &input);

private:
    std::array< std::string, static_cast< std::size_t > (Tokens::NonterminalsCount) > tokenArray;
    std::set< Tokenized, TokenizedComparator > tokenTree;
    /**
     * @brief removeBackslashes remove all backslashes and next characters
     *
     * backslashes in C/C++ are parsed by preprocessor as escaped char
     * we don't parse C/C++, so we can delete them with the next character
     *
     * @param input reference to string, in which are '\' removed
     */
    void removeBackslashes(std::string &input);
};

#endif // TOKENIZER_H
