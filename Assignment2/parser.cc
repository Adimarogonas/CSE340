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
string currentLable = "::";
bool isPrivate = false;
int curLevel = 0;
vector<declaration> decls;
vector<var> vars;

string reservedW[] = { "END_OF_FILE", "PUBLIC", "PRIVATE",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "ID", "LPAREN", "RPAREN",
    "ERROR"
};
parser::parser(){
}
var::var() {
	id = "";
	scope = "";
	encapsulation = "";
	nestLevel = 0;
}
var::var(std::string name, std::string label, std::string encap, int nLevel){
	id = name;
	scope = label;
	encapsulation = encap;
	nestLevel = nLevel;
}
declaration::declaration(){

}
declaration::declaration(var l, var r){
	left = l;
	right = r;
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
Token parser::expect(TokenType type){
	Token t = lexer.GetToken();
	if(t.token_type != type){
		//cout << "was " << reservedW[t.token_type] << " Expected " << reservedW[type] << " at " << t.line_no << "\n";
		//t.Print();
		syntax_error();
	}
	return t;
}
void parser::parse_program(){//global_vars scope
	parse_global_vars();
	parse_scope();
	printSymbolTable();
}
void parser::parse_global_vars(){//ε OR var_list SEMICOLON
	Token t;
	Token temp;
	t = expect(ID);
	string possibleVar = t.lexeme;//keep this ID lexeme in case it is not the scope;
	temp = peek();
	if(temp.token_type == LBRAC){
		lexer.UngetToken(t);
		return;
	} else {
		lexer.UngetToken(t);
		var newVar(possibleVar, "::", " ", curLevel);
		parse_var_list();
		expect(SEMICOLON);
	}

}
void parser::parse_var_list(){//ID or ID COMMA var_list
	Token t;
	string encap;
	if(isPrivate = true){
		encap = "-";
	} else {
		encap = "+";
	}
	if(currentLable == "::")
		encap = "::";
	t = expect(ID);
	var newVar(t.lexeme, currentLable, encap, curLevel);
	vars.push_back(newVar);
	t = lexer.GetToken();
	if(t.token_type == COMMA)
		parse_var_list();
	else
		lexer.UngetToken(t);
}
void parser::parse_scope(){//ID LBRACE public_vars private_vars stmt_list RBRACE
	//cout << "parsing scope\n";
	Token t = expect(ID);
	currentLable = t.lexeme;
	t = expect(LBRAC);
	curLevel++;
	parse_public_vars();
	parse_private_vars();
	parse_stmt_list();
	t = expect(RBRAC);
	curLevel--;
}
void parser::parse_public_vars(){//TODO: SETUP SYMBOL TABLE HERE!!!!
	Token t = peek();//PUBLIC COLON var_list SEMICOLON
	if(t.token_type == PUBLIC){
		t = lexer.GetToken();
		t = expect(COLON);
		isPrivate = false;
		parse_var_list();
		t = expect(SEMICOLON);
	}else {
		if(t.token_type == PRIVATE){
			parse_private_vars();
		} else {
			if(t.token_type != ID){
				syntax_error();
			} else {
				return;	
			}
		}
	}
}
void parser::parse_private_vars(){
	//cout << "Parsing private\n";
	Token t = peek();//PRIVATE COLON var_list SEMICOLON
	if(t.token_type == PRIVATE){
		t = lexer.GetToken();
		t = expect(COLON);
		isPrivate = false;
		parse_var_list();
		t = expect(SEMICOLON);
	} else {
		if(t.token_type == ID){
			return;
		} else {
			if(t.token_type != RBRAC)
				syntax_error();
		}
	}
	
}
void parser::parse_stmt_list(){//stmt OR stmt stmt_list
	parse_stmt();
	Token t = peek();
	if(t.token_type == ID)
		parse_stmt_list();
} 
void parser::parse_stmt(){//ID EQUAL ID SEMICOLON   OR    scope
	//cout << "Parsing stmt\n";
	Token t = expect(ID);
	string possibleLeft = t.lexeme;
	Token temp = peek();
	if(temp.token_type == LBRAC){
		lexer.UngetToken(t);
		parse_scope();
	} else {
		if(temp.token_type != RBRAC){
			t = expect(EQUAL);
			var lVar(possibleLeft, currentLable, "", curLevel);
			t = expect(ID);
			var rVar(t.lexeme, currentLable, "", curLevel);
			declaration nDecl(lVar,rVar);
			decls.push_back(nDecl);
			t = expect(SEMICOLON);
		}
	}

}
int parser::findMatches(var findVar){//finds number of matches
	int matches = 0;
	for(std::size_t i=0; i<vars.size(); ++i){
    	if(vars[i].id == findVar.id){
    		matches++;
    	}
	}
	return matches;
}
var parser::findVar(var Query){//searches for and returns variable
	for(std::size_t i=0; i<vars.size(); ++i){
    	if(vars[i].id == Query.id){
    		if(!(vars[i].encapsulation == "-" && vars[i].scope != Query.scope))
    			return vars[i];//TODO: DO CHECKING IF SUITABLE FOR VAR HERE THIS WILL SAVE CPU CYCLES
    	}
	}
	Query.scope = "?";
	return Query;
}
void parser::resolveDeclaration(var resolveVar){
		var printVar;
		int matches = findMatches(resolveVar);
		printVar = resolveVar;
		if(matches == 1){
			printVar = findVar(resolveVar);
			if(printVar.nestLevel > resolveVar.nestLevel || (printVar.encapsulation == "-" && printVar.scope != resolveVar.scope)){
				printVar = resolveVar;
				printVar.scope = "?";
			}
		} else {
			if(matches == 0){//TODO: Resolve multiple possible names
				printVar = resolveVar;
				printVar.scope = "?";
			}
		}
		if(printVar.scope != "::")
			cout << printVar.scope << "." << printVar.id;
		else
			cout << printVar.scope << printVar.id;
}
void parser::printSymbolTable(){
	for(std::size_t i=0; i<decls.size(); ++i){
		var leftres = decls[i].left;
		resolveDeclaration(leftres);
		cout << "=";
		var rightres = decls[i].right;
		resolveDeclaration(rightres);
		cout << "\n";
	}
}