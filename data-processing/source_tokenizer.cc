#include "file.h"

#include <vector>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

// Struct that represents a lexical token, parsed from C++ source code.
struct Token {
  const char *start;
  const char *end;
  enum Type {
    kUnknown,
    kLineComment,
    kBlockComment,
    kPunctuation,
    kStringLiteral,
    kNumericLiteral,
    kIdentifier,
    kKeyword
  } type;
  int start_line;
  int finish_line;
};

const std::unordered_set<std::string> kKeywords = {
  "alignas", "alignof", "and", "and_eq",
  "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept",
  "auto", "bitand", "bitor", "bool",
  "break", "case", "catch", "char",
  "char16_t", "char32_t", "class", "compl",
  "concept", "const", "constexpr", "const_cast",
  "continue", "decltype", "default", "delete",
  "do", "double", "dynamic_cast", "else",
  "enum", "explicit", "export", "extern",
  "false", "float", "for", "friend",
  "goto", "if", "inline", "int",
  "import", "long", "module", "mutable",
  "namespace", "new", "noexcept", "not",
  "not_eq", "nullptr", "operator", "or",
  "or_eq", "private", "protected", "public",
  "register", "reinterpret_cast", "requires", "return",
  "short", "signed", "sizeof", "static",
  "static_assert", "static_cast", "struct", "switch",
  "synchronized", "template", "this", "thread_local",
  "throw", "true", "try", "typedef",
  "typeid", "typename", "union", "unsigned",
  "using", "virtual", "void", "volatile",
  "wchar_t", "while", "xor", "xor_eq"
};

// State pattern hack for C++.
// See: http://www.gotw.ca/gotw/057.htm
class Lexer;
struct State_;
typedef State_ (Lexer::*State)();
struct State_ {
  State_(State st) : st(st) {}
  operator State() { return st; }
  State st;
};

// C++ lexer.
class Lexer {
public:
  // Lexes the given sequence into a series of tokens.
  std::vector<Token> Lex(const char *begin, const char *end);

private:
  // Start a token of the given variety.
  void StartToken(Token::Type type);
  // Emit the current token.
  void EmitToken();
  // Open slate state - no active token.
  State_ Initial();
  // Currently working on a numeric literal token.
  State_ NumericLiteral();
  // Currently working on a string (or character) token.
  State_ StringLiteral();
  // Currently working on a raw string literal token.
  State_ RawStringLiteral();
  // Currently working on a line comment token.
  State_ LineComment();
  // currently working on a block comment token.
  State_ BlockComment();
  // currently working on an identifier token.
  State_ Identifier();
  // Error state.
  State_ Error();
  // Reached end of input.
  State_ End();

  const char *begin, *end;
  int line;
  Token current;

  std::vector<Token> tokens;
};

std::vector<Token> Lexer::Lex(const char *begin, const char *end)
{
  this->begin = begin;
  this->end = end;
  this->line = 1;
  this->current = Token{nullptr, nullptr, Token::kUnknown, 0, 0};
  State st = &Lexer::Initial;
  while (st != &Lexer::Error && st != &Lexer::End) {
    st = (this->*st)();
    this->begin++;
  }
  return this->tokens;
}

void Lexer::StartToken(Token::Type type)
{
  current.start = begin;
  current.start_line = line;
  current.type = type;
}

void Lexer::EmitToken()
{
  current.end = begin;
  current.finish_line = line;
  if (current.type == Token::kIdentifier) {
    if (kKeywords.count(std::string(current.start, current.end))) {
      current.type = Token::kKeyword;
    }
  }
  tokens.push_back(current);
}

State_ Lexer::Initial()
{
  if (begin >= end) {
    return &Lexer::End;
  }
  switch (*begin) {
    case '\xef': case '\xbb': case '\xbf': {
      // Unicode byte order mark.
      return &Lexer::Initial;
    }
    case '\x0c': {
      // Form feed character.
      return &Lexer::Initial;
    }
    case ' ': case '\t': case '\r': {
      return &Lexer::Initial;
    }
    case '\n': {
      line++;
      return &Lexer::Initial;
    }
    case '!': case '%': case '^': case '&':
    case '*': case '(': case ')': case '-':
    case '+': case '=': case '{': case '}':
    case '|': case '~': case '[': case ']':
    case '\\': case ';': case ':': case '<':
    case '>': case '?': case ',': case '#':
    case '.': {
      tokens.push_back(Token{
        begin, begin+1, Token::kPunctuation, line, line
      });
      return &Lexer::Initial;
    }
    case '/': {
      if (begin + 1 < end && *(begin+1) == '/') {
        StartToken(Token::kLineComment);
        begin++;
        return &Lexer::LineComment;
      } else if (begin + 1 < end && *(begin+1) == '*') {
        StartToken(Token::kBlockComment);
        begin++;
        return &Lexer::BlockComment;
      }
      tokens.push_back(Token{
        begin, begin+1, Token::kPunctuation, line, line
      });
      return &Lexer::Initial;
    }
    case '\'': case '"': {
      StartToken(Token::kStringLiteral);
      return &Lexer::StringLiteral;
    }
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9': {
      StartToken(Token::kNumericLiteral);
      return &Lexer::NumericLiteral;
    }
    default: {
      if (std::isalpha(*begin) || *begin == '_') {
        StartToken(Token::kIdentifier);
        return &Lexer::Identifier;
      }
    }
  }
  return &Lexer::Error;
}

State_ Lexer::NumericLiteral()
{
  if (begin >= end) {
    EmitToken();
    return &Lexer::End;
  }
  switch (*begin) {
    case ' ': case '\t': case '\r': {
      EmitToken();
      return &Lexer::Initial;
    }
    case '\n': {
      EmitToken();
      line++;
      return &Lexer::Initial;
    }
    case '!': case '%': case '^': case '&':
    case '*': case '(': case ')': case '-':
    case '+': case '=': case '{': case '}':
    case '|': case '~': case '[': case ']':
    case '\\': case ';': case ':': case '<':
    case '>': case '?': case ',': case '#':
    case '.': {
      EmitToken();
      tokens.push_back(Token{
        begin, begin+1, Token::kPunctuation, line, line
      });
      return &Lexer::Initial;
    }
    case '/': {
      EmitToken();
      if (begin + 1 < end && *(begin+1) == '/') {
        StartToken(Token::kLineComment);
        begin++;
        return &Lexer::LineComment;
      } else if (begin + 1 < end && *(begin+1) == '*') {
        StartToken(Token::kBlockComment);
        begin++;
        return &Lexer::BlockComment;
      }
      tokens.push_back(Token{
        begin, begin+1, Token::kPunctuation, line, line
      });
      return &Lexer::Initial;
    }
    // Ignore actual legitimacy of a numeric literal here.
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9': case 'x': case 'X':
    case 'A': case 'B': case 'C': case 'D':
    case 'E': case 'F': case 'a': case 'b':
    case 'c': case 'd': case 'e': case 'f':
    case 'u': case 'U': case 'l': case 'L':
    case '\'': {
      return &Lexer::NumericLiteral;
    }
  }
  return &Lexer::Error;
}

State_ Lexer::StringLiteral()
{
  if (begin >= end) {
    return &Lexer::Error;
  }
  switch (*begin) {
    case '\n': {
      return &Lexer::Error;
    }
    case '\\': {
      begin++;
      return &Lexer::StringLiteral;
    }
    default: {
      if (*begin == *current.start) {
        begin++;
        EmitToken();
        begin--;
        return &Lexer::Initial;
      }
      return &Lexer::StringLiteral;
    }
  }
  return &Lexer::Error;
}

State_ Lexer::RawStringLiteral()
{
  const char *pos = begin;
  while (*pos != '(') {
    pos++;
  }
  std::string match = std::string(begin, pos) + ")\"";
  begin = pos;
  while (begin < end - match.size()) {
    if (*begin == '\n') {
      line++;
    }
    if (std::string(begin, begin+match.size()) == match) {
      begin += match.size();
      EmitToken();
      return &Lexer::Initial;
    }
    begin++;
  }
  return &Lexer::Error;
}

State_ Lexer::LineComment()
{
  if (begin >= end) {
    EmitToken();
    return &Lexer::End;
  }
  switch (*begin) {
    case '\n': {
      EmitToken();
      line++;
      return &Lexer::Initial;
    }
    case '\\': {
      begin++;
      return &Lexer::LineComment;
    }
  }
  return &Lexer::LineComment;
}

State_ Lexer::BlockComment()
{
  if (begin >= end) {
    return &Lexer::Error;
  }
  switch (*begin) {
    case '\n': {
      line++;
      return &Lexer::BlockComment;
    }
    case '*': {
      if (begin + 1 < end && *(begin+1) == '/') {
        begin += 2;
        EmitToken();
        begin--;
        return &Lexer::Initial;
      }
    }
  }
  return &Lexer::BlockComment;
}

State_ Lexer::Identifier()
{
  if (begin >= end) {
    EmitToken();
    return &Lexer::End;
  }
  switch (*begin) {
    case ' ':
    case '\t':
    case '\r': {
      EmitToken();
      return &Lexer::Initial;
    }
    case '\n': {
      EmitToken();
      line++;
      return &Lexer::Initial;
    }
    case '!': case '%': case '^': case '&':
    case '*': case '(': case ')': case '-':
    case '+': case '=': case '{': case '}':
    case '|': case '~': case '[': case ']':
    case '\\': case ';': case ':': case '<':
    case '>': case '?': case ',': case '#':
    case '.': {
      EmitToken();
      tokens.push_back(Token{
        begin, begin+1, Token::kPunctuation, line, line
      });
      return &Lexer::Initial;
    }
    case '/': {
      EmitToken();
      if (begin + 1 < end && *(begin+1) == '/') {
        StartToken(Token::kLineComment);
        begin++;
        return &Lexer::LineComment;
      } else if (begin + 1 < end && *(begin+1) == '*') {
        StartToken(Token::kBlockComment);
        begin++;
        return &Lexer::BlockComment;
      }
      tokens.push_back(Token{
        begin, begin+1, Token::kPunctuation, line, line
      });
      return &Lexer::Initial;
    }
    case '"': {
      if (std::string(current.start, begin) == "R") {
        current.type = Token::kStringLiteral;
        return &Lexer::RawStringLiteral;
      } // FALLTHROUGH
    case '\'':
      EmitToken();
      StartToken(Token::kStringLiteral);
      return &Lexer::StringLiteral;
    }
    default: {
      if (std::isalnum(*begin) || *begin == '_') {
        return &Lexer::Identifier;
      }
    }
  }
  return &Lexer::Error;
}

State_ Lexer::Error()
{
  return &Lexer::Error;
}

State_ Lexer::End()
{
  return &Lexer::End;
}

}

std::vector<Token> LexSource(const char *begin, const char *end)
{
  Lexer lexer;
  return lexer.Lex(begin, end);
}

void PrintTokens(const char *begin, const char *end)
{
  while (begin < end) {
    while (std::isalnum(*begin) || *begin == '_') {
      std::cout << *begin++;
    }
    while (!std::isalnum(*begin) && *begin != '_') {
      begin++;
    }
    if (begin < end) {
      std::cout << ' ';
    }
  }
}

int main(int argc, char **argv)
{
  std::string s;
  std::ofstream titles(argv[1]);
  while (std::cin >> s) {
    titles << s << '\n';
    File f(s.c_str());
    const char *buf = (char*)f.buffer();
    std::vector<Token> tokens = LexSource(buf, buf + f.size());
    for (const Token &t : tokens) {
      if (t.type == Token::kLineComment
          || t.type == Token::kBlockComment
          || t.type == Token::kStringLiteral) {
        PrintTokens(t.start, t.end);
      } else {
        std::cout << std::string(t.start, t.end) << " ";
      }
    }
    std::cout << '\n';
  }
}
