/**
  * @author Team A
  * @file ../javadocValidator/parser.h
  *
  * @brief class Parser checks, if Tokens are valid input
  */

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
    /**
     * @brief Parser ctor
     * @param code string with code to be parsed
     * @param fileName name of the file, which is parsed
     */
    Parser(std::string &code, std::string fileName);
    /**
     * @brief ~Parser dtor
     */
    ~Parser();
    /**
     * @brief initList initialize Parser with input set
     * @param inputSet parsed tokens
     */
    void initList(std::set< Tokenized, TokenizedComparator > inputSet);
    /**
     * @brief parseFile runs the validation
     * @return true if file is valid, false otherwise
     */
    bool parseFile();

private:
    std::list< Tokenized > nonterminalsList;
    std::string code;
    std::string fileName;

    /*        Filter and check            */
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

    /*        Main parsing part            */
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


    /*          Auxiliary functions          */
    /**
     * @brief handleDoxygenComment
     * @param it
     * @param params
     * @return
     */
    bool handleDoxygenComment(std::list< Tokenized >::iterator& it,
                                      std::set< std::string > &params);
    /**
     * @brief handleFunction
     * @param it
     * @param params
     * @return
     */
    bool handleFunction(std::list< Tokenized >::iterator& it,
                        std::set< std::string > &params);
    /**
     * @brief printArguments
     * @param dox
     * @param fun
     */
    void printArguments(std::set< std::string > dox, std::set< std::string > fun);


    /**
     * @brief getEnd
     * @param it
     * @return
     */
    std::string::size_type getEnd(std::list< Tokenized >::iterator it);
    /**
     * @brief getPreviousWord
     * @param it
     * @return
     */
    std::string getPreviousWord(std::list< Tokenized >::iterator it);
    /**
     * @brief getTextLine
     * @param it
     * @return
     */
    std::string getTextLine(std::list< Tokenized >::iterator &it);

    /**
     * @brief isDoxygenComment
     * @param it
     * @return
     */
    bool isDoxygenComment(std::list< Tokenized >::iterator it);
    /**
     * @brief isSpaceOrTab
     * @param it
     * @return
     */
    bool isSpaceOrTab(std::list< Tokenized >::iterator it);
    /**
     * @brief getNextWord
     * @param it
     * @return
     */
    std::string getNextWord(std::list< Tokenized >::iterator &it);
    /**
     * @brief isKeyword
     * @param it
     * @return
     */
    bool isKeyword(std::list< Tokenized >::iterator it);
    /**
     * @brief getAttribute
     * @param it
     * @return
     */
    std::string getAttribute(std::list< Tokenized >::iterator it);
};

#endif // PARSER_H
