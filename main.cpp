#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <sstream>

#include "tokenizer.h"
#include "ast.h"

int main(int argc, char* argv[]){


    std::ifstream file (argv[1]);
    std::string fileString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();

    AST tree;

    tree.parseFile(fileString);

    tree.runFile();

    return 0;
}