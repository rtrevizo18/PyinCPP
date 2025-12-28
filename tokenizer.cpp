#include "tokenizer.h"

// Helper function to check if a string starts with a given prefix
std::string Tokenizer::checkSub(std::string subString, std::string string) {
  if (subString.empty() || string.empty()) {
    return "";
  }
  if (string.length() < subString.length()) {
    return "";
  }
  std::string stringSubbed = string.substr(0, subString.length());
  if (subString == stringSubbed) {
    return stringSubbed;
  }
  return "";
}

// Helper to check if a character can continue an identifier (alphanumeric or underscore)
static bool isIdentifierChar(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

// Helper to check if a character can start an identifier (alpha or underscore)
static bool isIdentifierStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

std::vector<std::string> Tokenizer::tokenizeHelper(const std::string input) {
  std::vector<std::string> tokenVector;
  std::string inputString = input;

  // Multi-character operators must be checked before single-character ones
  // Order matters: longer operators first to avoid partial matches
  const std::vector<std::string> multiCharOps = {"==", "!=", "<=", ">=", "**"};

  // Single-character operators and symbols
  // NOTE: '\t' removed - tabs are handled separately as indentation
  const std::vector<char> opsyms = {'>', '<', '=', '*', '+', '-',
                                    '/', '(', ')', ':', ',', '!',
                                    '%', '[', ']', '{', '}'};

  const std::vector<std::string> keywords = {"return", "def",   "if",    "else",
                                             "elif",   "while", "for",   "in",
                                             "and",    "or",    "not",   "True",
                                             "False",  "None",  "break", "continue"};

  // Cut out any comments (but not inside strings - simplified: just find first #)
  // Note: This is a simplification; proper handling would track string state
  size_t foundComment = inputString.find('#');
  if (foundComment != std::string::npos) {
    inputString = inputString.substr(0, foundComment);
  }

  // Track if we're at the beginning of the line for indentation handling
  bool atLineStart = true;

  while (!inputString.empty()) {

    // Handle indentation at the start of the line
    // Indentation is significant only at the beginning of a logical line
    if (atLineStart) {
      // Handle tab character: treat as one indentation level
      if (inputString[0] == '\t') {
        tokenVector.push_back("tab");
        inputString = inputString.substr(1);
        continue; // Still at line start, might have more indentation
      }

      // Handle 4 spaces as one indentation level
      if (this->checkSub("    ", inputString) != "") {
        tokenVector.push_back("tab");
        inputString = inputString.substr(4);
        continue; // Still at line start, might have more indentation
      }

      // Handle 2 spaces as one indentation level (common alternative)
      // Uncomment if you want to support 2-space indentation:
      // if (this->checkSub("  ", inputString) != "") {
      //   tokenVector.push_back("tab");
      //   inputString = inputString.substr(2);
      //   continue;
      // }

      // Any other character means we're done with indentation
      atLineStart = false;
    }

    // Skip over whitespace (not at line start, so it's just spacing)
    if (inputString[0] == ' ' || inputString[0] == '\t') {
      inputString = inputString.substr(1);
      continue;
    }

    // Check for string literals (both single and double quotes)
    if (inputString[0] == '"' || inputString[0] == '\'') {
      char quoteChar = inputString[0];
      std::string strLiteral;
      strLiteral += quoteChar;
      size_t i = 1;

      while (i < inputString.size()) {
        char c = inputString[i];
        strLiteral += c;

        // Handle escape sequences
        if (c == '\\' && i + 1 < inputString.size()) {
          i++;
          strLiteral += inputString[i];
          i++;
          continue;
        }

        // Found closing quote
        if (c == quoteChar) {
          i++;
          break;
        }
        i++;
      }

      tokenVector.push_back(strLiteral);
      inputString = inputString.substr(i);
      continue;
    }

    // Check for multi-character operators first (before single-char)
    bool foundMultiOp = false;
    for (const std::string &op : multiCharOps) {
      if (this->checkSub(op, inputString) != "") {
        tokenVector.push_back(op);
        inputString = inputString.substr(op.length());
        foundMultiOp = true;
        break;
      }
    }
    if (foundMultiOp) {
      continue;
    }

    // Check for single-character operators/symbols
    if (std::find(opsyms.begin(), opsyms.end(), inputString[0]) !=
        opsyms.end()) {
      std::string s(1, inputString[0]);
      tokenVector.push_back(s);
      inputString = inputString.substr(1);
      continue;
    }

    // Check for keywords - but only if followed by a non-identifier character
    // This prevents "define" from matching "def"
    bool foundKeyWord = false;
    for (const std::string &keyword : keywords) {
      std::string checkSubString = this->checkSub(keyword, inputString);
      if (!checkSubString.empty()) {
        // Verify the keyword is not part of a longer identifier
        size_t keyLen = keyword.length();
        if (keyLen >= inputString.length() ||
            !isIdentifierChar(inputString[keyLen])) {
          tokenVector.push_back(checkSubString);
          inputString = inputString.substr(keyLen);
          foundKeyWord = true;
          break;
        }
      }
    }
    if (foundKeyWord) {
      continue;
    }

    // Check for identifiers (variables and function names)
    if (isIdentifierStart(inputString[0])) {
      std::string identifier;
      size_t i = 0;
      while (i < inputString.size() && isIdentifierChar(inputString[i])) {
        identifier += inputString[i];
        i++;
      }
      tokenVector.push_back(identifier);
      inputString = inputString.substr(i);
      continue;
    }

    // Check for numbers (integers and floats)
    if (std::isdigit(static_cast<unsigned char>(inputString[0])) ||
        (inputString[0] == '.' && inputString.size() > 1 &&
         std::isdigit(static_cast<unsigned char>(inputString[1])))) {
      std::string number;
      size_t i = 0;
      bool hasDecimal = false;

      while (i < inputString.size()) {
        char c = inputString[i];
        if (std::isdigit(static_cast<unsigned char>(c))) {
          number += c;
          i++;
        } else if (c == '.' && !hasDecimal) {
          // Check if this is a decimal point (not a method call like 1.something)
          if (i + 1 < inputString.size() &&
              std::isdigit(static_cast<unsigned char>(inputString[i + 1]))) {
            number += c;
            hasDecimal = true;
            i++;
          } else if (number.empty()) {
            // .5 style number
            number += c;
            hasDecimal = true;
            i++;
          } else {
            break;
          }
        } else {
          break;
        }
      }
      tokenVector.push_back(number);
      inputString = inputString.substr(i);
      continue;
    }

    // If no match found, skip the character (with a warning in debug mode)
    // This handles any unexpected characters gracefully
    inputString = inputString.substr(1);
  }

  return tokenVector;
}

std::vector<std::vector<std::string>>
Tokenizer::tokenize(const std::string input) {
  std::vector<std::vector<std::string>> expressionVector;
  std::string line;
  std::stringstream inputStream(input);

  while (std::getline(inputStream, line)) {
    expressionVector.push_back(this->tokenizeHelper(line));
  }

  // Remove empty lines (blank lines or comment-only lines)
  auto iterator = expressionVector.begin();
  while (iterator != expressionVector.end()) {
    if (iterator->empty()) {
      iterator = expressionVector.erase(iterator);
    } else {
      ++iterator;
    }
  }

  return expressionVector;
}
