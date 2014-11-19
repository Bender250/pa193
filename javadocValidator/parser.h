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

    ////        Filter and check            /////
    /**
     * @brief filterUnreachableNontokens deletes commeted (nodoxygen) sections, strings and chars
     *
     * initial run of parser deletes unnecessary parts:
     *      C comments slashAsterix contentOfComment asterixSlash
     *      C++ comments slashSlash
     *      doubleQuotes string doubleQuotes
     *      singleQuotes 0/1 char singleQuotes (0 chars in case of deleted
     *          escaped character)
     *
     * @return true in case of valid input, else false
     */
    bool filterUnreachableNontokens();

    /**
     * @brief filterRepeatingWhitespace optimalization for decrease number of tokens
     *
     * not necessary function
     */
    void filterRepeatingWhitespace();

    /**
     * @brief checkForBadKeyword checks, if is in document used @ with unknown keyword
     * @param it position where to check keyword
     *
     * doesn't move the iterator
     * unknow keyword is treated as warning, not error
     */
    void checkForBadKeyword(std::list< Tokenized >::iterator it);

    ////        Main parsing part            /////
    /**
     * @brief parseHeader parse header comment with file, author and version
     * @param it position where to start with searching for header
     * @return correct header = true, incorrect = false and reason is writen to stdout
     */
    bool parseHeader(std::list< Tokenized >::iterator it);
    /**
     * @brief iterateTroughtDocumentedFunctions
     * @return
     */
    bool iterateTroughtDocumentedFunctions();


    ////          Auxiliary functions          /////
    bool handleDoxygenComment(std::list< Tokenized >::iterator& it,
                              std::tuple< std::string, std::set< std::string >,
                                          std::string> &out);
    bool handleFunction(std::list< Tokenized >::iterator& it,
                        std::tuple< std::string, std::set< std::string >,
                                    std::string > &function);

    void printArguments(std::set< std::string > dox, std::set< std::string > fun);


    std::string::size_type getEnd(std::list< Tokenized >::iterator it);
    std::string getPreviousWord(std::list< Tokenized >::iterator it);
    std::string getTextLine(std::list< Tokenized >::iterator &it);

    bool isDoxygenComment(std::list< Tokenized >::iterator it);
    bool isSpaceOrTab(std::list<Tokenized>::iterator it);
    std::string getNextWord(std::list<Tokenized>::iterator &it);
    bool isKeyword(std::list<Tokenized>::iterator it);
};

#endif // PARSER_H
