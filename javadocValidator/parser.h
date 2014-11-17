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

    bool handleDoxygenComment(std::list< Tokenized >::iterator& it,
                              std::tuple< std::string, std::set< std::string >,
                                          std::string> &out);
    bool handleFunction(std::list< Tokenized >::iterator& it,
                        std::tuple< std::string, std::set< std::string >,
                                    std::string > &function);
    std::string getPreviousWord(std::list< Tokenized >::iterator it);
private:
    std::list< Tokenized > nonterminalsList;
    std::string input;
    std::string fileName;

    bool filterUnreachableNontokens();
    void filterQuotedNontokens();
    void filterCommentedNontokens();
    void filterRepeatingWhitespace();

    bool iterateTroughtDocumentedFunctions();

    void printArguments(std::set< std::string > dox, std::set< std::string > fun);

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
