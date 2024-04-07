#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#include "tokenizer.h"

struct astNode{
    std::map<std::string, std::string> map;
    std::vector<astNode*> body;

    ~astNode(){
        for(astNode* child : body){
            delete child;
        }
    }
};

class AST{
    private:
    AST* parentScope = nullptr;
    astNode* AST;
    std::map<std::string, int> symbolTable;
    std::map<std::string, std::map<std::string, std::string> > functionTable;
    size_t tabLevel = 0;
    astNode* blockStatements(std::vector<std::vector<std::string> >&, int&, bool&);
    astNode* printFunc(std::vector<std::string>&);
    astNode* assignExpr(std::vector<std::string>&);
    astNode* createifConditional(std::vector<std::string>&);
    astNode* conditionBranch(std::vector<std::vector<std::string> >&, int&, bool&);
    void deleteTabs(std::vector<std::string>&);
    void printRecursive(astNode*, int);
    void runProcess();
    void runRecursively(astNode*, int);
    int evalTree(astNode*);
    bool evalLogic(astNode*);
    public:
    void parseFile(std::string pyFile);
    void print();
    void runFile();
};

#endif