#ifndef PARSER_H
#define PARSER_H
#include <cstdint>
#include <list>
#include <iterator>
#include <algorithm>
#include <tuple>

#include "tokenizer.h"

class Parser
{
public:
    Parser(std::string &input, std::string fileName);
    ~Parser();
    void initList(std::set< Tokenized, TokenizedComparator > inputSet);
    bool parseFile();

private:
    std::list< Tokenized > nonterminalsList;
    std::string input;
    std::string fileName;

    bool filterUnreachableNontokens();
    void filterQuotedNontokens();
    void filterCommentedNontokens();
    void filterRepeatingWhitespace();

    void checkForBadKeyword(std::list< Tokenized >::iterator it);

    bool parseHeader(std::list< Tokenized >::iterator it);
    std::string::size_type getEnd(std::list< Tokenized >::iterator it);
    std::string getTextLine(std::list< Tokenized >::iterator &it);

    bool isDoxygenComment(std::list< Tokenized >::iterator it);
    bool isSpaceOrTab(std::list<Tokenized>::iterator it);
    std::string getNextWord(std::list<Tokenized>::iterator &it);
    bool isKeyword(std::list<Tokenized>::iterator it);
};

#endif // PARSER_H
