#include "ast.h"

bool AST_HELPER_isIn(const std::vector<std::string> expression, std::string str){
    return std::find(expression.begin(), expression.end(), str) != expression.end();
}

bool AST_HELPER_isVariableToken(std::string possibleVar){
    int i = 0;

    if(!(std::isalpha(possibleVar[i]) || possibleVar[i] == '_')){
        return false;
    }

    for(i = 1; i < possibleVar.size(); i++){
        if(!(std::isalnum(possibleVar[i]) || possibleVar[i] == '_')){
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

astNode* AST_HELPER_parseFunctionArguments(const std::vector<std::string>& tokens, size_t& index) {
    // Recursive helper function to parse function arguments
    std::stack<astNode*> nodeStack;
    std::stack<std::string> symbolStack;

    for (; index < tokens.size(); ++index) {
        const std::string& s = tokens[index];
        if (s == ")") {
            while (!symbolStack.empty() && symbolStack.top() != "(") {
                astNode* opNode = new astNode;
                opNode->map["type"] = "operator";
                opNode->map["value"] = symbolStack.top(); symbolStack.pop();

                astNode* rightNode = nodeStack.top(); nodeStack.pop();
                astNode* leftNode = nodeStack.top(); nodeStack.pop();

                opNode->body.push_back(leftNode);
                opNode->body.push_back(rightNode);

                nodeStack.push(opNode);
            }
            if (!symbolStack.empty()) {
                symbolStack.pop(); // Remove "("
            }
            break; // End of function arguments
        } else if (AST_HELPER_isDigit(s)) {
            astNode* numNode = new astNode;
            numNode->map["type"] = "intLiteral";
            numNode->map["value"] = s;
            nodeStack.push(numNode);
        } else if (AST_HELPER_isVariableToken(s)) {
            if (index + 1 < tokens.size() - 1 && tokens[index + 1] == "(") {
                // Function call
                astNode* funcCallNode = new astNode;
                funcCallNode->map["type"] = "funcCall";
                funcCallNode->map["name"] = s;

                size_t argIndex = index + 2;
                astNode* arguments = AST_HELPER_parseFunctionArguments(tokens, argIndex);
                funcCallNode->body.push_back(arguments);
                nodeStack.push(funcCallNode);
                index = argIndex;
            } else {
                // Variable
                astNode* varNode = new astNode;
                varNode->map["type"] = "variable";
                varNode->map["name"] = s;
                nodeStack.push(varNode);
            }
        }
        else if (AST_HELPER_isOperator(s)) {
            // Process operators
            while (!symbolStack.empty() && AST_HELPER_compare(symbolStack.top(), s)) {
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

    // Combine parsed nodes into a single node representing the arguments
    astNode* arguments = new astNode;

    if (!nodeStack.empty()) {
        while (!nodeStack.empty()) {
            arguments->body.push_back(nodeStack.top());
            nodeStack.pop();
        }
    }

    return arguments;
}

astNode* AST_HELPER_inToTree(const std::vector<std::string>& infix) {
    std::stack<astNode*> nodeStack;
    std::stack<std::string> symbolStack;
    std::map<std::string, int> priority;

    for (size_t i = 0; i < infix.size(); ++i) {
        const std::string& s = infix[i];
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
            // Check if it's a function call
            if (i + 1 < infix.size() - 1 && infix[i + 1] == "(") {
                // Function call
                astNode* funcCallNode = new astNode;
                funcCallNode->map["type"] = "funcCall";
                funcCallNode->map["name"] = s;

                size_t argIndex = i + 2;
                astNode* arguments = AST_HELPER_parseFunctionArguments(infix, argIndex);
                arguments->map["type"] = "args";
                funcCallNode->body.push_back(arguments);
                nodeStack.push(funcCallNode);
                i = argIndex;
            } else {
                // Variable
                astNode* varNode = new astNode;
                varNode->map["type"] = "variable";
                varNode->map["name"] = s;
                nodeStack.push(varNode);
            }
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

    currentLine++;

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


    //Skip over to next line in tokenVector

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
        else if (AST_HELPER_isVariableToken(expression[i])) {
            astNode* variableNode = new astNode;

            variableNode->map["type"] = "variable";
            variableNode->map["name"] = expression[i];
            printNode->body.push_back(variableNode);
        }
        else if (AST_HELPER_isDigit(expression[i])){
            astNode* variableNode = new astNode;

            variableNode->map["type"] = "intLiteral";
            variableNode->map["value"] = expression[i];
            printNode->body.push_back(variableNode);
        }
        else if (expression[i] == ")"){
            break;
        }
        else if (expression[i] == ","){
            continue;
        }
    }

    currentLine++;

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
    bool islessEqual = ((expression[2] + expression[3]) == "<=");
    bool isgreaterEqual = ((expression[2] + expression[3]) == ">=");

    if (!((expression[2] == "<") || (expression[2] == ">") || isEquality || islessEqual || isgreaterEqual)){
        throw;
    }
    if(isEquality || islessEqual || isgreaterEqual){
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
    else if (islessEqual){
        logicOp->map["value"] = "<=";
    }
    else if (isgreaterEqual){
        logicOp->map["value"] = ">=";
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

    if (isEquality || islessEqual || isgreaterEqual){
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

    currentLine++;

    return ifNode;
}

astNode* AST::conditionBranch(std::vector<std::vector<std::string> >& tokenVec, bool& except){
    //Increments tab level of user
    tabLevel++;
    astNode* conditionNode = new astNode;
    conditionNode->map["type"] = "branch";
    astNode* ifNode = createifConditional(tokenVec[currentLine]);
    while(tokenVec[currentLine].size() >= tabLevel && tokenVec[currentLine][tabLevel - 1] == "tab"){
        deleteTabs(tokenVec[currentLine]);
        if((tokenVec.size() - 1) == currentLine){
            astNode* blockNode = blockStatements(tokenVec, except);
            if(blockNode){
                ifNode->body.push_back(blockNode);
            }
            conditionNode->body.push_back(ifNode);
            return conditionNode;
        }
        else{
            astNode* blockNode = blockStatements(tokenVec, except);
            if(blockNode){
                ifNode->body.push_back(blockNode);
            }
        }
    }
    tabLevel--;

    conditionNode->body.push_back(ifNode);

    if(tokenVec[currentLine][tabLevel] == "else"){
        deleteTabs(tokenVec[currentLine]);
        if(tokenVec[currentLine][1] != ":"){
            delete conditionNode;
            delete ifNode;
            throw;
        }
        astNode* elseNode = new astNode;
        elseNode->map["type"] = "else";
        tabLevel++; currentLine++;
        while(tokenVec[currentLine].size() >= tabLevel && tokenVec[currentLine][tabLevel - 1] == "tab"){
            deleteTabs(tokenVec[currentLine]);
            if((tokenVec.size() - 1) == currentLine){
                astNode* blockNode = blockStatements(tokenVec, except);
                if(blockNode){
                    elseNode->body.push_back(blockNode);
                }
                conditionNode->body.push_back(elseNode);
                return conditionNode;
            }
            else{
                astNode* blockNode = blockStatements(tokenVec, except);
                if(blockNode){
                    elseNode->body.push_back(blockNode);
                }
            }
        }
        tabLevel--;
        conditionNode->body.push_back(elseNode);
    }

    return conditionNode;
}

astNode* AST::returnExpr(std::vector<std::string>& expression){
    if (expression.size() < 2){
        throw;
    }

    currentLine++;

    astNode* returnNode = new astNode;
    returnNode->map["type"] = "return";

    if (expression.size() == 2){
        if(AST_HELPER_isDigit(expression[1])){
            astNode* intNode = new astNode;
            intNode->map["type"] = "intLiteral";
            intNode->map["value"] = expression[1];
            returnNode->body.push_back(intNode);
            return returnNode;
        }
        else if (AST_HELPER_isVariableToken(expression[1])){
            astNode* secondVarNode = new astNode;
            secondVarNode->map["type"] = "variable";
            secondVarNode->map["name"] = expression[1];
            returnNode->body.push_back(secondVarNode);
            return returnNode;
        }
    }

    std::vector<std::string> arthiExpr = std::vector<std::string>(expression.begin() + 1, expression.end());

    astNode* treeExpression = new astNode;
    treeExpression->map["type"] = "expression";
    treeExpression->body.push_back(AST_HELPER_inToTree(arthiExpr));

    returnNode->body.push_back(treeExpression);


    //Skip over to next line in tokenVector

    return returnNode;
}
//Checking for statements that can exist in any code block
astNode* AST::blockStatements(std::vector<std::vector<std::string> >& tokenVec, bool& except){
    if(tokenVec[currentLine].empty()){
        currentLine++;
        return nullptr;
    }

    if (tokenVec[currentLine].size() > 1 && tokenVec[currentLine][1] == "="){
        try{
            return this->assignExpr(tokenVec[currentLine]);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[currentLine]) {
                str += s;
            }
            std::cout << "Invalid assigment:\n" << str << "\n";
            except = true;
        }
    }
    else if (tokenVec[currentLine][0] == "if"){
        try{
            return this->conditionBranch(tokenVec, except);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[currentLine]) {
                str += s;
            }
            std::cout << "Invalid conditional:\n" << str << "\n";
            except = true;
        }
    }
    else if (tokenVec[currentLine][0] == "print"){
        try{
            return this->printFunc(tokenVec[currentLine]);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[currentLine]) {
                str += s;
            }
            std::cout << "Invalid print function:\n" << str << "\n";
            except = true;
        }
    }
    else if (tokenVec[currentLine][0] == "return"){
        try{
            return this->returnExpr(tokenVec[currentLine]);
        }
        catch(std::string e){
            std::string str;
            for (const std::string& s : tokenVec[currentLine]) {
                str += s;
            }
            std::cout << "Invalid print function:\n" << str << "\n";
            except = true;
        }
    }
    return nullptr;
}

astNode* AST::funcDef(std::vector<std::vector<std::string> >& tokenVec, bool& except){

    astNode* funcAST = new astNode;
    funcAST->map["type"] = "funcDef";
    funcAST->map["name"] = tokenVec[currentLine][1];

    size_t i = 3;
    astNode* paramsNode = new astNode;
    paramsNode->map["type"] = "params";
    const std::vector<std::string>& expression = tokenVec[currentLine];
    while(i < expression.size() && expression[i] != ")"){
        if (AST_HELPER_isVariableToken(expression[i])){
            astNode* parNode = new astNode;
            parNode->map["type"] = "parameter";
            parNode->map["name"] = expression[i];
            paramsNode->body.push_back(parNode);
        }
        ++i;
    }

    funcAST->body.push_back(paramsNode);


    currentLine++;
    tabLevel++;


    astNode* blockNodes = new astNode;
    blockNodes->map["type"] = "blockStatements";
    while(tokenVec[currentLine].size() >= tabLevel && tokenVec[currentLine][tabLevel - 1] == "tab"){
        deleteTabs(tokenVec[currentLine]);
        if((tokenVec.size() - 1) == currentLine){
            astNode* blockNode = blockStatements(tokenVec, except);
            if(blockNode){
                blockNodes->body.push_back(blockNode);
            }
            funcAST->body.push_back(blockNodes);
            return funcAST;
        }
        else{
            astNode* blockNode = blockStatements(tokenVec, except);
            if(blockNode){
                blockNodes->body.push_back(blockNode);
            }
        }
    }
    tabLevel--;

    funcAST->body.push_back(blockNodes);

    return funcAST;

}

void AST::parseFile(std::string pyFile){

    std::vector<std::vector<std::string> > tokenVector;
    Tokenizer tokenizer;

    tokenVector = tokenizer.tokenize(pyFile);
    
    this->tree = new astNode;

    this->tree->map["type"] = "program";
    
    bool foundException = false;
    while(currentLine < tokenVector.size()){
        /*
        Checks over statements that can appear on any line, such as if, else,
        assignment, print function
        */
        astNode* checkBlock = blockStatements(tokenVector, foundException);

        if(checkBlock){
            this->tree->body.push_back(checkBlock);
            continue;
        }
        else if (tokenVector[currentLine][0] == "def"){

            astNode* funcAST = funcDef(tokenVector, foundException);

            functionTable[funcAST->map["name"]] = funcAST;

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
    this->printRecursive(this->tree);
}

int AST::evalTree(astNode* node){
    // Check if the node is an integer literal
    if (node->map["type"] == "intLiteral") {
        return std::stoi(node->map["value"]);
    }

    if (node->map["type"] == "variable") {

        if(symbolTable.find(node->map["name"]) != symbolTable.end()){
            return symbolTable[node->map["name"]];
        }

        AST* currScope = parentScope;

        while(currScope != nullptr){
            if(currScope->symbolTable.find(node->map["name"]) != currScope->symbolTable.end()){
                return currScope->symbolTable[node->map["name"]];
            }
            currScope = currScope->parentScope;
        }

        std::cout << "VARIABLE NOT FOUND. ERROR\n";

        return -999;
    }

    if(node->map["type"] == "funcCall"){
        std::string funcName = node->map["name"];

        astNode* funcTree = functionTable[funcName];

        AST* funcAST = new AST;

        funcAST->parentScope = this;
        funcAST->tree = funcTree;

        astNode* params = funcAST->tree->body[0];
        astNode* args = node->body[0];
        astNode* funcStatements = funcAST->tree->body[1];

        astNode* paramNode = nullptr;
        astNode* argNode = nullptr;

        for(int i = 0; i < params->body.size(); i++){
            paramNode = params->body[i];
            argNode = args->body[i];

            funcAST->symbolTable[paramNode->map["name"]] = this->evalTree(argNode);
        }

        return funcAST->runRecursively(funcStatements, 0);
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
    else if(op->map["value"] == ">=")
        return valueLHS >= valueRHS;
    else if(op->map["value"] == "<=")
        return valueLHS <= valueRHS;

    return false;
}

int AST::runRecursively(astNode* node, int i = 0){
    if(node == nullptr){
        return -1;
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
        else if (child->map["type"] == "return"){
            return evalTree(child->body[0]);
        }
    }

    return 0;
}

void AST::runFile(){
    runRecursively(this->tree, 0);
}

