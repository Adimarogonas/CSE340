#include <iostream>
#include <cstdlib>
#include <sstream>
#include "parser.h"

using namespace st;
LexicalAnalyzer lexer;
Token token;
void Parser::syntax_error(){
	cout << "SYNTAX ERROR !!!\n";

}