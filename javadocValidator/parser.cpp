#include "parser.h"

Parser::Parser(std::string &code, std::string fileName)
    : code(code), fileName(fileName)
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
    if (!iterateTroughtDocumentedFunctions())
        return false;

    return true;
}

void Parser::checkForBadKeyword(std::list< Tokenized >::iterator it)
{
    auto tmpIt = it; //found double quote begin

    --tmpIt;
    if (isKeyword(tmpIt) && tmpIt->second == it->second) {
        nonterminalsList.erase(it);
        return;
    }
    ++tmpIt;
    ++tmpIt;
    if (isKeyword(tmpIt) && tmpIt->second == it->second) {
        nonterminalsList.erase(it);
        return;
    }

    std::cout << "Warning: unrecognized keyword: " << getNextWord(it)
              << std::endl;
    return;
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
                if (it->first == Tokens::at)
                    checkForBadKeyword(it);
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
                    if (it == nonterminalsList.end()) {
                        std::cout << "Warning: code has unfinished c comment"
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

            while (it->first != Tokens::doubleQuotes) {
                if (it == nonterminalsList.end()) {
                    std::cout << "Warning: code has unfinished double quotes"
                              << std::endl;
                    break;
                }
                ++it;
            }
            nonterminalsList.erase(beginIt, it);
            break;
        }
        case Tokens::singleQuotes: {
            beginIt = it; //found single quote begin

            //single quotes can take 2 or 1 char, in case of two, it is escaped
            //escaped characters are deleted = 0 characters between ''
            ++it;
            if (it == nonterminalsList.end()) {
                std::cout << "Error: unfinished single quotes" << std::endl;
                return false;
            }
            if (it->first == Tokens::singleQuotes) {
                nonterminalsList.erase(beginIt, it);
                break;
            }
            //one character between ' '
            ++it;
            if (it == nonterminalsList.end()) {
                std::cout << "Error: unfinished single quotes" << std::endl;
                return false;
            }
            if (it->first == Tokens::singleQuotes){
                nonterminalsList.erase(beginIt, it);
                break;
            }

            std::cout << "Warning: missing end of single quote"
                      << std::endl;
            break;
        }
        default:
            break;
        }
    return true;
}

bool Parser::filterRepeatingSpaceOrTabs(std::list< Tokenized >::iterator it,
                                        std::list< Tokenized >::iterator tmpIt)
{
    //have to delete next whitespace, to correctly hold information about ends of words
    auto position = it->second;
    ++it;

    if (it == nonterminalsList.end())
        return false;

    tmpIt = it;

    while (it != nonterminalsList.end()
           && position == (it->second - 1)
           && isSpaceOrTab(it)) {
        ++position;
        ++it;
    }

    if (tmpIt != it) {
        it = nonterminalsList.erase(tmpIt, it);

        if (it == nonterminalsList.end())
            return false;
    }

    return true;
}

bool Parser::filterRepeatingNewlines(std::list< Tokenized >::iterator tmpIt,
                                     std::list< Tokenized >::iterator it)
{
    //have to delete next whitespace, to correctly hold information about ends of words
    auto position = it->second;
    ++it;

    if (it == nonterminalsList.end())
        return false;

    tmpIt = it;

    while (it != nonterminalsList.end()
           && position == (it->second - 1)
           && it->first == Tokens::newLine) {
        ++position;
        ++it;
    }

    if (tmpIt != it) {
        it = nonterminalsList.erase(tmpIt, it);

        if (it == nonterminalsList.end())
            return false;
    }

    return true;
}

void Parser::filterRepeatingWhitespace()
{
    auto tmpIt = nonterminalsList.begin();
    for (auto it = nonterminalsList.begin(); it != nonterminalsList.end(); ++it) {
        if (isSpaceOrTab(it)) {
            if (!filterRepeatingSpaceOrTabs(it, tmpIt))
                break;
        }
        //different behavior for newlines is beacuse in doxygen, we distinguish between space and newlines
        if (it->first == Tokens::newLine) {
            if (!filterRepeatingNewlines(tmpIt, it))
                break;
        }
    }
}



bool Parser::parseHeader(std::list< Tokenized >::iterator it)
{
    auto beginIt = it;

    if (it->first != Tokens::commentBegin) {
        std::cout << "Warning: Expected token commentBegin, got: "
                  << code.substr(0, getEnd(it))
                  << std::endl
                  << "File should start with comment with @author value"
                  << std::endl;
        //move to first comment
        while (it->first != Tokens::commentBegin) {
            ++it;
            if (it == nonterminalsList.end()) {
                std::cout << "Error: file with no doxygen" << std::endl;
                return false;
            }
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

            ++it; //step on next token (from @file)
            auto file = getNextWord(it);
            if (file.empty()) {
                std::cout << "Error: file position is empty" << std::endl;
                return false;
            }

            if (file != fileName) {
                std::cout << "Error: file name doesnt match, expcted: "
                          << fileName
                          << ", got: "
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

    ++it; //step on comment end
    nonterminalsList.erase(beginIt, it);

    if (!hasAuthor) {
        std::cout << "Error: missing @author command in header" << std::endl;
        return false;
    }
    if (!hasFile && !hasVersion) {
        std::cout << "Error: missing both javadoc style @version and doxygen @file command in header"
                  << std::endl;
        return false;
    }
    if (!hasFile)
        std::cout << "Warning: missing @file command in header" << std::endl;

    return true;
}

bool Parser::handleDoxygenComment(std::list< Tokenized >::iterator& it,
                                  std::set< std::string > &params)
{
    bool hasBrief = false;
    bool hasReturn = false;

    while (it != nonterminalsList.end()
           && it->first != Tokens::commentEnd) {
        switch (it->first) {
        case Tokens::atBrief: {
            if (hasBrief) {
                std::cout << "Error: multiple @brief in comment" << std::endl;
                return false;
            }

            ++it;
            if (it == nonterminalsList.end()) {
                std::cout << "Error: unfinished doxygen" << std::endl;
                return false;
            }

            std::string brief = getNextWord(it);
            if (brief.empty())
                std::cout << "Warning: @brief is empty" << std::endl;

            hasBrief = true;
            break;
        }
        case Tokens::atParam: {
            ++it;
            if (it == nonterminalsList.end()) {
                std::cout << "Error: unfinished doxygen" << std::endl;
                return false;
            }

            std::string tmpParam = getNextWord(it);
            if (tmpParam.empty())
                std::cout << "Warning: @param is empty" << std::endl;

            auto ret = params.insert(tmpParam);
            if (!ret.second) {
                std::cout << "Warning: multiple declaration of @param: "
                          << (*ret.first) << std::endl;
            }

            break;
        }
        case Tokens::atReturn: {
            if (hasReturn) {
                std::cout << "Error: multiple @return in comment" << std::endl;
                return false;
            }

            ++it;
            if (it == nonterminalsList.end()) {
                std::cout << "Error: unfinished doxygen" << std::endl;
                return false;
            }

            std::string returnVal = getNextWord(it);
            if (returnVal.empty())
                std::cout << "Warning: @return is empty" << std::endl;

            hasReturn = true;
            break;
        }
        default:
            break;
        }

        ++it;
    }

    ++it; //step over comment end

    if (!hasBrief) {
        std::cout << "Error: no @brief in comment" << std::endl;
        return false;
    }
    return true;
}

bool Parser::handleFunction(std::list< Tokenized >::iterator& it,
                            std::set< std::string > &params)
{
    //find opening left parenthesis
    while (it != nonterminalsList.end()
           && it->first != Tokens::lPar) {
        if (it->first == Tokens::commentBegin) {
            std::cout << "Warning: doxygen comment is without function."
                      << "This is possible for header comment only." << std::endl;
            return true;
        }
        ++it;
    }

    //--it; returnType = getNextWord(it); // get return value
    /*this enables parsing, if return value is void (better said _Noreturn),
     * so @return doesnt have to be specified
     * it is not supported
     */

    std::string name = getPreviousWord(it);

    if (name.empty()) {
        std::cout << "Error: could not handle function name, maybe wrong placed parenthesis"
                  << std::endl;
        return false;
    }

    while (it != nonterminalsList.end()
           && it->first != Tokens::lPar)
        ++it;

    int openParenthesisCounter = 0;
    int angleBracketsCounter = 0;
    bool parenthesisEnded = false;

    while (it != nonterminalsList.end()
           && !parenthesisEnded) {
        switch (it->first) {
        case Tokens::lPar: {
            ++openParenthesisCounter;
            break;
        }
        case Tokens::rPar: {
            --openParenthesisCounter;
            if (openParenthesisCounter == 0)
                parenthesisEnded = true;

            //break; //WARNING: commented to behave as comma
        }
        case Tokens::comma: {
            if (angleBracketsCounter > 0)
                break;
            std::string tmp = getAttribute(it);
            if (tmp.empty()) {
                std::cout << "Warning: function arg name is empty" << std::endl;
                break;
            }

            auto ret = params.insert(tmp);

            if (!ret.second) {
                std::cout << "Error: multiple params with name: "
                          << (*ret.first) << std::endl;
                return false;
            }
            break;
        }
        case Tokens::lAngleBracket: {
            ++angleBracketsCounter;
            break;
        }
        case Tokens::rAngleBracket: {
            --angleBracketsCounter;
            if (openParenthesisCounter < 0)
                std::cout << "Warning: more > than < in function" << std::endl;

            break;
        }
        default:
            break;
        }

        ++it;
    }

    return true;
}

void Parser::printArguments(std::set< std::string > dox, std::set< std::string > fun)
{
    for (auto &s : dox) {
        std::cout << "dox arg: " << s <<std::endl;
    }
    for (auto &s : fun) {
        std::cout << "fun arg: " << s <<std::endl;
    }
}

bool Parser::iterateTroughtDocumentedFunctions()
{
    for (auto it = nonterminalsList.begin(); it != nonterminalsList.end(); ++it) {
        if (it->first != Tokens::commentBegin) {
            //nonterminalsList.erase(it);
        }
        else {
            std::set< std::string > doxygenParms;
            std::set< std::string > functionParams;

            if (!handleDoxygenComment(it, doxygenParms))
                return false;

            if (!handleFunction(it, functionParams))
                return false;

            if (doxygenParms != functionParams) {
                std::cout << "Arguments are different to @params"
                          << std::endl;

                printArguments(doxygenParms, functionParams);
                return false;
            }
        }
    }
    return true;
}

bool Parser::isSpaceOrTab(std::list< Tokenized >::iterator it) {
    return (it->first == Tokens::space || it->first == Tokens::tab);
}

std::string::size_type Parser::getEnd(std::list< Tokenized >::iterator it)
{
    ++it;
    if (it == nonterminalsList.end())
        return std::string::npos;

    return it->second;
}

std::string Parser::getTextLine(std::list< Tokenized >::iterator &it)
{
    auto begin = it;
    while (it->first != Tokens::newLine && it != nonterminalsList.end())
        ++it;

    return code.substr(begin->second, it->second);
}

std::string Parser::getNextWord(std::list< Tokenized >::iterator &it)
{
    std::string output = code.substr(it->second, getEnd(it) - it->second);
    auto pos = output.find_first_not_of(" \t\n");
    if (pos == std::string::npos)
        return "";
    else
        return output.substr(pos, output.size());
}

std::string Parser::getPreviousWord(std::list< Tokenized >::iterator it)
{
    --it; //get before comma or rPar

    while ((it->first == Tokens::space
           || it->first == Tokens::tab
           || it->first == Tokens::newLine)
           && (it->second == getEnd(it) - 1)) //skip whitespace between function name and lPar
        --it; //get back before whitespace

    return getNextWord(it);
}

std::string Parser::getAttribute(std::list< Tokenized >::iterator it)
{
    std::string att = getPreviousWord(it);

    auto beg = att.find_first_not_of("*&(");
    auto end = att.find_first_of("[");
    if (beg == std::string::npos)
        return "";
    if (end == std::string::npos)
        return att.substr(beg, end);
    else
        return att.substr(beg, end-1); //cut the [
}


bool Parser::isDoxygenComment(std::list< Tokenized >::iterator it)
{
    auto tmpIt = it;
    ++tmpIt;
    if (tmpIt == nonterminalsList.end())
        return false;

    if (tmpIt->first == Tokens::commentBegin)
        return true;

    --it;
    if (it == nonterminalsList.begin())
        return false;

    if (it->first == Tokens::commentBegin)
        return true;

    return false;
}

bool Parser::isKeyword(std::list< Tokenized >::iterator it) {
    return (it->first == Tokens::atAuthor
            || it->first == Tokens::atBrief
            || it->first == Tokens::atFile
            || it->first == Tokens::atParam
            || it->first == Tokens::atReturn
            || it->first == Tokens::atVersion
            || it->first == Tokens::atSee
            || it->first == Tokens::atLink
            || it->first == Tokens::atSince);
}
