# PyinCPP

## A basic Python interpreter written in C++ by @rtrevizo18

### Compilation:

*Note*: *mypython* uses some C++11 features, so it must be compiled with -std=c++11 flag

**IF USING G++ WITH WILDCARD**: g++ -std=c++11 *.cpp -o mypython

**EASIER WITH INCLUDED MAKEFILE**: make

### Design:

**Tokenizer**: 
    Breaks down the file into tokens, which are grouped together in an expression vector, then those expressions are grouped into full token vector (i.e. 2D vector where inner vector stores lines and outer vector contains all of the lines of the program). It seeks out for specific keywords/symbols that are apart of Python syntax. User-defined symbols, such as variable and function names, become their own token. For example: "def foo(x):" becomes {"def", "foo", "(", "x", ")", ":"}. Additionally, it makes identation visible by tagging it with a "tab" token, making evaluation easier. Defined in tokenizer.h/tokenizer.cpp.

**AST Creation**:
    Creates a parsable AST by evaluating tokens on a line-by-line basis. Based on context from specific symbols/keywords in the line, the code can determine the kind of statement and goes down its corresponding code path. Nodes in the tree contain a map corresponding to the node's specific attributes and a vector of node pointers, which serve as its children (Refactoring the map into string properties in the node is likely much more performant, but less flexible. In this case, a map isn't needed, but functionality is built around it, so it would take a lot of time to remake).

**AST Parsing**:
    After the AST is created, the program runs by parsing the tree.