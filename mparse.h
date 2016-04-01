/*	____________________________________________________________________________

	                Parser Component for the MaccNCheese Compiler
			SHELL GENERATED BY PARSERGEN VERSION 5.0.3

	                                mparse.h

	                              Version 2007

	                           James L. Richards
	                     Last Update: August 28, 2007

	The routines in this unit are based on those provided in the book
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr.,
	Benjamin Cummings Publishing Co. (1991).

	See Section 2.3-2.4, pp. 31-40.
	____________________________________________________________________________
*/

#ifndef PARSER
#define PARSER

#include <string>
using namespace std;

#include "mscan.h"
#include "mcode.h"

class Parser
{
public:

	Parser();
	// Initializes the parser object.

	void SystemGoal();
	// Checks <system goal> syntax and semantics.

private:

	Token savedToken;
	bool  tokenAvailable;

	void SyntaxError(Token t, string msg);
	// Processes a syntax error and aborts compilation.

	Token NextToken();
	// Gets the next token and saves it.

	void Match(Token t);
	// Returns successfully if the next token matches t; otherwise a syntax
	// error occurs and compilation is aborted.

	void InitTail();

	void VarDecTail(ExprRec& var);

	void VarDecList(ExprRec& var);

	void DecTail();

	void Declaration();

	void DecList();

	void BoolLit();

	void CheeseTypeTail();

	void CheeseType();

	void Type(ExprRec& var);

	void Literal();

	void MultOp();

	void FactorTail(ExprRec& result);

	void Primary(ExprRec& result);

	void AddOp();

	void ExprTail(ExprRec& result);

	void Factor(ExprRec& result);

	void RelOp();

	void CondTail();

	void IntListTail();

	void IntList();

	void CaseListTail();

	void Case();

	void CaseList();

	void ForAssign();

	void ElseClause();

	void Condition();

	void VarDecs();

	void SelectStmt();

	void ForStmt();

	void WhileStmt();

	void LoopStmt();

	void IfStmt();

	void ItemListTail(ExprRec& expr);

	void ItemList();

	void VariableTail();

	void VarListTail();

	void VarList();

	void InitList();

	void Expression(ExprRec& result);

	void AssignTail();

	void Variable();

	void BreakStmt();

	void ShoutStmt();

	void ListenStmt();

	void AssignStmt();

	void StructStmt();

	void SimpleStmt();

	void StmtTail();

	void Statement();

	void StmtList();

	void Program();

}; // end of Parser class

#endif
