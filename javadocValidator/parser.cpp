#include "parser.h"

Parser::Parser(std::string &input, std::string fileName)
    : input(input), fileName(fileName)
{
}

Parser::~Parser()
{
    nonterminalsList.clear();
}

void Parser::initList(std::set<Tokenized, TokenizedComparator> inputSet)
{
    std::list< Tokenized > tmpList(inputSet.begin(), inputSet.end());
    nonterminalsList = tmpList;
}

bool Parser::parseFile()
{
    filterRepeatingWhitespace();

    if (!filterUnreachableNontokens())
        return false;
    if (!parseHeader(nonterminalsList.begin()))
        return false;
    return true;
}

bool Parser::filterUnreachableNontokens()
{
    std::list< Tokenized >::iterator beginIt;
    for (auto it = nonterminalsList.begin(); it != nonterminalsList.end(); ++it)
        switch (it->first) {
        case Tokens::commentBegin: {
            //skip doxygen comment
            while (it->first != Tokens::commentEnd) {
                ++it;
                if (it == nonterminalsList.end()) {
                    std::cout << "Error: code has unfinished doxygen comment"
                              << std::endl;
                    return false;
                }
            }
            break;
        }
        case Tokens::cppComment: {
            beginIt = it; //found cpp comment begin

            while (it->first != Tokens::newLine && it != nonterminalsList.end())
                ++it;
            //it is now end of comment
            nonterminalsList.erase(beginIt, it); //delete comment
            break;
        }
        case Tokens::cCommentBegin: {
            beginIt = it; //found c comment begin
            if (isDoxygenComment(it))
                ++it; //skip doxygen comment

            //handle nondoxygen comment
            else {
                while (it->first != Tokens::cCommentEnd) {
                    if (it != nonterminalsList.end()) {
                        std::cout << "Warning: code has unfinished cpp comment"
                                  << std::endl;
                    }
                    ++it;
                    break;
                }

                //it is now end of comment
                nonterminalsList.erase(beginIt, it);
            }
            break;
        }
        case Tokens::doubleQuotes: {
            beginIt = it; //found double quote begin

            while (it->first != Tokens::cCommentEnd) {
                if (it != nonterminalsList.end()) {
                    std::cout << "Warning: code has unfinished cpp comment"
                              << std::endl;
                    break;
                }
                /*if (isPreviousCharacterBackslash(it)) {
                    ++it;
                    continue;
                }*/
                ++it;
            }
            nonterminalsList.erase(beginIt, it);
            break;
        }
        default:
            break;
        }
    return true;
}

void Parser::filterRepeatingWhitespace()
{
    auto tmpIt = nonterminalsList.begin();
    for (auto it = nonterminalsList.begin(); it != nonterminalsList.end(); ++it) {
        if (isSpaceOrTab(it)) {
            //have to delete next whitespace, to correctly hold information about ends of words
            auto position = it->second;
            ++it;
            tmpIt = it;

            while (isSpaceOrTab(it) && position == (it->second - 1)) {
                ++position;
                ++it;
            }

            if (tmpIt != it)
                nonterminalsList.erase(tmpIt, it);
        }
        //different behavior for newlines is beacuse is doxygen, we distinguish between space and newlines
        if (it->first == Tokens::newLine) {
            //have to delete next whitespace, to correctly hold information about ends of words
            auto position = it->second;
            ++it;
            tmpIt = it;

            while (it->first == Tokens::newLine && position == (it->second - 1)) {
                ++position;
                ++it;
            }

            if (tmpIt != it)
                nonterminalsList.erase(tmpIt, it);
        }
    }
}

bool Parser::isSpaceOrTab(std::list< Tokenized >::iterator it) {
    return (it->first == Tokens::space || it->first == Tokens::tab);
}

bool Parser::parseHeader(std::list< Tokenized >::iterator it)
{
    if (it->first != Tokens::commentBegin) {
        std::cout << "Warning: Expected token commentBegin, got: "
                  << input.substr(0, getEnd(it))
                  << std::endl
                  << "File should start with comment with @author value"
                  << std::endl;
        //move to first comment
        while (it->first != Tokens::commentBegin) {
            auto tmpIt = it;
            ++it;
            nonterminalsList.erase(tmpIt);
        }
    }
    bool hasFile = false;
    bool hasVersion = false;
    bool hasAuthor = false;
    bool hasBrief = false;
    for(++it; it->first != Tokens::commentEnd; ++it) {
        switch (it->first) {
        case Tokens::atAuthor: {
            if (hasAuthor) {
                std::cout << "Warning: author command repeated" << std::endl;
            }

            if (getTextLine(it).empty()) {
                std::cout << "Error: author position is empty" << std::endl;
                return false;
            }

            hasAuthor = true;
            break;
        }
        case Tokens::atBrief: {
            if (hasBrief) {
                std::cout << "Error: brief command repeated" << std::endl;
                return false;
            }

            if (getTextLine(it).empty()) {
                std::cout << "Error: brief position is empty" << std::endl;
                return false;
            }

            hasBrief = true;
            break;
        }
        case Tokens::atFile: {
            if (hasFile) {
                std::cout << "Error: file command repeated" << std::endl;
                return false;
            }

            auto file = getTextLine(it);
            if (file.empty()) {
                std::cout << "Error: file position is empty" << std::endl;
                return false;
            }

            if (file != fileName) {
                std::cout << "Error: file name doesnt match, expcted:"
                          << fileName
                          << " , got: "
                          << file
                          << std::endl;
                return false;
            }

            hasFile = true;
            break;
        }
        case Tokens::atVersion: {
            if (hasVersion) {
                std::cout << "Error: version command repeated" << std::endl;
                return false;
            }

            hasVersion = true;
            break;
        }
        default:
            break;
        }
    }
    return true;
}

std::string::size_type Parser::getEnd(std::list< Tokenized >::iterator it)
{
    ++it;
    return it->second;
}

std::string Parser::getTextLine(std::list< Tokenized >::iterator &it)
{
    auto begin = it;
    while(it->first != Tokens::newLine && it != nonterminalsList.end())
        ++it;

    return input.substr(begin->second, it->second); //TODO: test if returns \0
}

bool Parser::isDoxygenComment(std::list< Tokenized >::iterator it)
{
    auto tmpIt = it;
    ++tmpIt;
    if (tmpIt->first == Tokens::commentBegin)
        return true;
    --it;
    if (it->first == Tokens::commentBegin)
        return true;

    return false;
}

/*bool Parser::isPreviousCharacterBackslash(std::list< Tokenized >::iterator it)
{
    auto position = it->second;
    if (position < 1)
        return false;

    --position;
    //there may be more tokens on position, search until token is before previous character
    while (it->second > position) {
        --it;
        if (it->first == Tokens::backslash)
            return true;
    }

    return false;
}*/
