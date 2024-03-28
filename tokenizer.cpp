#include "tokenizer.h"

std::string Tokenizer::checkSub(std::string subString, std::string string){
    if (subString.empty() || string.empty()){
        return "";
    }
    if(string.length() < subString.length()){
        return "";
    }
    std::string stringSubbed = string.substr(0, subString.length());
    if (subString == stringSubbed){
        return stringSubbed;
    }
    return "";
}

std::vector<std::string> Tokenizer::tokenizeHelper(const std::string input){
    std::vector<std::string> tokenVector;
    std::string inputString = input;
    const std::vector<char> opsyms = {'>', '<', '=', '*', '+', '-', '/', '(', ')', ':', '#', '\"', '\t', ','};
    const std::vector<std::string> keywords = {"return", "def", "if", "else"};

    while (!inputString.empty()){
        /*Check for tabbing/spaces first*/
        if (this->checkSub("    ", inputString) != ""){
            tokenVector.push_back("tab");
            inputString = inputString.substr(4);
            continue;
        }

        /*Skip over whitespace*/
        if (this->checkSub(" ", inputString) != ""){
            inputString = inputString.substr(1);
            continue;
        }

        /*Then check for symbols*/
        if(std::find(opsyms.begin(), opsyms.end(), inputString[0]) != opsyms.end()){
            std::string s (1, inputString[0]);
            tokenVector.push_back(s);
            inputString = inputString.substr(1);
            continue;
        }

        bool foundKeyWord = false;
        for(int i = 0; i < keywords.size(); i++){
            std::string checkSubString = this->checkSub(keywords[i], inputString);
            if (!checkSubString.empty()){
                tokenVector.push_back(checkSubString);
                inputString = inputString.substr(checkSubString.size());
                foundKeyWord = true;
                break;
            }
        }
        if(foundKeyWord){
            continue;
        }

        // Check for variables and functions
        if (std::isalpha(inputString[0]) || inputString[0] == '_') {
            std::string variableOrFunction;
            int i = 0;
            while (i < inputString.size() && (std::isalnum(inputString[i]) || inputString[i] == '_')) {
                variableOrFunction += inputString[i];
                i++;
            }
            tokenVector.push_back(variableOrFunction);
            inputString = inputString.substr(i);
            continue;
        }
        //Check for numbers
        if (std::isdigit(inputString[0])){
            std::string number;
            int i = 0;
            while (i < inputString.size() && (std::isdigit(inputString[i]) || inputString[i] == '.')) {
                number += inputString[i];
                i++;
            }
            tokenVector.push_back(number);
            inputString = inputString.substr(i);
            continue;
        }

        // If no match found, just skip over character
        inputString = inputString.substr(1);
    }


    return tokenVector;
}


std::vector<std::vector<std::string> > Tokenizer::tokenize(const std::string input){
    std::vector<std::vector<std::string> > expressionVector;
    std::string line;
    std::stringstream inputStream(input);
    while(std::getline(inputStream, line)){
        expressionVector.push_back(this->tokenizeHelper(line));
    }

    for(int i = 0; i < expressionVector.size(); i++){
        for(int j = 0; j < expressionVector[i].size(); j++){
            std::cout << expressionVector[i][j] << " ";
        }
        std::cout << "\n";
    }
    return expressionVector;
}

