#include "ast.h"

void AST::parseFile(std::string pyFile){

    std::vector<std::vector<std::string> > tokenVector;
    Tokenizer tokenizer;

    tokenVector = tokenizer.tokenize(pyFile);

    for(int i = 0; i < tokenVector.size(); i++){
        std::vector<std::string> expression = tokenVector[i];
    }

    if (expression[0] == "def"){

    }
}