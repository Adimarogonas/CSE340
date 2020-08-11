/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "parser.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "INT", "REAL", "BOOL", "TRUE", "FALSE", "IF", "WHILE", "SWITCH",
    "CASE", "PUBLIC", "PRIVATE", "NOT", "DOT", "PLUS", "MINUS", "DIV", "MULT", "EQUAL",
    "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "LTEQ", "NOTEQUAL",
     "LESS", "GTEQ", "GREATER", "REALNUM", "NUM", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}
LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

char LexicalAnalyzer::Peek(){//checks a character without scanning it
	char c;
	input.GetChar(c);
	char ret = c;
	input.UngetChar(c);
	return ret;	
}
bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{//TODO: Figure methodology for base16 numbers
    char c;
    input.GetChar(c);
    if (isdigit(c)) {
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
           	}
     			
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
    }
        //REALNUM = NUM DOT digit digit*
    input.GetChar(c);//IF DOT IS FOUND BEGIN SCANNING REALNUM
    if(c == '.'){//if realnum, then it will be proceeded with a dot
        char temp = Peek();//set temporary character to help keep track of peek
        if(isdigit(temp)){
        	tmp.lexeme += ".";
        	return scanRealNum();
        } else {//if the temp is not a digit unget the dot and return a NUM token
        	input.UngetChar(c);
        	tmp.token_type = NUM;
        	tmp.line_no = line_no;
        	return tmp;
    	}
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    }
}
Token LexicalAnalyzer::scanRealNum(){//REALNUM NUM DOT digit digit*
	char c;
	input.GetChar(c);
	if(isdigit(c)){
		while (!input.EndOfInput() && isdigit(c)) {//scan all digits
			if(isdigit(c)){
    			tmp.lexeme += c;//add to lexeme if it is a digit
    		}
       		input.GetChar(c);
    	}
	} else {
		tmp.lexeme = "";
		tmp.token_type = ERROR;
	}
	input.UngetChar(c);//ungets the last char because while loop always goes one char too far
	tmp.token_type = REALNUM;
	tmp.line_no = line_no;
	return tmp;
}
Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
    	case '!':
    		tmp.token_type = NOT;
    		return tmp;
    		break;

        case '.':
            tmp.token_type = DOT;
            return tmp;
            break;

        case '+':
            tmp.token_type = PLUS;
            return tmp;
            break;

        case '-':
            tmp.token_type = MINUS;
            return tmp;
            break;

        case '/':
            tmp.token_type = DIV;
            return tmp;
            break;

        case '*':
            tmp.token_type = MULT;
            return tmp;
            break;

        case '=':
            tmp.token_type = EQUAL;
            return tmp;
            break;

        case ':':
            tmp.token_type = COLON;
            return tmp;
            break;

        case ',':
            tmp.token_type = COMMA;
            return tmp;
            break;

        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
            break;

        case '{':
            tmp.token_type = LBRACE;
            return tmp;
            break;

        case '}':
            tmp.token_type = RBRACE;
            return tmp;
            break;

        case '(':
            tmp.token_type = LPAREN;
            return tmp;
            break;

        case ')':
            tmp.token_type = RPAREN;
            return tmp;
            break;

        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;
    /*
    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }*/
    parser p;
    p.parse_program();
}
