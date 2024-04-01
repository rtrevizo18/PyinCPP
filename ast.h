#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <map>
#include <stack>

#include "tokenizer.h"

struct astNode{
    std::map<std::string, std::string> map;
    std::vector<astNode*> body;
};

class AST{
    private:
    AST* parentScope = nullptr;
    astNode* AST;
    std::map<std::string, int> symbolTable;
    std::map<std::string, std::map<std::string, std::string> > functionTable;
    astNode* blockStatement(const std::vector<std::vector<std::string> >&);
    astNode* printFunc(const std::vector<std::string>&);
    astNode* assignExpr(const std::vector<std::string>&);
    astNode* conditionBranch(const std::vector<std::string>&);
    void printRecursive(astNode* node, int depth);
    public:
    void parseFile(std::string pyFile);
    void print();
    void runFile();
};

#endif