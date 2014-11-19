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
            if (it->first == Tokens::singleQuotes) {
                nonterminalsList.erase(beginIt, it);
                break;
            }
            //one character between ' '
            ++it;
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

void Parser::filterRepeatingWhitespace()
{
    auto tmpIt = nonterminalsList.begin();
    for (auto it = nonterminalsList.begin(); it != nonterminalsList.end(); ++it) {
        if (isSpaceOrTab(it)) {
            //have to delete next whitespace, to correctly hold information about ends of words
            auto position = it->second;
            ++it;
            tmpIt = it;

            while (it != nonterminalsList.end()
                   && position == (it->second - 1)
                   && isSpaceOrTab(it)) {
                ++position;
                ++it;
            }

            if (tmpIt != it)
                nonterminalsList.erase(tmpIt, it);

            if (it == nonterminalsList.end())
                break;
        }
        //different behavior for newlines is beacuse is doxygen, we distinguish between space and newlines
        if (it->first == Tokens::newLine) {
            //have to delete next whitespace, to correctly hold information about ends of words
            auto position = it->second;
            ++it;
            tmpIt = it;

            while (it != nonterminalsList.end()
                   && position == (it->second - 1)
                   && it->first == Tokens::newLine) {
                ++position;
                ++it;
            }

            if (tmpIt != it)
                nonterminalsList.erase(tmpIt, it);

            if (it == nonterminalsList.end())
                break;
        }
    }
}

bool Parser::handleDoxygenComment(std::list< Tokenized >::iterator& it,
                                  std::tuple< std::string, std::set< std::string >,
                                              std::string > &out)
{
    std::string brief;
    std::set< std::string > params;
    std::string returnVal;

    while (it != nonterminalsList.end()
           && it->first != Tokens::commentEnd) {
        switch (it->first) {
        case Tokens::atBrief: {
            if (!brief.empty()) {
                std::cout << "Error: multiple @brief in comment" << std::endl;
                return false;
            }

            ++it;

            brief = getNextWord(it); //TODO: repair correct load of function name
            if (brief.empty())
                std::cout << "Warning: @brief is empty" << std::endl;

            break;
        }
        case Tokens::atParam: {
            ++it;

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
            if (!returnVal.empty()) {
                std::cout << "Error: multiple @return in comment" << std::endl;
                return false;
            }

            ++it;

            returnVal = getNextWord(it);
            if (returnVal.empty())
                std::cout << "Warning: @return is empty" << std::endl;

            break;
        }
        default:
            break;
        }

        ++it;
    }

    ++it; //step over comment end

    if (brief.empty()) {
        std::cout << "Error: no @brief in comment or empty function name" << std::endl;
        return false;
    }
    out = make_tuple(brief, params, returnVal);
    return true;
}

bool Parser::handleFunction(std::list< Tokenized >::iterator& it,
                            std::tuple< std::string, std::set< std::string >,
                                        std::string > &function)
{
    std::string name;
    std::set< std::string > params;
    std::string returnType;

    //find opening left parenthesis
    while (it != nonterminalsList.end()
           && it->first != Tokens::lPar) {
        if (it->first == Tokens::commentBegin) {
            std::cout << "Warning: doxygen comment has without function."
                      << "This is possible for header comment only." << std::endl;
            return true;
        }
        ++it;
    }

    //--it; returnType = getNextWord(it); // get return value

    name = getPreviousWord(it);

    if (name.empty()) {
        std::cout << "Error: could not handle function name, maybe wrong placed parenthesis"
                  << std::endl;
        return false;
    }

    while (it != nonterminalsList.end()
           && it->first != Tokens::lPar)
        ++it;

    int openParenthesisCounter = 0;
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
            std::string tmp = getPreviousWord(it);
            if (tmp.empty()) {
                std::cout << "Error: function arg name is empty" << std::endl;
                return false;
            }

            auto ret = params.insert(tmp);

            if (!ret.second) {
                std::cout << "Error: multiple params with name: "
                          << (*ret.first) << std::endl;
                return false;
            }
            break;
        }
        default:
            break;
        }

        ++it;
    }

    function = make_tuple(name, params, returnType);

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
    std::tuple< std::string, std::set< std::string >, std::string > doxygen;
    std::tuple< std::string, std::set< std::string >, std::string > function;
    for (auto it = nonterminalsList.begin(); it != nonterminalsList.end(); ++it) {
        if (it->first != Tokens::commentBegin) {
            //nonterminalsList.erase(it);
        }
        else {
            if (!handleDoxygenComment(it, doxygen))
                return false;

            if (!handleFunction(it, function))
                return false;

            if (std::get<0>(doxygen) != std::get<0>(function)) {
                std::cout << "Function name is different to comment"
                          << std::endl
                          << "@brief: " << std::get<0>(doxygen)
                          << std::endl
                          << "function name: " << std::get<0>(function)
                          << std::endl;
                return false;
            }

            std::set< std::string > dox = std::get<1>(doxygen);
            std::set< std::string > fun = std::get<1>(function);
            if (dox != fun) {
                std::cout << "Arguments are different to @params"
                          << std::endl;

                printArguments(dox, fun);
                //TODO compare
                return false;
            }
        }
    }
    return true;
}

bool Parser::isSpaceOrTab(std::list< Tokenized >::iterator it) {
    return (it->first == Tokens::space || it->first == Tokens::tab);
}

bool Parser::parseHeader(std::list< Tokenized >::iterator it)
{
    auto beginIt = it;

    if (it->first != Tokens::commentBegin) {
        std::cout << "Warning: Expected token commentBegin, got: "
                  << input.substr(0, getEnd(it))
                  << std::endl
                  << "File should start with comment with @author value"
                  << std::endl;
        //move to first comment
        while (it->first != Tokens::commentBegin) {
            ++it;
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



std::string::size_type Parser::getEnd(std::list< Tokenized >::iterator it)
{
    ++it;
    return it->second;
}

std::string Parser::getTextLine(std::list< Tokenized >::iterator &it)
{
    auto begin = it;
    while (it->first != Tokens::newLine && it != nonterminalsList.end())
        ++it;

    return input.substr(begin->second, it->second); //TODO: test if returns \0
}

std::string Parser::getNextWord(std::list< Tokenized >::iterator &it)
{
    std::string output = input.substr(it->second, getEnd(it) - it->second);
    auto pos = output.find_first_not_of(" \t\n");
    if (pos == std::string::npos)
        return "";
    else
        return output.substr(pos, output.size());
}

std::string Parser::getPreviousWord(std::list< Tokenized >::iterator it)
{
    --it;

    while (it->second == (getEnd(it) - 1)) //skip whitespace between function name and lPar
        --it; //get back before function name


    return getNextWord(it);
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

bool Parser::isKeyword(std::list< Tokenized >::iterator it) {
    return (it->first == Tokens::atAuthor
            || it->first == Tokens::atBrief
            || it->first == Tokens::atFile
            || it->first == Tokens::atParam
            || it->first == Tokens::atReturn
            || it->first == Tokens::atVersion);
}
