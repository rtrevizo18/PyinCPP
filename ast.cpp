#include "ast.h"


astNode* AST::assignExpr(const std::vector<std::string>& expression){
    //FIXME
}

astNode* AST::printFunc(const std::vector<std::string>& expression){

    if (!(expression[1] == "(")){
        throw;
    }

    astNode* printNode = new astNode;
    printNode->map["type"] = "print";

    for (int i = 2; i < expression.size(); i++){
        
        if(expression[i][0] == '\"'){
            if(expression[i][expression.size() - 1] != '\"'){
                throw;
            }
            astNode* strNode = new astNode;

            strNode->map["type"] = "strLiteral";
            strNode->map["value"] = expression[i];
            printNode->body.push_back(strNode);
        }
        else if (std::isalpha(expression[i][0]) || expression[i][0] == '_') {
            astNode* variableNode = new astNode;

            variableNode->map["type"] = "variable";
            variableNode->map["name"] = expression[i];
            printNode->body.push_back(variableNode);
        }
        else if (expression[i] == ")"){
            break;
        }
    }

    return printNode;
}

void AST::parseFile(std::string pyFile){

    std::vector<std::vector<std::string> > tokenVector;
    Tokenizer tokenizer;

    tokenVector = tokenizer.tokenize(pyFile);

    astNode* programNode = new astNode;

    AST->map["type"] = "Program";
    
    bool foundException = false;
    for(int i = 0; i < tokenVector.size(); i++){

        if (foundException){
            break;
        }

        std::vector<std::string> expression = tokenVector[i];

        if (expression[0] == "def"){
            astNode* node = new astNode;

            node->map["type"] = "funcDef";


            //FIXME: Add support for function declarations,
            //including storing number of args, expressions, and return statement
            continue;
        }

        if (expression[0] == "print"){
            astNode* print = nullptr;

            try{
                print = this->printFunc(expression);
                programNode->body.push_back(print);
            }
            catch(std::string e){
                std::string stringStr (expression.begin(), expression.end());
                std::cout << "Invalid print function:\n" << stringStr << "\n";
                foundException = true;
            }

            continue;
        }

        if (expression[0] == "if"){
            astNode* node = new astNode;

            node->map["type"] = "branch";

            //FIXME: Add support for branch node, which has one or two child nodes
            //depending on whether an "if" has a corresponding "else"
            continue;
        }

        if (expression [1] == "="){
            astNode* assign = nullptr;

            try{
                assign = assignExpr(expression);
                programNode->body.push_back(assign);
            }
            catch(std::string e){
                std::string stringStr (expression.begin(), expression.end());
                std::cout << "Invalid assignment statement:\n" << stringStr << "\n";
                foundException = true;
            }

            //FIXME: Add support for assignment node, which has a variable node and
            //an expression node, which has its own function that turns expression
            //into a tree so it can be evaluated
            continue;
        }

        for(int j = 0; j < expression.size(); j++){
            //Skip past rest of expression if comment
            if (expression[j] == "#"){
                break;
            }

            if(expression[j] == ""){

            }
        }

    }
}