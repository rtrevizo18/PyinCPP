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

void AST::deleteTabs(std::vector<std::string>& expression){
    expression.erase(expression.begin(), expression.begin() + tabLevel);
}

astNode* AST::assignExpr(std::vector<std::string>& expression){
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
            assignNode->body.push_back(intNode);
            return assignNode;
        }
        else if (AST_HELPER_isVariableToken(expression[2])){
            astNode* secondVarNode = new astNode;
            secondVarNode->map["type"] = "variable";
            secondVarNode->map["name"] = expression[2];
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

astNode* AST::printFunc(std::vector<std::string>& expression){
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
            strNode->map["value"] = expression[i].substr(1, expression[i].size() - 2);
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

astNode* AST::createifConditional(std::vector<std::string>& expression){
    //Guard Cases
    if (expression.size() < 5){
        throw;
    }
    if(expression[expression.size() - 1] != ":"){
        throw;
    }
    if(!(AST_HELPER_isVariableToken(expression[1]) || AST_HELPER_isDigit(expression[1]))){
        throw;
    }
    bool isEquality = ((expression[2] + expression[3]) == "==");
    if (!((expression[2] == "<") || (expression[2] == ">") || isEquality)){
        throw;
    }
    if(isEquality){
        if (!(AST_HELPER_isVariableToken(expression[4]) || AST_HELPER_isDigit(expression[4]))){
            throw;
        }
    }
    else if (!(AST_HELPER_isVariableToken(expression[3]) || AST_HELPER_isDigit(expression[3]))){
        throw;
    }

    astNode* ifNode = new astNode;
    astNode* logicOp = new astNode;
    astNode* operand1 = new astNode;
    astNode* operand2 = new astNode;


    ifNode->map["type"] = "if";
    logicOp->map["type"] = "logicOp";

    if (isEquality){
        logicOp->map["value"] = "==";
    }
    else{
        logicOp->map["value"] = expression[2];
    }

    if (AST_HELPER_isVariableToken(expression[1])){
        operand1->map["type"] = "variable";
        operand1->map["name"] = expression[1];
    }
    else{
        operand1->map["type"] = "intLiteral";
        operand1->map["value"] = expression[1];
    }

    if (isEquality){
        if (AST_HELPER_isVariableToken(expression[4])){
            operand2->map["type"] = "variable";
            operand2->map["name"] = expression[4];
        }
        else{
            operand2->map["type"] = "intLiteral";
            operand2->map["value"] = expression[4];
        }
    }
    else{
        if (AST_HELPER_isVariableToken(expression[3])){
            operand2->map["type"] = "variable";
            operand2->map["name"] = expression[3];
        }
        else{
            operand2->map["type"] = "intLiteral";
            operand2->map["value"] = expression[3];
        }
    }

    logicOp->body.push_back(operand1);
    logicOp->body.push_back(operand2);
    ifNode->body.push_back(logicOp);

    return ifNode;
}

astNode* AST::conditionBranch(std::vector<std::vector<std::string> >& tokenVec, int& i, bool& except){
    //Increments tab level of user
    tabLevel++;

    astNode* conditionNode = new astNode;
    conditionNode->map["type"] = "branch";
    astNode* ifNode = createifConditional(tokenVec[i]);
    i++;
    while(tokenVec[i][tabLevel - 1] == "tab"){
        deleteTabs(tokenVec[i]);
        astNode* blockNode = blockStatements(tokenVec, i, except);
        ifNode->body.push_back(blockNode);
        if (blockNode->map["type"] == "assign" || blockNode->map["type"] == "print"){
            continue;
        }
    }
    tabLevel--;

    conditionNode->body.push_back(ifNode);

    if(tokenVec[i][0] == "else"){
        if(tokenVec[i][1] != ":"){
            delete conditionNode;
            delete ifNode;
            throw;
        }
        astNode* elseNode = new astNode;
        elseNode->map["type"] = "else";
        tabLevel++; i++;
        while(tokenVec[i][tabLevel - 1] == "tab"){
            deleteTabs(tokenVec[i]);
            astNode* blockNode = blockStatements(tokenVec, i, except);
            elseNode->body.push_back(blockNode);
            if (blockNode->map["type"] == "assign" || blockNode->map["type"] == "print"){
                continue;
            }
        }
        tabLevel--;
        conditionNode->body.push_back(elseNode);
    }

    return conditionNode;
}
//Checking for statements that can exist in any code block
astNode* AST::blockStatements(std::vector<std::vector<std::string> >& tokenVec, int& i, bool& except){
    if (tokenVec[i].size() > 1 && tokenVec[i][1] == "="){
        try{
            return this->assignExpr(tokenVec[i++]);
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
            return this->conditionBranch(tokenVec, i, except);
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
            return this->printFunc(tokenVec[i++]);
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

void AST::parseFile(std::string pyFile){

    std::vector<std::vector<std::string> > tokenVector;
    Tokenizer tokenizer;

    tokenVector = tokenizer.tokenize(pyFile);
    
    this->AST = new astNode;

    this->AST->map["type"] = "program";
    
    bool foundException = false;
    int idx = 0;
    while(idx < tokenVector.size()){

        if (foundException){
            break;
        }
        /*
        Checks over statements that can appear on any line, such as if, else,
        assignment, print function
        */
        astNode* checkBlock = blockStatements(tokenVector, idx, foundException);

        if(checkBlock){
            this->AST->body.push_back(checkBlock);
            continue;
        }

        std::vector<std::string> expression = tokenVector[idx];

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

int AST::evalTree(astNode* node){
    // Check if the node is an integer literal
    if (node->map["type"] == "intLiteral") {
        return std::stoi(node->map["value"]);
    }

    if (node->map["type"] == "variable") {
        return symbolTable[node->map["name"]];
    }

    // If it's an operator node, evaluate its children recursively
    if (node->map["type"] == "operator") {
        std::string op = node->map["value"];
        int result = evalTree(node->body[0]);
        for (int i = 1; i < node->body.size(); ++i) {
            astNode* child = node->body[i];
            if (op == "+") {
                result += evalTree(child);
            } else if (op == "-") {
                result -= evalTree(child);
            } else if (op == "*") {
                result *= evalTree(child);
            } else if (op == "/") {
                result /= evalTree(child);
            }
        }
        return result;
    }

    return -1;
}

bool AST::evalLogic(astNode* node){
    astNode* op = node;
    astNode* LHS = op->body[0];
    astNode* RHS = op->body[1];

    int valueLHS;
    int valueRHS;

    if(LHS->map["type"] == "variable"){
        valueLHS = symbolTable[LHS->map["name"]];
    }
    else if(LHS->map["type"] == "intLiteral"){
        valueLHS = std::stoi(LHS->map["value"]);
    }

    if(RHS->map["type"] == "variable"){
        valueRHS = symbolTable[RHS->map["name"]];
    }
    else if(RHS->map["type"] == "intLiteral"){
        valueRHS = std::stoi(RHS->map["value"]);
    }

    if(op->map["value"] == "<")
        return valueLHS < valueRHS;
    else if(op->map["value"] == ">")
        return valueLHS > valueRHS;
    else if(op->map["value"] == "==")
        return valueLHS == valueRHS;

    return false;
}

void AST::runRecursively(astNode* node, int i = 0){
    if(node == nullptr){
        return;
    }

    for(i = 0; i < node->body.size(); i++){
        astNode* child = node->body[i];
        if (child->map["type"] == "assign"){
            std::string var = child->body[0]->map["name"];
            astNode* RHS = child->body[1];
            std::string typeRHS = RHS->map["type"];
            if(typeRHS == "intLiteral"){
                this->symbolTable[var] = std::stoi(RHS->map["value"]);
            }
            else if (typeRHS == "variable"){
                this->symbolTable[var] = symbolTable[RHS->map["name"]];
            }
            else if (typeRHS == "expression"){
                this->symbolTable[var] = evalTree(RHS->body[0]);
            }
        }
        else if (child->map["type"] == "print"){
            for (int j = 0; j < child->body.size(); j++){
                astNode* printNode = child->body[j];
                std::string typePrint = printNode->map["type"];
                if(typePrint == "strLiteral" || typePrint == "intLiteral"){
                    std::cout << printNode->map["value"];
                }
                else if(typePrint == "variable"){
                    std::cout << symbolTable[printNode->map["name"]];
                }
                if(j == child->body.size() - 1){
                    if(!(i == node->body.size() - 1)){
                        std::cout << "\n";
                    }
                }
                else{
                    std::cout << " ";
                }
            }
        }
        else if (child->map["type"] == "branch"){
            astNode* ifNode = child->body[0];
            bool isTrue = evalLogic(ifNode->body[0]);
            if(isTrue){
                runRecursively(ifNode, 1);
            }
            else{
                if(child->body.size() == 2){
                    astNode* elseNode = child->body[1];
                    runRecursively(elseNode);
                }
            }
        }
    }
}

void AST::runFile(){
    runRecursively(this->AST, 0);
}

