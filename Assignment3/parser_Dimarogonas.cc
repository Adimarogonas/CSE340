#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"

using namespace std;
LexicalAnalyzer lexer;
vector<variable> varList;
vector<std::string> declaredNames;
vector<vector<variable>> declarations;
string Reserved[] = { "END_OF_FILE",
    "INT", "REAL", "BOOL", "TRUE", "FALSE", "IF", "WHILE", "SWITCH",
    "CASE", "PUBLIC", "PRIVATE", "NOT", "DOT", "PLUS", "MINUS", "DIV", "MULT", "EQUAL",
    "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "LTEQ", "NOTEQUAL",
     "LESS", "GTEQ", "GREATER", "REALNUM", "NUM", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
};
declaration::declaration(){

}
variable::variable(){

}
variable::variable(string n, string t, int ln, TokenType tType){
	name = n;
	type = t;
	line_no = ln;
	tok_type = tType;
}
parser::parser(){

}
void parser::syntax_error(){
	cout << "Syntax Error";
	exit(1);	
}
Token parser::peek(){
	Token ret = lexer.GetToken();
	lexer.UngetToken(ret);
	return ret;
}
Token parser::expect(TokenType expected){
	Token t = lexer.GetToken();
	if(t.token_type == expected)
		return t;
	else{
		cout << "Expected: " << Reserved[expected] << "\t" << "Recieved:" << Reserved[t.token_type] << endl;
		syntax_error();
	}
}
void parser::parse_program(){//global_vars body
	parse_global_vars();
	parse_body();
	for(int i=0; i<varList.size() - 1; i++){
		resolve_decl(declarations[i]);
	}
	for(int i=0; i<varList.size(); i++){
		if(varList[i].type == "INT")
			cout << varList[i].name << ": " << "int " << "#" << endl;
		if(varList[i].type == "BOOL")
			cout << varList[i].name << ": " << "bool " << "#" << endl;
		if(varList[i].type == "REAL")
			cout << varList[i].name << ": " << "real " << "#" << endl;
	}
}
void parser::parse_global_vars(){//var_decl_list
	Token t = peek();
	if(t.token_type == LBRACE){
		return;
	}
	parse_var_decl_list();
}
void parser::parse_var_decl_list(){//var_decl | var_decl var_decl_list
	parse_var_decl();
	Token t = peek();
	if(t.token_type == ID)
		parse_var_decl();
}
void parser::parse_var_decl(){//var_list COLON type_name SEMICOLON
	parse_var_list();
	expect(COLON);
	Token t = lexer.GetToken();
	if(t.token_type == INT){
		for(auto& it : declaredNames){
			variable newVar(it,"INT", t.line_no, t.token_type);
			varList.push_back(newVar);
		}
	} else {
		if(t.token_type == REAL){
			for(auto& it : declaredNames){
				variable newVar(it,"REAL", t.line_no, t.token_type);
				varList.push_back(newVar);
			}
		} else {
			if(t.token_type == BOOL){
				for(auto& it : declaredNames){
					variable newVar(it,"BOOL", t.line_no, t.token_type);
					varList.push_back(newVar);
				}
			} else {
				syntax_error();
			}
		}
	}
	expect(SEMICOLON);
	declaredNames.clear();
}
void parser::parse_var_list(){//ID | ID COMMA var_list
	Token t = expect(ID);
	declaredNames.push_back(t.lexeme);
	t = peek();
	if(t.token_type == COMMA){
		t = lexer.GetToken();
		parse_var_list();
	}
}
void parser::parse_body(){//LBRACE stmt_list RBRACE
	Token t = expect(LBRACE);
	parse_stmt_list();
	expect(RBRACE);
}
void parser::parse_stmt_list(){//stmt stmt_list
	parse_stmt();
	Token t = peek();
	if(t.token_type == ID | t.token_type == IF | t.token_type == WHILE | t.token_type == SWITCH)
		parse_stmt_list();
	//add recursion and way to detect if token is statement
}
void parser::parse_stmt(){//assignment_stmt | if_stmt | while_statmt | switch_stmt
	Token t = peek();
	if(t.token_type == ID)
		parse_assignment_stmt();
	if(t.token_type == IF)
		parse_if_stmt();
	if(t.token_type == WHILE)
		parse_while_stmt();
	if(t.token_type == SWITCH)
		parse_switch_stmt();

}
void parser::parse_assignment_stmt(){//ID EQUAL expression SEMICOLON
	Token t = expect(ID);
	//Figure out type here
	variable leftVar = variable(t.lexeme, find_Var(t), t.line_no, t.token_type);
	vector<variable> decl;
	decl.push_back(leftVar);
	expect(EQUAL);
	decl = parse_expression(decl);//Add expected LH type to param
	expect(SEMICOLON);
	//TODO: Write code to resolve here
	declarations.push_back(decl);
}
vector<variable> parser::parse_expression(vector<variable> decl){//primary | binary_operator expression expression | unary_operator expression
	//1 = called from LHS
	//2 = called from Unary Operator
	//3 = called from Binary Operator
	//4 = called from an IF, WHILE loop
	//5 = Called from switch
	Token t = peek();
	switch(t.token_type){
		case ID:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, find_Var(t), t.line_no, t.token_type);
			decl.push_back(newSymbol);
			break;
		}
			break;
		case NUM:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "INT", t.line_no, t.token_type);
			decl.push_back(newSymbol);
		}
			break;
		case REALNUM:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "REAL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
		}
			break;
		case TRUE:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
		}
			break;
		case FALSE:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
		}
			break;
		case NOT:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
		}
			break;
		case PLUS:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "OP", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case MINUS:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "OP", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case MULT:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "OP", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case DIV:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "OP", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case GREATER:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case LESS:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case GTEQ:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case LTEQ:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		case EQUAL:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;

		case NOTEQUAL:
		{
			Token t = lexer.GetToken();
			variable newSymbol = variable(t.lexeme, "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
			decl = parse_expression(decl);
			decl = parse_expression(decl);
		}
			break;
		default:
			syntax_error();
			break;
	}
	return decl;
}
void parser::parse_if_stmt(){//IF LPAREN expression RPAREN body
	Token t = expect(IF);
	expect(LPAREN);
	vector<variable> decl;
	variable newSymbol = variable("if", "BOOL", t.line_no, t.token_type);
	decl.push_back(newSymbol);
	decl = parse_expression(decl);
	declarations.push_back(decl);
	expect(RPAREN);
	parse_body();

}
void parser::parse_while_stmt(){//WHILE LPAREN expression RPAREN body
	Token t = expect(WHILE);
	expect(LPAREN);
	vector<variable> decl;
	variable newSymbol = variable("while", "BOOL", t.line_no, t.token_type);
			decl.push_back(newSymbol);
	decl = parse_expression(decl);
	declarations.push_back(decl);
	expect(RPAREN);
	parse_body();

}
void parser::parse_switch_stmt(){//SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE
	Token t = expect(SWITCH);
	expect(LPAREN);
	vector<variable> decl;
	variable newSymbol = variable("switch", "INT", t.line_no, t.token_type);
	decl.push_back(newSymbol);
	decl = parse_expression(decl);
	declarations.push_back(decl);
	expect(RPAREN);
	expect(LBRACE);
	parse_case_list();
	expect(RBRACE);
}
void parser::parse_case_list(){
	parse_case();
	Token t = peek();
	if(t.token_type == CASE)
		parse_case_list();
}
void parser::parse_case(){//Case NUM COLON body
	expect(CASE);
	expect(NUM);
	expect(COLON);
	parse_body();
}
string parser::find_Var(Token t){
	for(auto& it : varList){
		if(t.lexeme == it.name)
			return it.type;
		//cout << t.lexeme << " is not equal to " << it.name << endl;
	}
	return "?";
}
bool parser::in_list(variable v){
	for(auto& it : varList){
		if(v.name == it.name)
			return true;
	}
	return false;
}
void parser::type_mismatch(int miss,int line_no){
	cout << "TYPE MISMATCH " << line_no << " C" << miss << endl;
	exit(1); 
}
void parser::resolve_decl(vector<variable> decl){
	vector<variable> output;
	if(decl[0].tok_type == ID){
		variable LHS = decl[0];
		string RHStype = resolve_rhs(decl,1);//Find the RHS type

		if(LHS.type == "?")//Account for unknown contigency
			LHS.type = RHStype;

		if(RHStype == "?")
			RHStype = LHS.type;

		if(RHStype != LHS.type)
			type_mismatch(1,LHS.line_no);
		bool present = in_list(LHS);
		for(int i=0; i<decl.size(); i++){
			if(decl[i].tok_type == ID){
				if(!in_list(decl[i])){
					variable addVar = variable(decl[i].name, RHStype, LHS.line_no, decl[i].tok_type);
					output.push_back(addVar);
					varList.push_back(addVar);

				}
				for(int j=0; j<varList.size(); j++){
					if((decl[i].name == varList[j].name) && (decl[i].type != varList[j].type)){
						varList[j].type = decl[i].type;
						break;
					}
				}
			}
		}
		if(!present){
			for(int i=0; i<output.size() - 1; i++){
				cout << output[i].name << ", ";
			}
			if(output.back().type == "int")
				cout << output.back().name << ": " << "int" << " #" << endl;
			if(output.back().type == "BOOL")
				cout << output.back().name << ": " << "bool" << " #" << endl;
			if(output.back().type == "REAL")
				cout << output.back().name << ": " << "real" << " #" << endl;
			if(output.back().type == "?")
				cout << output.back().name << ": " << "?" << " #" << endl;
		}
	}

	//Check Switch statement
	if(decl[0].tok_type == SWITCH){
		string SWITCHtype = resolve_rhs(decl,1);
		if(SWITCHtype != "INT")
			type_mismatch(5, decl[0].line_no);
	}

	//Check Bool Operators
	if(decl[0].tok_type == IF || decl[0].tok_type == BOOL || decl[0].tok_type == LTEQ || decl[0].tok_type == GTEQ
		|| decl[0].tok_type == LESS || decl[0].tok_type == GREATER || decl[0].tok_type == EQUAL || decl[0].tok_type == NOTEQUAL){
		
		if(decl[1].type == "INT"){

		}
		string SWITCHtype = resolve_rhs(decl,1);
		if(SWITCHtype != "BOOL")
			type_mismatch(4, decl[0].line_no);
	}
}
string parser::resolve_rhs(vector<variable> decl, int idx){
	for(int i=idx; i<decl.size(); i++){
		if(decl[i].type == "OP"){
			string OpType = resolve_OP(decl, i);
			return OpType;
		}
		if(decl[i].tok_type == NOT){
			if(decl[i+1].type != "BOOL")
				type_mismatch(3,decl[i].line_no);
		}
		if(decl[i].type == "INT")
			return "INT";
		if(decl[i].type == "BOOL")
			return "BOOL";
		if(decl[i].type == "REAL")
			return "REAL";
	}
	return "?";
}
string parser::resolve_OP(vector<variable> decl, int idx){
		if(decl[idx+1].type == "OP")
			decl[idx+1].type = resolve_OP(decl, idx+2);
		if(decl[idx+2].type == "OP")
			decl[idx+2].type = resolve_OP(decl,idx+2);
		if(decl[idx+1].type != decl[idx+2].type && (decl[idx+1].type != "?") && (decl[idx+2].type != "?")){
				if(!(decl[idx+1].type == "OP" || decl[idx+2].type == "OP"))
					type_mismatch(2, decl[idx+1].line_no);
		}
		string ret;
		if(decl[idx+1].type == "?")
			ret = decl[idx+2].type;
		if(decl[idx+2].type == "?")
			ret = decl[idx+1].type;
		return ret;
}