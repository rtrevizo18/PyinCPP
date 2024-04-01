#include "ast.h"

bool AST_HELPER_isIn(const std::vector<std::string> expression, std::string str){
    return std::find(expression.begin(), expression.end(), str) != expression.end();
}

bool AST_HELPER_isVariableToken(std::string possibleVar){
    for(int i = 0; i < possibleVar.size(); i++){
        if(!(std::isalpha(possibleVar[i]) || possibleVar[i] == '_')){
            return false;
        }
    }
    return true;
}

bool AST_HELPER_isDigit(const std::string& str) {
    for (char ch : str) {
        if (!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

bool AST_HELPER_isOperator(const std::string& token){
    return token == "+" || token == "-" || token == "*" || token == "/";
}

bool AST_HELPER_compare(std::string op1, std::string op2) {
    if (op1 == "(" || op1 == ")")
      return false;
    return op1 == "*" || op1 == "/" || op2 == "+" || op2 == "-";
}

astNode* AST_HELPER_inToTree(const std::vector<std::string>& infix) {
    std::stack<astNode*> nodeStack;
    std::stack<std::string> symbolStack;
    std::map<std::string, int> priority;

    for (std::string s : infix) {
        if(s == "("){
            symbolStack.push("(");
        }
        else if (AST_HELPER_isDigit(s)){
            astNode* numNode = new astNode;
            numNode->map["type"] = "intLiteral";
            numNode->map["value"] = s;
            nodeStack.push(numNode);
        }
        else if (AST_HELPER_isVariableToken(s)){
            astNode* varNode = new astNode;
            varNode->map["type"] = "variable";
            varNode->map["name"] = s;
            nodeStack.push(varNode);
        }
        else if (s == ")"){
            while(symbolStack.top() != "("){
                astNode* opNode = new astNode;
                opNode->map["type"] = "operator";
                opNode->map["value"] = symbolStack.top(); symbolStack.pop();

                astNode* rightNode = nodeStack.top(); nodeStack.pop();
                astNode* leftNode = nodeStack.top(); nodeStack.pop();

                opNode->body.push_back(leftNode);
                opNode->body.push_back(rightNode);

                nodeStack.push(opNode);
            }
            symbolStack.pop();
        }
        else if (AST_HELPER_isOperator(s)){
            while(!symbolStack.empty() && AST_HELPER_compare(symbolStack.top(), s)){
                astNode* opNode = new astNode;
                opNode->map["type"] = "operator";
                opNode->map["value"] = symbolStack.top(); symbolStack.pop();

                astNode* rightNode = nodeStack.top(); nodeStack.pop();
                astNode* leftNode = nodeStack.top(); nodeStack.pop();

                opNode->body.push_back(leftNode);
                opNode->body.push_back(rightNode);

                nodeStack.push(opNode);
            }
            symbolStack.push(s);
        }
    }

    while(!symbolStack.empty()){
        astNode* opNode = new astNode;
        opNode->map["type"] = "operator";
        opNode->map["value"] = symbolStack.top(); symbolStack.pop();

        astNode* rightNode = nodeStack.top(); nodeStack.pop();
        astNode* leftNode = nodeStack.top(); nodeStack.pop();

        opNode->body.push_back(leftNode);
        opNode->body.push_back(rightNode);

        nodeStack.push(opNode);
    }

    return nodeStack.top();
}

astNode* AST::assignExpr(const std::vector<std::string>& expression){
    if(!AST_HELPER_isVariableToken(expression[0])){
        throw;
    }
    if (expression.size() < 3){
        throw;
    }

    astNode* assignNode = new astNode;
    assignNode->map["type"] = "assign";
    
    astNode* varNode = new astNode;
    varNode->map["type"] = "variable";
    varNode->map["name"] = expression[0];
    assignNode->body.push_back(varNode);

    if (expression.size() == 3){
        if(AST_HELPER_isDigit(expression[2])){
            astNode* intNode = new astNode;
            intNode->map["type"] = "intLiteral";
            intNode->map["value"] = expression[2];
            assignNode->body.push_back(varNode);
            assignNode->body.push_back(intNode);
            return assignNode;
        }
        else if (AST_HELPER_isVariableToken(expression[2])){
            astNode* secondVarNode = new astNode;
            secondVarNode->map["type"] = "variable";
            secondVarNode->map["name"] = expression[2];
            assignNode->body.push_back(varNode);
            assignNode->body.push_back(secondVarNode);
            return assignNode;
        }
    }

    std::vector<std::string> arthiExpr = std::vector<std::string>(expression.begin() + 2, expression.end());

    astNode* treeExpression = new astNode;
    treeExpression->map["type"] = "expression";
    treeExpression->body.push_back(AST_HELPER_inToTree(arthiExpr));

    assignNode->body.push_back(treeExpression);

    return assignNode;
}

astNode* AST::printFunc(const std::vector<std::string>& expression){
    if (!(expression[1] == "(")){
        throw;
    }

    astNode* printNode = new astNode;
    printNode->map["type"] = "print";

    for (int i = 2; i < expression.size(); i++){
        
        if(expression[i][0] == '\"'){
            if(expression[i][expression[i].size() - 1] != '\"'){
                delete printNode;
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
        else if (expression[i] == ","){
            continue;
        }
    }

    return printNode;
}
//Checking for statements that can exist in any code block
astNode* AST::blockStatements(const std::vector<std::vector<std::string> >& tokenVec, int i, bool& e){
    if (tokenVec[i].size() > 1 && tokenVec[i][1] == "="){
        try{
            return this->assignExpr(tokenVec[i]);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[i]) {
                str += s;
            }
            std::cout << "Invalid assigment:\n" << str << "\n";
            e = true;
        }
    }
    else if (tokenVec[i][0] == "if"){
        try{
            return this->conditionBranch(tokenVec, i);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[i]) {
                str += s;
            }
            std::cout << "Invalid conditional:\n" << str << "\n";
            e = true;
        }
    }
    else if (tokenVec[i][0] == "print"){
        try{
            return this->printFunc(tokenVec[i]);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[i]) {
                str += s;
            }
            std::cout << "Invalid print function:\n" << str << "\n";
            e = true;
        }
    }
    return nullptr;
}

astNode* AST::conditionBranch(const std::vector<std::vector<std::string> >& tokenVec, int i){
    if (tokenVec[i][tokenVec.size() - 1] != ":"){
        throw;
    }
}

void AST::parseFile(std::string pyFile){

    std::vector<std::vector<std::string> > tokenVector;
    Tokenizer tokenizer;

    tokenVector = tokenizer.tokenize(pyFile);
    
    this->AST = new astNode;

    this->AST->map["type"] = "program";
    
    bool foundException = false;
    for(int i = 0; i < tokenVector.size(); i++){

        if (foundException){
            break;
        }
        /*
        Checks over statements that can appear on any line, such as if, else,
        assignment, print function
        */
        astNode* checkBlock = blockStatements(tokenVector, i, foundException);

        if(checkBlock){
            this->AST->body.push_back(checkBlock);
            continue;
        }

        std::vector<std::string> expression = tokenVector[i];

        if (expression[0] == "def"){
            astNode* node = new astNode;

            node->map["type"] = "funcDef";


            //FIXME: Add support for function declarations,
            //including storing number of args, expressions, and return statement
            continue;
        }
    }
}

void AST::printRecursive(astNode* node, int depth = 0){
    // Print indentation based on the depth of the node
    for (int i = 0; i < depth; ++i)
        std::cout << "  ";

    // Print the node's map
    std::cout << "Node Map:" << std::endl;
    for (const auto& pair : node->map) {
        for (int i = 0; i <= depth; ++i)
            std::cout << "  ";
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    // Print the children of the node
    for (astNode* child : node->body) {
        printRecursive(child, depth + 1); // Recursive call for each child
    }
}

void AST::print(){
    this->printRecursive(this->AST);
}

void AST::runFile(){

}

