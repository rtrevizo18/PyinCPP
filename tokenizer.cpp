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
    const std::vector<char> opsyms = {'>', '<', '=', '*', '+', '-', '/', '(', ')', ':', '\t', ','};
    const std::vector<std::string> keywords = {"return", "def", "if", "else"};

    //Cut out any comments
    size_t foundComment = inputString.find('#');
    if (foundComment != std::string::npos){
        inputString = inputString.substr(0, foundComment);
    }

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

        //Check for special case print function
        if(this->checkSub("print(", inputString) != ""){
            //push "print" and "(" tokens and cut from string
            tokenVector.push_back(inputString.substr(0, 5));
            tokenVector.push_back(inputString.substr(5, 1));
            inputString = inputString.substr(6);


            while (!inputString.empty()){
                if (inputString[0] == ','){
                    tokenVector.push_back(",");
                    inputString = inputString.substr(1);
                }
                else if (inputString[0] == '\"'){
                    inputString = inputString.substr(1);
                    size_t findQuote = inputString.find_first_of('\"');
                    if(findQuote == std::string::npos){
                        std::cout << "UNCLOSED STRING\n";
                        break;
                    }
                    std::string strLiteral = "\"" + inputString.substr(0, findQuote + 1);

                    tokenVector.push_back(strLiteral);

                    inputString = inputString.substr(findQuote + 1);
                }
                else if (std::isalpha(inputString[0]) || inputString[0] == '_') {
                    std::string variableOrFunction;
                    int i = 0;
                    while (i < inputString.size() && (std::isalnum(inputString[i]) || inputString[i] == '_')) {
                        variableOrFunction += inputString[i];
                        i++;
                    }
                    tokenVector.push_back(variableOrFunction);
                    inputString = inputString.substr(i);
                }
                else if (std::isdigit(inputString[0])){
                    std::string integer;
                    int i = 0;
                    while (i < inputString.size() && std::isdigit(inputString[i])) {
                        integer += inputString[i];
                        i++;
                    }
                    tokenVector.push_back(integer);
                    inputString = inputString.substr(i);
                }
                else if (inputString[0] == ')'){
                    tokenVector.push_back(")");
                    inputString = inputString.substr(1);
                }
                else if (inputString[0] == '#'){
                    tokenVector.push_back("#");
                    inputString = inputString.substr(1);
                }
                else if (inputString[0] == ' '){
                    inputString = inputString.substr(1);
                }

                //Might need more testcases ???
            }

            continue;
        }

        //Then check for symbols
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

    //Cut out empty lines

    auto iterator = expressionVector.begin();

    while(iterator != expressionVector.end()){
        if(iterator->empty()){
            iterator = expressionVector.erase(iterator);
        }
        else{
            ++iterator;
        }
    }
    
    return expressionVector;
}