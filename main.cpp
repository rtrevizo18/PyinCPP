#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "ast.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <file_path>\n";
    return 1; // indicate error
  }

  std::string fileRelativePath = argv[1];

  std::ifstream file(fileRelativePath);
  std::string fileString = std::string(std::istreambuf_iterator<char>(file),
                                       std::istreambuf_iterator<char>());
  file.close();

  AST tree;

  tree.parseFile(fileString);

  tree.runFile();

  return 0;
}