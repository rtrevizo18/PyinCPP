#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <sstream>

#include "tokenizer.h"

int main(int argc, char* argv[]){

    std::ifstream file ("testcases/in03.py");
    std::string fileString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();

    Tokenizer test;

    test.tokenize(fileString);

    return 0;
}