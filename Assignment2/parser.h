#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <iostream>
#include "lexer.h"
#include "inputbuf.h"
class var {
	public:
		std::string id, scope, encapsulation;
		int nestLevel;
		var();
		var(std::string name, std::string label, std::string encap, int nLevel);
};
class declaration {
	public:
		declaration();
		declaration(var l, var r);	
		var left,right;
	private:
};
class parser{
	public:
		parser();
		void syntax_error();
		Token peek(); 
		Token expect(TokenType type);
		void parse_program();
	private:
		void parse_global_vars();
		void parse_var_list();
		void parse_scope();
		void parse_public_vars();
		void parse_private_vars();
		void parse_stmt_list();
		void parse_stmt();
		int findMatches(var findVar);
		var findVar(var query);
		void resolveDeclaration(var resolveVar);
		void printSymbolTable();
		void printVarList();
};
#endif //__PARSER_H__