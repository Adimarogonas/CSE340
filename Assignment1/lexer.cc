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
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

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
bool LexicalAnalyzer::isdigit8(char c){//0 to 7
		int digit = c - '0';
		if(digit < 8){
			return true;
		}
		else{
			return false;
		}

}
bool LexicalAnalyzer::ispdigit8(char c){//1 to 8
	int digit = c - '0';
	if(digit <= 7 && digit > 0){
		return true;
	}
	else{
		return false;
	}
}
bool LexicalAnalyzer::isdigit16(char c){//0 to F
	int digit = c - '0';
	if((digit <= 9 && digit >= 0) || (c >= 'A' && c <= 'F')){
		return true;
	} else {
		return false;
	}
}
bool LexicalAnalyzer::ispdigit16(char c){//1 to F
	int digit = c - '0';
	if((digit <= 9 && digit >= 1) || (c >= 'A' && c <= 'F')){
		return true;
	} else {
		return false;
	}
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
	bool isbase8 = true;//Assumes its base8. if contradiction is found it is set to false essentially performs PBC
    char c;
    input.GetChar(c);
    if (isdigit(c)) {
    	if(!ispdigit8(c) && !isdigit8(c)){
    		isbase8 = false;//check first digit for base 8
    	}
        if (c == '0') {
            tmp.lexeme = "0";

        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
                if(isdigit(c)){
                	if(!isdigit8(c)){//DIGIT 8 is a subset of a num
                		isbase8 = false;//check remaining digits for base 8
                	}
            	}
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }
        //CHECKING FOR BASE8 BASE16 or REALNUM HERE!!!
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
        	//SCANS BASE08NUM HERE
        }else{//else check for base08 and prepare the token for return
         if(c == 'x' && isbase8){//((pdigit8 digit8*) + 0) (x) (08)
         	char temp = Peek();
         	if(temp == '0'){
         		input.GetChar(c);
         		temp = Peek();
         		if(temp == '8'){
         			input.GetChar(c);
         			tmp.lexeme += "x08";
         			tmp.token_type = BASE08NUM;
         			tmp.line_no = line_no;
         			return tmp;
         		} else {
         		input.UngetChar(c);
         		tmp.token_type = NUM;
         		tmp.line_no = line_no;
         		return tmp;
         		}
         	} else {
         		if(temp == '1'){
         			input.GetChar(c);
         			temp = Peek();
         			if(temp == '6'){
         				input.GetChar(c);
         				tmp.lexeme += "x16";
         				tmp.token_type = BASE16NUM;
         				tmp.line_no = line_no;
         				return tmp;
         			} else {
         				input.UngetChar(c);
         				tmp.token_type = NUM;
         				tmp.line_no = line_no;
         				return tmp;
         			}
         		}
         		else{
         		input.UngetChar(c);
         		input.UngetChar(c);
         		input.UngetChar(c);
         		tmp.token_type = ID;
         		tmp.line_no = line_no;
         		return tmp;
         		}
         	}//Changelog: Does all the prepping for BASE08 here
         	//TODO: test case 3 does not pass
        	//scanBase08Num();
        } else {
        	input.UngetChar(c);
        	tmp.token_type = NUM;        	
        	tmp.line_no = line_no;
        	return tmp;
        	}
    	}
    } else {//TODO: Add code to check if digit is base 16
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
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
//SNIPPET NOT FUNCTIONAL AND SUPERFLUOUS BASE8 SCANNED WITHIN ScanNumber()
/***
Token LexicalAnalyzer::scanBase08Num(){
	printf("scanning base 8\n");
	char temp = Peek();
	char c;
	if(temp == '0'){
		input.GetChar(c);
		temp = Peek();
		if(temp == '8'){
			input.GetChar(c);
			tmp.lexeme += "x08";
			tmp.token_type = BASE08NUM;
			tmp.line_no = line_no;
			return tmp;
		} else {
			input.UngetChar(c);
			tmp.lexeme = "";
			tmp.token_type = ERROR;
			tmp.line_no = line_no;
		}

	} else {
		input.UngetChar(c);
		tmp.lexeme = "";
		tmp.token_type = ERROR;
		tmp.line_no = line_no;
	}
}**/
Token LexicalAnalyzer::scanBase016Num(){
	//scan all digit16 here?

	//scan x16
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
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
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

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
