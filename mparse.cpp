/*	____________________________________________________________________________

	          Parser Component Implementation for the Micro Compiler

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
#include <fstream>
#include <cstdlib>

using namespace std;

extern ifstream sourceFile;
extern ofstream outFile, listFile;

#include "mscan.h"   // Scanner class definition
#include "mparse.h"  // Parser class definition
#include "mcode.h"   // CodeGen class definition

extern Scanner scan; // global scanner declared in micro.cpp
extern CodeGen code; // global code generator declared in micro.cpp

Parser::Parser() {
	tokenAvailable = false;  // no token initially available to parser
}

void Parser::Match(Token t) {
	if (t != NextToken()) {
		SyntaxError(t);
	} else {
		tokenAvailable = false;
    }
}

Token Parser::NextToken() {
	if (!tokenAvailable) {
		savedToken = scan.GetNextToken();
		tokenAvailable = true;
	}
	return savedToken;
}

void Parser::SyntaxError(Token t) {
	cout << " *** Syntax Error Detected " << int(t) << ' ' << int(savedToken);
	listFile << " *** Syntax Error Detected " << int(t) << ' ' << int(savedToken);
	sourceFile.close();
	listFile.close();
	outFile.close();
	cin.ignore(256, '\n');
	cin.get();
	exit(1); // error termination
}

void Parser::SystemGoal() {
	ProgramProc();
	Match(EOF_SYM);
	code.Finish();  /*** CODE ***/
}

void Parser::ProgramProc() {
	code.Start();  /*** CODE ***/
	StatementList();
}

void Parser::StatementList() {
	Statement();
	for (;;) { // loop exited by return
		switch (NextToken()) {
		case ID:
		case LISTEN_SYM:
		case SHOUT_SYM:
			Statement();
			break;
		default:
			return;
		}
	}
}

void Parser::Statement() {
	ExprRec identifier, expr;
	switch (NextToken()) {
	case ID:
		Ident(identifier);
		Match(ASSIGN_OP);
		Expression(expr);
		code.Assign(identifier, expr);  /*** CODE ***/
		Match(SEMICOLON);
		break;
	case LISTEN_SYM:
		Match(LISTEN_SYM);
		Match(LBANANA);
		IdList();
		Match(RBANANA);
		Match(SEMICOLON);
		break;
	case SHOUT_SYM:
		Match(SHOUT_SYM);
		Match(LBANANA);
		ExprList();
		Match(RBANANA);
		Match(SEMICOLON);
		code.NewLine();  /*** CODE ***/
		break;
	default:
		SyntaxError(NextToken());
	}
}

void Parser::IdList() {
	ExprRec identifier;
	Ident(identifier);
	code.ReadId(identifier);  /*** CODE ***/
	for (;;) { // loop exited by return
		if (NextToken() == COMMA) {
			Match(COMMA);
			Ident(identifier);
			code.ReadId(identifier);  /*** CODE ***/
		} else {
			return; // exit the function
        }
	}
}

void Parser::ExprItem() {
	//cerr << "ExprItem Check" << endl;
	//cerr << expr.kind << endl;
	////cerr << expr.val << endl;
	//cerr << expr.name << endl;
	//cerr << "ExprItem Check done" << endl;
	switch(NextToken()){
	case CHEESE_LIT: // it is an string literal
	    Match(CHEESE_LIT);
		code.WriteString();  /*** CODE ***/
		break;
	default:
		ExprRec expr;
		Expression(expr);
		code.WriteExpr(expr);  /*** CODE ***/
		for (;;) {  // loop exited by return
			if (NextToken() == COMMA) {
				Match(COMMA);
				Expression(expr);
				code.WriteExpr(expr);  /*** CODE ***/
			} else {
				return; // exit loop
            }
		}
		break;
	}
}
void Parser::ExprList() {
	ExprItem();
	// code.WriteExpr(expr);  /*** CODE ***/
	for (;;) {  // loop exited by return
		if (NextToken() == COMMA) {
			Match(COMMA);
			ExprItem();
	        //code.WriteExpr(expr);  /*** CODE ***/
        } else {
			return; // exit loop
        }
	}
}

void Parser::Expression(ExprRec& result) {
	ExprRec leftOperand, rightOperand;
	OpRec op;
	Primary(result);
	for (;;) {  // loop exited by return
		if (NextToken() == PLUS_OP || NextToken() == MINUS_OP) {
			// from call to Primary of gen_infix
			leftOperand.kind = result.kind;
			leftOperand.val = result.val;
			leftOperand.name= result.name;
			AddOp(op);
			Primary(rightOperand);
            /*** CODE ***/
			code.GenInfix (leftOperand, op, rightOperand, result);
		} else {
			return; // exit loop
        }
	}
}

void Parser::Primary(ExprRec& result) {
	switch (NextToken()) {
	case LBANANA:
		Match(LBANANA);
		Expression(result);
		Match(RBANANA);
		break;
	case ID:
		Match(ID);
		code.ProcessId(result);  /*** CODE ***/
		code.ProcessId(result);  /*** CODE ***/
		break;
	case INT_LIT:
		Match(INT_LIT);
		code.ProcessLiteral(result);  /*** CODE ***/
		break;
	case CHEESE_LIT:
		Match(CHEESE_LIT);
		code.ProcessLiteral(result);  /*** CODE ***/
		break;
	default:
		SyntaxError(NextToken());
	}
}

void Parser::AddOp(OpRec& op) {
	switch (NextToken()) {
	case PLUS_OP:
		Match(PLUS_OP);
		code.ProcessOp(op);  /*** CODE ***/
		break;
	case MINUS_OP:
		Match(MINUS_OP);
		code.ProcessOp(op);  /*** CODE ***/
		break;
	default:
		SyntaxError(NextToken());
	}
}

void Parser::Ident(ExprRec& result) {
	Match(ID);
	code.ProcessId(result);  /*** CODE ***/
}
