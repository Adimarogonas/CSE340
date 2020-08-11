#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <iostream>
#include "lexer.h"
#include "inputbuf.h"
class variable{
	public:
		variable();
		variable(std::string n, std::string t, int ln, TokenType tType);
		std::string name;
		std::string type;
		int line_no;
		TokenType tok_type;
};
class declaration{
	public:
		declaration();
		std::vector<variable> symbolList;
};
class parser{
	public:
		parser();
		void parse_program();
		Token peek();
		Token expect(TokenType expected);
	private:
		void syntax_error();
		void parse_global_vars();
		void parse_decl_list();
		void parse_var_decl_list();
		void parse_var_decl();
		void parse_var_list();
		void parse_body();
		void parse_stmt_list();
		void parse_stmt();
		void parse_assignment_stmt();
		std::vector<variable> parse_expression(std::vector<variable> decl);
		void parse_if_stmt();
		void parse_while_stmt();
		void parse_switch_stmt();
		void parse_case_list();
		void parse_case();
		std::string find_Var(Token t);
		void change_Var(variable v);
		bool in_list(variable v);
		void type_mismatch(int miss, int line_no);
		void resolve_decl(std::vector<variable> decl);
		std::string resolve_rhs(std::vector<variable> decl, int idx);
		std::string resolve_OP(std::vector<variable> decl, int idx);
		void fix_varList(std::vector<variable> decl);
		void output2();
};
#endif //__PARSER_H__