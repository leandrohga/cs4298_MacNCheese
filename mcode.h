/*	____________________________________________________________________________

	           Semantics and Coding Component for the Micro Compiler

	                                 mcode.h

	                              Version 2007

	                           James L. Richards
	                     Last Update: August 28, 2007
	                     Update by M. J. Wolf: January 21,2016

	The routines in this unit are based on those provided in the book
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr.,
	Benjamin Cummings Publishing Co. (1991).

	The target language is SAM assembly language for the MACC2 virtual computer.
	____________________________________________________________________________
*/

#ifndef CODEGEN
#define CODEGEN

#include <string>
#include <vector>
using namespace std;

#include "mscan.h"

enum OpKind {
	PLUS, MINUS, MULT, DIV
};

struct OpRec { // information about an operator
	OpKind op; // operator type
};

enum VarKind {
	BOOL, INT, FLOAT, CHEESE
};

enum ExprKind {
	ID_EXPR, LITERAL_EXPR, TEMP_EXPR
};

struct ExprRec { //information about a constant, variable, or an intermediate (temporary) result
	ExprKind kind; //operand type
	string name; //used when kind is ID_EXPR or TEMP_EXPR
	VarKind var_type; //used when kind is ID_EXPR
	//used when using a LITERAL_EXPR:
	bool bval; //Boolean value
	int ival; //Integer value
	float fval; //Float value
	string sval; //String
};

class CodeGen {
public:

	CodeGen();
	// Initializes the code generator

/* _____________________________________________________________________________
*/

	void Assign(const ExprRec & target, const ExprRec & source);
	// Produces the assembly code for an assignment from Source to Target.

	void Finish();
	// Generates code to finish the program.

	void GenInfix(const ExprRec & e1, const OpRec & op, const ExprRec & e2, ExprRec& e);
	// Produces the assembly code for an infix operation.

	void NewLine();
	// Produces the assembly code for starting a new output line.

	void ProcessId(ExprRec& e);
	// Declares the identifier in the token buffer and builds a
	// corresponding semantic record e.

	void ProcessLit(ExprRec& e);
	// Converts the literal found in the token buffer into numeric form
	// and builds a corresponding semantic record e.

	void ProcessOp(OpRec& o);
	// Produces an operator descriptor O for the operator in the token
	// buffer.

	void ReadId(const ExprRec & InVar);
	// Produces the assembly code for reading a value for InVar.

	void Start();
	// Initializes the compiler.

	void Shout(const ExprRec & outExpr);
	// Produces code for outExpr according to its type.

	void WriteExpr(const ExprRec & OutExpr);
	// Produces the assembly code for writing the value of OutExpr.

	void WriteString(const ExprRec & OutExpr);
	// Produces the assembly code for writing the String.

	void DefineVar(ExprRec& var);

/* _____________________________________________________________________________
*/

private:

	vector<string> symbolTable;

	int  maxTemp;     // max temporary allocated so far; initially 0

	void CheckId(const string & s);
	// Declares s as a new variable and enters it into the symbol table when s
	// is not already in the symbol table.

	void Enter(const string & s);
	// Enters s unconditionally into the symbol table.

	void ExtractExpr(const ExprRec & e, string& s);
	// Returns an operand representation s for the expression e.

	string ExtractOpFloat(const OpRec & o);
	// Returns a representation for the operator o for floats.

	string ExtractOp(const OpRec& o);
	// Returns a representation for the operator o for integers.

	void Generate(const string & s1, const string & s2, const string & s3);
	// Produces the SAM assembly code for one or two operand instructions.
	// s1 is the opcode; s2 and s3 are operands.


	string GetTemp();
	// Creates a temporary variable and returns its name.

	void IntToAlpha(int val, string& str);
	// Makes a string representation for a positive integer val.

	bool LookUp(const string &s);
	// Returns true if s is in the symbol table; otherwise,
	// false is returned.

	void SemanticError(string msg); /* FIXME should this be here? */
	// Trow a semantic error and stop execution.
};

#endif
