/*	____________________________________________________________________________

	          Parser Component Implementation for the MaccNCheese Compiler
			SHELL GENERATED BY PARSERGEN VERSION 5.0.3
	                               mparse.cpp

	                              Version 2007

	                           James L. Richards
	                     Last Update: August 28, 2007
			     Updated by Marty J. Wolf
			       January 4, 2016

	The routines in this unit are based on those provided in the book
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr.,
	Benjamin Cummings Publishing Co. (1991).

	See Section 2.3-2.4, pp. 31-40.
	____________________________________________________________________________
*/

#include <iostream>
#include <string>
using namespace std;

#include "mscan.h"	// Scanner class definition
#include "mparse.h"	// Parser class definition
#include "mcode.h"	// CodeGen class definition

extern Scanner scan; // global scanner declared in micro.cpp
extern CodeGen code; // global code generator declared in micro.cpp

Parser::Parser() {
	tokenAvailable = false; // no token initially available to parser
}

void Parser::SyntaxError(Token t, string msg) {
	cout << "Syntax Error: " + msg << endl;
	exit(1); // abort on any syntax error
}

Token Parser::NextToken() {
	if (!tokenAvailable) {
		savedToken = scan.GetNextToken();
		tokenAvailable = true;
	}
	return savedToken;
}

void Parser::Match(Token t) {
	if (t != NextToken())
		SyntaxError(t, "");
	else
		tokenAvailable = false;
}


void Parser::InitTail() {
	switch (NextToken()) {
	case COMMA:
		Match(COMMA);
//		Expression();
		InitTail();
		break;
	case RMUSTACHE:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarDecTail(ExprRec& var) {
	switch (NextToken()) {
	case COMMA:
		Match(COMMA);
		Match(ID);
		code.DefineVar(var);
		VarDecTail(var);
		break;
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarDecList(ExprRec& var) {
	Match(ID);
	code.DefineVar(var); /*** CODE ***/
	VarDecTail(var);
}

void Parser::DecTail() {
	switch (NextToken()) {
	case BOOL_SYM:
	case CHEESE_SYM:
	case FLOAT_SYM:
	case HIPHIP_SYM:
	case INT_SYM:
		Declaration();
		DecTail();
		break;
	case RMUSTACHE:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Declaration() {
	ExprRec var;
	switch (NextToken()) {
	case BOOL_SYM:
	case CHEESE_SYM:
	case FLOAT_SYM:
	case INT_SYM:
		Type(var);
		Match(COLON);
		VarDecList(var);
		Match(SEMICOLON);
		break;
	case HIPHIP_SYM:
		Match(HIPHIP_SYM);
		Match(LSTAPLE);
		Match(INT_LIT);
		Match(RSTAPLE);
//		Type();
//		VarDecList();
		Match(SEMICOLON);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::DecList() {
	Declaration();
	DecTail();
}

void Parser::BoolLit(ExprRec& var) {
	switch (NextToken()) {
	case FALSE_SYM:
		Match(FALSE_SYM);
		var.var_type = BOOL;
		var.bval = "false";
		break;
	case TRUE_SYM:
		Match(TRUE_SYM);
		var.var_type = BOOL;
		var.bval = "true";
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::CheeseTypeTail() {
	switch (NextToken()) {
	case LSTAPLE:
		Match(LSTAPLE);
		Match(INT_LIT);
		Match(RSTAPLE);
		break;
	case COLON:
	case ID:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::CheeseType() {
	Match(CHEESE_SYM);
	CheeseTypeTail();
}

void Parser::Type(ExprRec& var) {
	switch (NextToken()) {
	case BOOL_SYM:
		Match(BOOL_SYM);
		var.var_type = BOOL;
		break;
	case INT_SYM:
		Match(INT_SYM);
		var.var_type = INT;
		break;
	case FLOAT_SYM:
		Match(FLOAT_SYM);
		var.var_type = FLOAT;
		break;
	case CHEESE_SYM:
		CheeseType();
		var.var_type = CHEESE;
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Literal(ExprRec & var) {
	switch (NextToken()) {
	case FALSE_SYM:
	case TRUE_SYM:
		BoolLit(var);
		break;
	case INT_LIT:
		Match(INT_LIT);
		break;
	case FLOAT_LIT:
		Match(FLOAT_LIT);
		break;
	case CHEESE_LIT:
		Match(CHEESE_LIT);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::MultOp() {
	switch (NextToken()) {
	case MULT_OP:
		Match(MULT_OP);
		break;
	case DIV_OP:
		Match(DIV_OP);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::FactorTail(ExprRec& result) {
	ExprRec leftOperand, rightOperand;
	OpRec op;
	switch (NextToken()) {
	case MULT_OP:
	case DIV_OP:
		leftOperand.kind = result.kind;
		leftOperand.ival = result.ival; /* FIXME TODO check other types */
		leftOperand.name = result.name;
		MultOp();
		code.ProcessOp(op); /*** CODE ***/
		Primary(rightOperand);
		 /*** CODE ***/
		code.GenInfix(leftOperand, op, rightOperand, result);
		FactorTail(result);
		break;
	case PLUS_OP:
	case MINUS_OP:
		leftOperand.kind = result.kind;
		leftOperand.ival = result.ival; /* FIXME TODO check other types */
		leftOperand.name = result.name;
		AddOp();
		code.ProcessOp(op);
		Primary(rightOperand);
		/*** CODE ***/
		code.GenInfix(leftOperand, op, rightOperand, result);
		FactorTail(result);
		break;
	case RSTAPLE:
	case RBANANA:
	case RMUSTACHE:
	case SEMICOLON:
	case COMMA:
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP1:
	case EQ_OP2:
	case NE_OP:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Primary(ExprRec& result) {
	switch (NextToken()) {
	case FALSE_SYM:
	case TRUE_SYM:
		Literal(result);
		result.var_type = BOOL;
		code.ProcessLit(result); /*** CODE ***/
		break;
	case INT_LIT:
		Literal(result);
		result.var_type = INT;
		code.ProcessLit(result); /*** CODE ***/
		break;
	case FLOAT_LIT:
		Literal(result);
		result.var_type = FLOAT;
		code.ProcessLit(result); /*** CODE ***/
		break;
	case CHEESE_LIT:
		Literal(result);
		result.var_type = CHEESE;
		code.ProcessLit(result); /*** CODE ***/
		break;
	case ID:
		Variable();
		//code.ProcessVar(); /*** CODE ***/
		break;
	case LBANANA:
		Match(LBANANA);
		Expression(result);
		Match(RBANANA);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::AddOp() {
	switch (NextToken()) {
	case PLUS_OP:
		Match(PLUS_OP);
		break;
	case MINUS_OP:
		Match(MINUS_OP);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::ExprTail(ExprRec& result) {
	ExprRec leftOperand, rightOperand;
	OpRec op;
	switch (NextToken()) {
	case PLUS_OP:
	case MINUS_OP:
		leftOperand.kind = result.kind;
		leftOperand.ival = result.ival; /* FIXME TODO check other types */
		leftOperand.name = result.name;
		AddOp();
		code.ProcessOp(op); /*** CODE ***/
		Factor(rightOperand);
		 /*** CODE ***/
		code.GenInfix(leftOperand, op, rightOperand, result);
		ExprTail(result);
		break;
	case RSTAPLE:
	case RBANANA:
	case RMUSTACHE:
	case SEMICOLON:
	case COMMA:
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP1:
	case EQ_OP2:
	case NE_OP:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Factor(ExprRec& result) {
	Primary(result);
	FactorTail(result);
}

void Parser::RelOp() {
	switch (NextToken()) {
	case LT_OP:
		Match(LT_OP);
		break;
	case LE_OP:
		Match(LE_OP);
		break;
	case GT_OP:
		Match(GT_OP);
		break;
	case GE_OP:
		Match(GE_OP);
		break;
	case EQ_OP1:
		Match(EQ_OP1);
		break;
	case EQ_OP2:
		Match(EQ_OP2);
		break;
	case NE_OP:
		Match(NE_OP);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::CondTail() {
	switch (NextToken()) {
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP1:
	case EQ_OP2:
	case NE_OP:
		RelOp();
		// code.ProcessOp();
//		Expression();
		break;
	case RBANANA:
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::IntListTail() {
	switch (NextToken()) {
	case COMMA:
		Match(COMMA);
		Match(INT_LIT);
		// code.IntAppend();
		IntListTail();
		break;
	case COLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::IntList() {
	Match(INT_LIT);
	// code.IntAppend();
	IntListTail();
}

void Parser::CaseListTail() {
	switch (NextToken()) {
	case CASE_SYM:
		Case();
		CaseListTail();
		break;
	case OTHERWISE_SYM:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Case() {
	Match(CASE_SYM);
	IntList();
	Match(COLON);
	StmtList();
	// code.CaseEnd();
}

void Parser::CaseList() {
	Case();
	CaseListTail();
}

void Parser::ForAssign() {
	Variable();
	// code.ProcessVar();
	Match(ASSIGN_OP);
//	Expression();
	// code.ForAssign();
}

void Parser::ElseClause() {
	switch (NextToken()) {
	case ELSE_SYM:
		Match(ELSE_SYM);
		// code.IfElse();
		StmtList();
		break;
	case END_SYM:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Condition() {
//	Expression();
	CondTail();
	// code.SetCondition();
}

void Parser::VarDecs() {
	Match(DECS_SYM);
	Match(LMUSTACHE);
	DecList();
	Match(RMUSTACHE);
}

void Parser::SelectStmt() {
	Match(SELECT_SYM);
	Match(LBANANA);
//	Expression();
	Match(RBANANA);
	// code.SelectBegin();
	CaseList();
	Match(OTHERWISE_SYM);
	// code.Otherwise();
	Match(COLON);
	StmtList();
	// code.CaseEnd();
	Match(END_SYM);
	// code.SelectEnd();
}

void Parser::ForStmt() {
	Match(FOR_SYM);
	Match(LBANANA);
	ForAssign();
	Match(SEMICOLON);
	Condition();
	// code.ForBegin();
	Match(SEMICOLON);
	ForAssign();
	// code.ForUpdate();
	Match(RBANANA);
	StmtList();
	Match(END_SYM);
	// code.ForEnd();
}

void Parser::WhileStmt() {
	Match(WHILE_SYM);
	Match(LBANANA);
	Condition();
	Match(RBANANA);
	// code.WhileBegin();
	StmtList();
	Match(END_SYM);
	// code.WhileEnd();
}

void Parser::LoopStmt() {
	Match(DO_SYM);
	// code.LoopBegin();
	StmtList();
	Match(UNTIL_SYM);
	Match(LBANANA);
	Condition();
	Match(RBANANA);
	// code.LoopEnd();
	Match(SEMICOLON);
}

void Parser::IfStmt() {
	Match(IF_SYM);
	Match(LBANANA);
	Condition();
	Match(RBANANA);
	// code.IfThen();
	StmtList();
	ElseClause();
	Match(END_SYM);
	// code.IfEnd();
}

void Parser::ItemListTail(ExprRec& expr) {
	switch (NextToken()) {
	case COMMA:
		Match(COMMA);
		Expression(expr);
		code.Shout(expr); /*** CODE ***/
		ItemListTail(expr);
		break;
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::ItemList() {
	ExprRec expr;
	Expression(expr);
	code.Shout(expr); /*** CODE ***/
	ItemListTail(expr);
}

void Parser::VariableTail() {
	switch (NextToken()) {
	case LSTAPLE:
		Match(LSTAPLE);
//		Expression();
		Match(RSTAPLE);
		break;
	case RSTAPLE:
	case RBANANA:
	case RMUSTACHE:
	case SEMICOLON:
	case COMMA:
	case ASSIGN_OP:
	case PLUS_OP:
	case MINUS_OP:
	case MULT_OP:
	case DIV_OP:
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP1:
	case EQ_OP2:
	case NE_OP:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarListTail() {
	switch (NextToken()) {
	case COMMA:
		Match(COMMA);
		Variable();
		// code.ProcessVar();
		// code.Listen();
		VarListTail();
		break;
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarList() {
	Variable();
	// code.ProcessVar();
	// code.Listen();
	VarListTail();
}

void Parser::InitList() {
//	Expression();
	InitTail();
}

void Parser::Expression(ExprRec& result) {
	Factor(result);
	ExprTail(result);
}

void Parser::AssignTail() {
	switch (NextToken()) {
	case FALSE_SYM:
	case TRUE_SYM:
	case LBANANA:
	case ID:
	case INT_LIT:
	case FLOAT_LIT:
	case CHEESE_LIT:
//		Expression();
		break;
	case LMUSTACHE:
		Match(LMUSTACHE);
		InitList();
		Match(RMUSTACHE);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Variable() {
	Match(ID);
	VariableTail();
}

void Parser::BreakStmt() {
	Match(BREAK_SYM);
	// code.Break();
	Match(SEMICOLON);
}

void Parser::ShoutStmt() {
	Match(SHOUT_SYM);
	ItemList();
	code.NewLine(); /*** CODE ***/
	Match(SEMICOLON);
}

void Parser::ListenStmt() {
	Match(LISTEN_SYM);
	VarList();
	Match(SEMICOLON);
}

void Parser::AssignStmt() {
	Variable();
	// code.ProcessVar();
	Match(ASSIGN_OP);
	AssignTail();
	// code.Assign();
	Match(SEMICOLON);
}

void Parser::StructStmt() {
	switch (NextToken()) {
	case IF_SYM:
		IfStmt();
		break;
	case DO_SYM:
		LoopStmt();
		break;
	case WHILE_SYM:
		WhileStmt();
		break;
	case FOR_SYM:
		ForStmt();
		break;
	case SELECT_SYM:
		SelectStmt();
		break;
	case DECS_SYM:
		VarDecs();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::SimpleStmt() {
	switch (NextToken()) {
	case ID:
		AssignStmt();
		break;
	case LISTEN_SYM:
		ListenStmt();
		break;
	case SHOUT_SYM:
		ShoutStmt();
		break;
	case BREAK_SYM:
		BreakStmt();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::StmtTail() {
	switch (NextToken()) {
	case BREAK_SYM:
	case DECS_SYM:
	case DO_SYM:
	case FOR_SYM:
	case IF_SYM:
	case LISTEN_SYM:
	case SELECT_SYM:
	case SHOUT_SYM:
	case WHILE_SYM:
	case ID:
		Statement();
		StmtTail();
		break;
	case EOF_SYM:
	case CASE_SYM:
	case ELSE_SYM:
	case END_SYM:
	case OTHERWISE_SYM:
	case UNTIL_SYM:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Statement() {
	switch (NextToken()) {
	case BREAK_SYM:
	case LISTEN_SYM:
	case SHOUT_SYM:
	case ID:
		SimpleStmt();
		break;
	case DECS_SYM:
	case DO_SYM:
	case FOR_SYM:
	case IF_SYM:
	case SELECT_SYM:
	case WHILE_SYM:
		StructStmt();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::StmtList() {
	Statement();
	StmtTail();
}

void Parser::Program() {
	code.Start(); /*** CODE ***/
	StmtList();
	code.Finish(); /*** CODE ***/
}

void Parser::SystemGoal() {
	Program();
	Match(EOF_SYM);
}
