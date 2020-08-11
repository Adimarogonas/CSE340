/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    INT, REAL, BOOL, TRUE, FALSE, IF, WHILE, SWITCH, CASE, PUBLIC, PRIVATE, NOT, DOT, PLUS, MINUS, DIV, MULT, EQUAL, COLON,
    COMMA, SEMICOLON, LBRACE, RBRACE, LPAREN, RPAREN, LTEQ, NOTEQUAL, LESS, GTEQ, GREATER, 
    REALNUM, NUM, ID, ERROR// TODO: Add labels for new token types here
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    char Peek();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
    Token scanRealNum();
};

#endif  //__LEXER__H__
