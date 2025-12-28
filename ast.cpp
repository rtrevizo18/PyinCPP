#include "ast.h"

bool AST_HELPER_isIn(const std::vector<std::string> expression,
                     std::string str) {
  return std::find(expression.begin(), expression.end(), str) !=
         expression.end();
}

bool AST_HELPER_isVariableToken(std::string possibleVar) {
  int i = 0;

  if (!(std::isalpha(possibleVar[i]) || possibleVar[i] == '_')) {
    return false;
  }

  for (i = 1; i < possibleVar.size(); i++) {
    if (!(std::isalnum(possibleVar[i]) || possibleVar[i] == '_')) {
      return false;
    }
  }
  return true;
}

// Checks if a string represents a valid number (integer or floating point)
bool AST_HELPER_isNumber(const std::string &str) {
  if (str.empty()) {
    return false;
  }

  bool hasDecimal = false;
  size_t start = 0;

  for (size_t i = start; i < str.size(); i++) {
    char ch = str[i];
    if (ch == '.') {
      if (hasDecimal) {
        return false; // Multiple decimal points
      }
      hasDecimal = true;
    } else if (!isdigit(static_cast<unsigned char>(ch))) {
      return false;
    }
  }

  return true;
}

// Legacy function name for compatibility - now handles both ints and floats
bool AST_HELPER_isDigit(const std::string &str) {
  return AST_HELPER_isNumber(str);
}

bool AST_HELPER_isOperator(const std::string &token) {
  return token == "+" || token == "-" || token == "*" || token == "/";
}

bool AST_HELPER_compare(std::string op1, std::string op2) {
  if (op1 == "(" || op1 == ")")
    return false;
  return op1 == "*" || op1 == "/" || op2 == "+" || op2 == "-";
}

size_t AST_HELPER_closeIndex(const std::vector<std::string> &tokens,
                             size_t openingIndex) {
  size_t nestedCount = 1;
  for (size_t i = openingIndex + 1; i < tokens.size(); ++i) {
    if (tokens[i] == "(") {
      ++nestedCount;
    } else if (tokens[i] == ")") {
      --nestedCount;
      if (nestedCount == 0) {
        return i;
      }
    }
  }
  return 0;
}

std::vector<std::string>
AST_HELPER_getExpr(const std::vector<std::string> &tokens, int open_index,
                   int close_index) {
  if (tokens[open_index] != "(" || tokens[close_index] != ")") {
    return {};
  }

  std::vector<std::string> expression(tokens.begin() + open_index + 1,
                                      tokens.begin() + close_index);
  return expression;
}

std::vector<std::vector<std::string>>
AST_HELPER_split(const std::vector<std::string> &input) {
  std::vector<std::vector<std::string>> result;
  std::vector<std::string> tokens;
  int parenthesesLevel = 0;

  for (const std::string &token : input) {
    if (token == "(") {
      ++parenthesesLevel;
    } else if (token == ")") {
      --parenthesesLevel;
    } else if (token == "," && parenthesesLevel == 0) {
      result.push_back(tokens);
      tokens.clear();
      continue; // Skip adding ',' to tokens
    }
    tokens.push_back(token);
  }

  if (!tokens.empty()) {
    result.push_back(tokens);
  }

  return result;
}

astNode *AST_HELPER_inToTree(const std::vector<std::string> &infix) {
  std::stack<astNode *> nodeStack;
  std::stack<std::string> symbolStack;
  std::map<std::string, int> priority;

  for (size_t i = 0; i < infix.size(); ++i) {
    const std::string &s = infix[i];
    if (s == "(") {
      symbolStack.push("(");
    } else if (AST_HELPER_isDigit(s)) {
      astNode *numNode = new astNode;
      numNode->map["type"] = "intLiteral";
      numNode->map["value"] = s;
      nodeStack.push(numNode);
    } else if (AST_HELPER_isVariableToken(s)) {
      // Check if it's a function call
      if (i + 1 < infix.size() - 1 && infix[i + 1] == "(") {
        // Function call
        astNode *funcCallNode = new astNode;
        funcCallNode->map["type"] = "funcCall";
        funcCallNode->map["name"] = s;

        // Seperating arguments
        size_t openParen = i + 1;
        size_t closingParen = AST_HELPER_closeIndex(infix, openParen);
        std::vector<std::string> fullArgTokens =
            AST_HELPER_getExpr(infix, openParen, closingParen);

        std::vector<std::vector<std::string>> expressions =
            AST_HELPER_split(fullArgTokens);
        std::vector<astNode *> arguments;

        for (int j = 0; j < expressions.size(); j++) {
          arguments.push_back(AST_HELPER_inToTree(expressions[j]));
        }

        funcCallNode->body = arguments;
        nodeStack.push(funcCallNode);
        i = closingParen;
      } else {
        // Variable
        astNode *varNode = new astNode;
        varNode->map["type"] = "variable";
        varNode->map["name"] = s;
        nodeStack.push(varNode);
      }
    } else if (s == ")") {
      while (symbolStack.top() != "(") {
        astNode *opNode = new astNode;
        opNode->map["type"] = "operator";
        opNode->map["value"] = symbolStack.top();
        symbolStack.pop();

        astNode *rightNode = nodeStack.top();
        nodeStack.pop();
        astNode *leftNode = nodeStack.top();
        nodeStack.pop();

        opNode->body.push_back(leftNode);
        opNode->body.push_back(rightNode);

        nodeStack.push(opNode);
      }
      symbolStack.pop();
    } else if (AST_HELPER_isOperator(s)) {
      while (!symbolStack.empty() && AST_HELPER_compare(symbolStack.top(), s)) {
        astNode *opNode = new astNode;
        opNode->map["type"] = "operator";
        opNode->map["value"] = symbolStack.top();
        symbolStack.pop();

        astNode *rightNode = nodeStack.top();
        nodeStack.pop();
        astNode *leftNode = nodeStack.top();
        nodeStack.pop();

        opNode->body.push_back(leftNode);
        opNode->body.push_back(rightNode);

        nodeStack.push(opNode);
      }
      symbolStack.push(s);
    }
  }

  while (!symbolStack.empty()) {
    astNode *opNode = new astNode;
    opNode->map["type"] = "operator";
    opNode->map["value"] = symbolStack.top();
    symbolStack.pop();

    astNode *rightNode = nodeStack.top();
    nodeStack.pop();
    astNode *leftNode = nodeStack.top();
    nodeStack.pop();

    opNode->body.push_back(leftNode);
    opNode->body.push_back(rightNode);

    nodeStack.push(opNode);
  }

  return nodeStack.top();
}

void AST::deleteTabs(std::vector<std::string> &expression) {
  expression.erase(expression.begin(), expression.begin() + tabLevel);
}

astNode *AST::assignExpr(std::vector<std::string> &expression) {
  if (!AST_HELPER_isVariableToken(expression[0])) {
    throw;
  }
  if (expression.size() < 3) {
    throw;
  }

  currentLine++;

  astNode *assignNode = new astNode;
  assignNode->map["type"] = "assign";

  astNode *varNode = new astNode;
  varNode->map["type"] = "variable";
  varNode->map["name"] = expression[0];
  assignNode->body.push_back(varNode);

  if (expression.size() == 3) {
    if (AST_HELPER_isDigit(expression[2])) {
      astNode *intNode = new astNode;
      intNode->map["type"] = "intLiteral";
      intNode->map["value"] = expression[2];
      assignNode->body.push_back(intNode);
      return assignNode;
    } else if (AST_HELPER_isVariableToken(expression[2])) {
      astNode *secondVarNode = new astNode;
      secondVarNode->map["type"] = "variable";
      secondVarNode->map["name"] = expression[2];
      assignNode->body.push_back(secondVarNode);
      return assignNode;
    }
  }

  std::vector<std::string> arthiExpr =
      std::vector<std::string>(expression.begin() + 2, expression.end());

  astNode *treeExpression = new astNode;
  treeExpression->map["type"] = "expression";
  treeExpression->body.push_back(AST_HELPER_inToTree(arthiExpr));

  assignNode->body.push_back(treeExpression);

  // Skip over to next line in tokenVector

  return assignNode;
}

// Helper to check if a token is a string literal (starts and ends with matching quotes)
static bool AST_HELPER_isStringLiteral(const std::string &token) {
  if (token.size() < 2) {
    return false;
  }
  char first = token[0];
  char last = token[token.size() - 1];
  return (first == '"' && last == '"') || (first == '\'' && last == '\'');
}

// Helper to extract the content from a string literal (removes surrounding quotes)
static std::string AST_HELPER_getStringContent(const std::string &token) {
  if (token.size() < 2) {
    return "";
  }
  return token.substr(1, token.size() - 2);
}

astNode *AST::printFunc(std::vector<std::string> &expression) {
  if (!(expression[1] == "(")) {
    throw;
  }

  astNode *printNode = new astNode;
  printNode->map["type"] = "print";

  for (size_t i = 2; i < expression.size(); i++) {
    const std::string &token = expression[i];

    if (AST_HELPER_isStringLiteral(token)) {
      // Handle both single and double quoted strings
      astNode *strNode = new astNode;
      strNode->map["type"] = "strLiteral";
      strNode->map["value"] = AST_HELPER_getStringContent(token);
      printNode->body.push_back(strNode);
    } else if (AST_HELPER_isVariableToken(token)) {
      astNode *variableNode = new astNode;
      variableNode->map["type"] = "variable";
      variableNode->map["name"] = token;
      printNode->body.push_back(variableNode);
    } else if (AST_HELPER_isDigit(token)) {
      astNode *numNode = new astNode;
      numNode->map["type"] = "intLiteral";
      numNode->map["value"] = token;
      printNode->body.push_back(numNode);
    } else if (token == ")") {
      break;
    } else if (token == ",") {
      continue;
    }
  }

  currentLine++;

  return printNode;
}

// Helper to check if a token is a comparison operator
static bool AST_HELPER_isComparisonOp(const std::string &token) {
  return token == "==" || token == "!=" || token == "<" || token == ">" ||
         token == "<=" || token == ">=";
}

astNode *AST::createifConditional(std::vector<std::string> &expression) {
  // Expected format: "if" <operand1> <comparison_op> <operand2> ":"
  // With new tokenizer, comparison operators are single tokens (e.g., "==")
  // So minimum size is 5: ["if", operand1, op, operand2, ":"]

  // Guard Cases
  if (expression.size() < 5) {
    throw;
  }
  if (expression[expression.size() - 1] != ":") {
    throw;
  }
  if (!(AST_HELPER_isVariableToken(expression[1]) ||
        AST_HELPER_isDigit(expression[1]))) {
    throw;
  }

  // The comparison operator is now a single token at index 2
  const std::string &compOp = expression[2];
  if (!AST_HELPER_isComparisonOp(compOp)) {
    throw;
  }

  // The second operand is at index 3 (not 4, since operator is single token)
  if (!(AST_HELPER_isVariableToken(expression[3]) ||
        AST_HELPER_isDigit(expression[3]))) {
    throw;
  }

  astNode *ifNode = new astNode;
  astNode *logicOp = new astNode;
  astNode *operand1 = new astNode;
  astNode *operand2 = new astNode;

  ifNode->map["type"] = "if";
  logicOp->map["type"] = "logicOp";
  logicOp->map["value"] = compOp;

  if (AST_HELPER_isVariableToken(expression[1])) {
    operand1->map["type"] = "variable";
    operand1->map["name"] = expression[1];
  } else {
    operand1->map["type"] = "intLiteral";
    operand1->map["value"] = expression[1];
  }

  if (AST_HELPER_isVariableToken(expression[3])) {
    operand2->map["type"] = "variable";
    operand2->map["name"] = expression[3];
  } else {
    operand2->map["type"] = "intLiteral";
    operand2->map["value"] = expression[3];
  }

  logicOp->body.push_back(operand1);
  logicOp->body.push_back(operand2);
  ifNode->body.push_back(logicOp);

  currentLine++;

  return ifNode;
}

astNode *AST::conditionBranch(std::vector<std::vector<std::string>> &tokenVec,
                              bool &except) {
  // Increments tab level of user
  tabLevel++;
  astNode *conditionNode = new astNode;
  conditionNode->map["type"] = "branch";
  astNode *ifNode = createifConditional(tokenVec[currentLine]);
  while (tokenVec[currentLine].size() >= tabLevel &&
         tokenVec[currentLine][tabLevel - 1] == "tab") {
    deleteTabs(tokenVec[currentLine]);
    if ((tokenVec.size() - 1) == currentLine) {
      astNode *blockNode = blockStatements(tokenVec, except);
      if (blockNode) {
        ifNode->body.push_back(blockNode);
      }
      conditionNode->body.push_back(ifNode);
      return conditionNode;
    } else {
      astNode *blockNode = blockStatements(tokenVec, except);
      if (blockNode) {
        ifNode->body.push_back(blockNode);
      }
    }
  }
  tabLevel--;

  conditionNode->body.push_back(ifNode);

  if (tokenVec[currentLine][tabLevel] == "else") {
    deleteTabs(tokenVec[currentLine]);
    if (tokenVec[currentLine][1] != ":") {
      delete conditionNode;
      delete ifNode;
      throw;
    }
    astNode *elseNode = new astNode;
    elseNode->map["type"] = "else";
    tabLevel++;
    currentLine++;
    while (tokenVec[currentLine].size() >= tabLevel &&
           tokenVec[currentLine][tabLevel - 1] == "tab") {
      deleteTabs(tokenVec[currentLine]);
      if ((tokenVec.size() - 1) == currentLine) {
        astNode *blockNode = blockStatements(tokenVec, except);
        if (blockNode) {
          elseNode->body.push_back(blockNode);
        }
        conditionNode->body.push_back(elseNode);
        return conditionNode;
      } else {
        astNode *blockNode = blockStatements(tokenVec, except);
        if (blockNode) {
          elseNode->body.push_back(blockNode);
        }
      }
    }
    tabLevel--;
    conditionNode->body.push_back(elseNode);
  }

  return conditionNode;
}

astNode *AST::returnExpr(std::vector<std::string> &expression) {
  if (expression.size() < 2) {
    throw;
  }

  currentLine++;

  astNode *returnNode = new astNode;
  returnNode->map["type"] = "return";

  if (expression.size() == 2) {
    if (AST_HELPER_isDigit(expression[1])) {
      astNode *intNode = new astNode;
      intNode->map["type"] = "intLiteral";
      intNode->map["value"] = expression[1];
      returnNode->body.push_back(intNode);
      return returnNode;
    } else if (AST_HELPER_isVariableToken(expression[1])) {
      astNode *secondVarNode = new astNode;
      secondVarNode->map["type"] = "variable";
      secondVarNode->map["name"] = expression[1];
      returnNode->body.push_back(secondVarNode);
      return returnNode;
    }
  }

  std::vector<std::string> arthiExpr =
      std::vector<std::string>(expression.begin() + 1, expression.end());

  astNode *treeExpression = new astNode;
  treeExpression->map["type"] = "expression";
  treeExpression->body.push_back(AST_HELPER_inToTree(arthiExpr));

  returnNode->body.push_back(treeExpression);

  // Skip over to next line in tokenVector

  return returnNode;
}
// Checking for statements that can exist in any code block
astNode *AST::blockStatements(std::vector<std::vector<std::string>> &tokenVec,
                              bool &except) {
  if (tokenVec[currentLine].empty()) {
    currentLine++;
    return nullptr;
  }

  if (tokenVec[currentLine].size() > 1 && tokenVec[currentLine][1] == "=") {
    try {
      return this->assignExpr(tokenVec[currentLine]);
    } catch (std::string e) {
      std::string str;
      for (const std::string &s : tokenVec[currentLine]) {
        str += s;
      }
      std::cout << "Invalid assigment:\n" << str << "\n";
      except = true;
    }
  } else if (tokenVec[currentLine][0] == "if") {
    try {
      return this->conditionBranch(tokenVec, except);
    } catch (std::string e) {
      std::string str;
      for (const std::string &s : tokenVec[currentLine]) {
        str += s;
      }
      std::cout << "Invalid conditional:\n" << str << "\n";
      except = true;
    }
  } else if (tokenVec[currentLine][0] == "print") {
    try {
      return this->printFunc(tokenVec[currentLine]);
    } catch (std::string e) {
      std::string str;
      for (const std::string &s : tokenVec[currentLine]) {
        str += s;
      }
      std::cout << "Invalid print function:\n" << str << "\n";
      except = true;
    }
  } else if (tokenVec[currentLine][0] == "return") {
    try {
      return this->returnExpr(tokenVec[currentLine]);
    } catch (std::string e) {
      std::string str;
      for (const std::string &s : tokenVec[currentLine]) {
        str += s;
      }
      std::cout << "Invalid print function:\n" << str << "\n";
      except = true;
    }
  }
  return nullptr;
}

astNode *AST::funcDef(std::vector<std::vector<std::string>> &tokenVec,
                      bool &except) {

  astNode *funcAST = new astNode;
  funcAST->map["type"] = "funcDef";
  funcAST->map["name"] = tokenVec[currentLine][1];

  size_t i = 3;
  astNode *paramsNode = new astNode;
  paramsNode->map["type"] = "params";
  const std::vector<std::string> &expression = tokenVec[currentLine];
  while (i < expression.size() && expression[i] != ")") {
    if (AST_HELPER_isVariableToken(expression[i])) {
      astNode *parNode = new astNode;
      parNode->map["type"] = "parameter";
      parNode->map["name"] = expression[i];
      paramsNode->body.push_back(parNode);
    }
    ++i;
  }

  funcAST->body.push_back(paramsNode);

  currentLine++;
  tabLevel++;

  astNode *blockNodes = new astNode;
  blockNodes->map["type"] = "blockStatements";
  while (tokenVec[currentLine].size() >= tabLevel &&
         tokenVec[currentLine][tabLevel - 1] == "tab") {
    deleteTabs(tokenVec[currentLine]);
    if ((tokenVec.size() - 1) == currentLine) {
      astNode *blockNode = blockStatements(tokenVec, except);
      if (blockNode) {
        blockNodes->body.push_back(blockNode);
      }
      funcAST->body.push_back(blockNodes);
      return funcAST;
    } else {
      astNode *blockNode = blockStatements(tokenVec, except);
      if (blockNode) {
        blockNodes->body.push_back(blockNode);
      }
    }
  }
  tabLevel--;

  funcAST->body.push_back(blockNodes);

  return funcAST;
}

void AST::parseFile(std::string pyFile) {

  std::vector<std::vector<std::string>> tokenVector;
  Tokenizer tokenizer;

  tokenVector = tokenizer.tokenize(pyFile);

  this->tree = new astNode;

  this->tree->map["type"] = "program";

  bool foundException = false;
  while (currentLine < tokenVector.size()) {
    /*
    Checks over statements that can appear on any line, such as if, else,
    assignment, print function
    */
    astNode *checkBlock = blockStatements(tokenVector, foundException);

    if (checkBlock) {
      this->tree->body.push_back(checkBlock);
      continue;
    } else if (tokenVector[currentLine][0] == "def") {

      astNode *funcAST = funcDef(tokenVector, foundException);

      functionTable[funcAST->map["name"]] = funcAST;

      // FIXME: Add support for function declarations,
      // including storing number of args, expressions, and return statement
      continue;
    }
  }
}

void AST::printRecursive(astNode *node, int depth = 0) {
  // Print indentation based on the depth of the node
  for (int i = 0; i < depth; ++i)
    std::cout << "  ";

  // Print the node's map
  std::cout << "Node Map:" << std::endl;
  for (const auto &pair : node->map) {
    for (int i = 0; i <= depth; ++i)
      std::cout << "  ";
    std::cout << pair.first << ": " << pair.second << std::endl;
  }

  // Print the children of the node
  for (astNode *child : node->body) {
    printRecursive(child, depth + 1); // Recursive call for each child
  }
}

void AST::print() { this->printRecursive(this->tree); }

int AST::evalTree(astNode *node) {
  // Check if the node is an integer literal
  if (node->map["type"] == "intLiteral") {
    return std::stoi(node->map["value"]);
  }

  if (node->map["type"] == "variable") {

    if (symbolTable.find(node->map["name"]) != symbolTable.end()) {
      return symbolTable[node->map["name"]];
    }

    AST *currScope = parentScope;

    while (currScope != nullptr) {
      if (currScope->symbolTable.find(node->map["name"]) !=
          currScope->symbolTable.end()) {
        return currScope->symbolTable[node->map["name"]];
      }
      currScope = currScope->parentScope;
    }

    std::cout << "VARIABLE NOT FOUND. ERROR\n";

    return -999;
  }

  // If it's an operator node, evaluate its children recursively
  if (node->map["type"] == "operator") {
    std::string op = node->map["value"];
    int result = evalTree(node->body[0]);
    for (int i = 1; i < node->body.size(); ++i) {
      astNode *child = node->body[i];
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

  if (node->map["type"] == "funcCall") {

    std::string funcName = node->map["name"];

    astNode *funcTree = functionTable[funcName];

    AST *funcAST = new AST;

    funcAST->parentScope = this;
    funcAST->tree = funcTree;
    funcAST->functionTable = this->functionTable;

    astNode *params = funcAST->tree->body[0];
    astNode *funcStatements = funcAST->tree->body[1];

    astNode *paramNode = nullptr;
    astNode *argNode = nullptr;

    for (int i = 0; i < params->body.size(); i++) {
      paramNode = params->body[i];
      argNode = node->body[i];

      funcAST->symbolTable[paramNode->map["name"]] = this->evalTree(argNode);
    }

    funcAST->runRecursively(funcStatements, 0);

    int returnValue = funcAST->returnVal;

    return returnValue;
  }

  if (node->map["type"] == "expression") {
    return evalTree(node->body[0]);
  }

  return -1;
}

bool AST::evalLogic(astNode *node) {
  astNode *op = node;
  astNode *LHS = op->body[0];
  astNode *RHS = op->body[1];

  int valueLHS;
  int valueRHS;

  if (LHS->map["type"] == "variable") {
    valueLHS = symbolTable[LHS->map["name"]];
  } else if (LHS->map["type"] == "intLiteral") {
    valueLHS = std::stoi(LHS->map["value"]);
  }

  if (RHS->map["type"] == "variable") {
    valueRHS = symbolTable[RHS->map["name"]];
  } else if (RHS->map["type"] == "intLiteral") {
    valueRHS = std::stoi(RHS->map["value"]);
  }

  if (op->map["value"] == "<")
    return valueLHS < valueRHS;
  else if (op->map["value"] == ">")
    return valueLHS > valueRHS;
  else if (op->map["value"] == "==")
    return valueLHS == valueRHS;
  else if (op->map["value"] == "!=")
    return valueLHS != valueRHS;
  else if (op->map["value"] == ">=")
    return valueLHS >= valueRHS;
  else if (op->map["value"] == "<=")
    return valueLHS <= valueRHS;

  return false;
}

void AST::runRecursively(astNode *node, int i = 0) {
  if (node == nullptr) {
    return;
  }

  for (i = 0; i < node->body.size(); i++) {
    if (returnFound) {
      break;
    }
    astNode *child = node->body[i];
    if (child->map["type"] == "assign") {
      std::string var = child->body[0]->map["name"];
      astNode *RHS = child->body[1];
      std::string typeRHS = RHS->map["type"];
      if (typeRHS == "intLiteral") {
        this->symbolTable[var] = std::stoi(RHS->map["value"]);
      } else if (typeRHS == "variable") {
        this->symbolTable[var] = symbolTable[RHS->map["name"]];
      } else if (typeRHS == "expression") {
        this->symbolTable[var] = evalTree(RHS->body[0]);
      }
    } else if (child->map["type"] == "print") {
      for (int j = 0; j < child->body.size(); j++) {
        astNode *printNode = child->body[j];
        std::string typePrint = printNode->map["type"];
        if (typePrint == "strLiteral" || typePrint == "intLiteral") {
          std::cout << printNode->map["value"];
        } else if (typePrint == "variable") {
          std::cout << symbolTable[printNode->map["name"]];
        }
        if (j == child->body.size() - 1) {
          if (!(i == node->body.size() - 1)) {
            std::cout << "\n";
          }
        } else {
          std::cout << " ";
        }
      }
    } else if (child->map["type"] == "branch") {
      astNode *ifNode = child->body[0];
      bool isTrue = evalLogic(ifNode->body[0]);
      if (isTrue) {
        runRecursively(ifNode, 1);
      } else {
        if (child->body.size() == 2) {
          astNode *elseNode = child->body[1];
          runRecursively(elseNode);
        }
      }
    } else if (child->map["type"] == "return") {
      returnVal = evalTree(child->body[0]);
      returnFound = true;
      return;
    }
  }
}

void AST::runFile() { runRecursively(this->tree, 0); }

// Deconstructor
AST::~AST() {
  if (tree != nullptr) {
    delete tree;
  }
  for (auto &pair : functionTable) {
    delete pair.second; // Deleting the astNode pointer
  }
}
