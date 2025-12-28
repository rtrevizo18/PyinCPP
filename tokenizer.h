#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @class Tokenizer
 * @brief Breaks down Python source code into tokens for AST construction.
 *
 * The tokenizer processes Python source code line-by-line, producing a 2D vector
 * where each inner vector represents the tokens of a single line.
 *
 * Token types produced:
 * - "tab"           : Indentation level (1 tab char OR 4 spaces = 1 "tab" token)
 * - Keywords        : "def", "if", "else", "elif", "return", "while", "for", etc.
 * - Identifiers     : Variable/function names (e.g., "foo", "my_var", "_private")
 * - Operators       : Single-char ("+", "-", "*", "/", "<", ">", "=", etc.)
 *                     Multi-char ("==", "!=", "<=", ">=", "**")
 * - Literals        : Numbers ("42", "3.14"), Strings ("\"hello\"", "'world'")
 * - Delimiters      : "(", ")", ":", ",", "[", "]", "{", "}"
 *
 * Indentation handling:
 * - Tab characters ('\t') are converted to "tab" tokens
 * - 4 consecutive spaces at line start are converted to "tab" tokens
 * - Multiple indentation levels produce multiple "tab" tokens
 *
 * Comments:
 * - Everything after '#' on a line is ignored
 *
 * Empty lines:
 * - Blank lines and comment-only lines are removed from output
 */
class Tokenizer {
private:
  /**
   * @brief Check if a string starts with a given prefix
   * @param subString The prefix to check for
   * @param string The string to check
   * @return The matched prefix if found, empty string otherwise
   */
  std::string checkSub(std::string subString, std::string string);

  /**
   * @brief Tokenize a single line of Python code
   * @param input A single line of source code
   * @return Vector of tokens for this line
   */
  std::vector<std::string> tokenizeHelper(const std::string input);

public:
  /**
   * @brief Tokenize an entire Python source file
   * @param input The complete source code as a string
   * @return 2D vector: outer = lines, inner = tokens per line
   *
   * Example:
   *   Input: "def foo():\n    return 1"
   *   Output: [["def", "foo", "(", ")", ":"],
   *            ["tab", "return", "1"]]
   */
  std::vector<std::vector<std::string>> tokenize(const std::string input);
};

#endif
