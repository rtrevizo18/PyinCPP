#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

class Tokenizer{
    private:
    std::string checkSub(std::string, std::string);
    std::vector<std::string>tokenizeHelper(const std::string);
    public:
    std::vector<std::vector<std::string> > tokenize(const std::string);
};



#endif