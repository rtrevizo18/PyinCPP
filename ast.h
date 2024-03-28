#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <map>

#include "tokenizer.h"

struct astNode{
    std::string name;
    std::string type;
    std::vector<astNode*> children;
};

struct functionDef{
    std::string name;
    std::vector<std::string> nameParams;
    int numParams;
    AST functionTree;
};

class expression{
    public:
    std::string type;
    expression(){
        this->type = "null";
    }
    virtual std::string getType() const {
        return this->type;
    }
};

class binaryExpr : public expression{
    private:
    int firstOp;
    int secondOp;
    std::string operation;
    public:
    binaryExpr(int f, int s, std::string op){
        this->firstOp = f;
        this->secondOp = s;
        this->operation = op;
        this->type = "binary";
    }
};

class assignExpr : public expression{
    private:
    std::string variableName;
    int valueAssigned;
    public:
    assignExpr(){
        this->type = "assign";
    }
};

class condiBranch : public expression{
    private:
    bool hasElse;
    std::string ifCondition;
    public:
    condiBranch(){
        this->type = "condition";
    }

};

class AST{
    private:
    AST* parentScope = nullptr;
    std::map<std::string, int> symbolTable;
    std::map<std::string, functionDef> functionTable;
    std::vector<astNode*> children;
    void check();
    void assignmentCreation();
    void functionCheck();
    public:
    void parseFile(std::string pyFile);
};

#endif