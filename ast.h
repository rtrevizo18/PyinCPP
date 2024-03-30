#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <map>

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
    astNode* printFunc(const std::vector<std::string>&);
    astNode* assignExpr(const std::vector<std::string>&);
    void functionCheck();
    public:
    void parseFile(std::string pyFile);
};

#endif