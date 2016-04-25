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
#include <stack>
using namespace std;

#include "mscan.h"

/* FIXME find a better place for this */
#define CHEESE_SIZE_DEF 50

enum OpKind {
	PLUS, MINUS, MULT, DIV, LT, LE, GT, GE, EQ, NE
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

/* suggested in "cs4298-Info/compiler/Symbol Table Information.doc":
struct OperationRecord {
	OpKind operator;
	VarKind resultType;
}

struct OperandRecord {
	VarKind dataType;
	bool isVariable; //maybe use ExprKind instead of bool?
	string variableName;
	bool isIndexed;
	int relativeAddress;
}
*/

struct ExprRec { //information about a constant, variable, or an intermediate (temporary) result
	ExprKind kind; //operand type
	string name; //used when kind is ID_EXPR or TEMP_EXPR
	VarKind var_type; //used when kind is ID_EXPR or TEMP_EXPR
	//used when using a LITERAL_EXPR:
	int ival; //Integer value
	string s_fval; //Float value
	string sval; //String
};

struct SymbolEntry { //Entry used for the symbol table
	string name; //Name of the variable
	VarKind type; //Type of the variable
	int size; //Size of the variable
	int ival; //Integer value
	string s_fval; //String representing a float value
	string sval; //String representing a cheese value
	string label; //Label for the variable - used with cheeses
	int relativeAddress; //TODO: Relative address of the variable
	bool isArray; //TODO: is it an array?
	int arrayLength; //TODO: size of the array
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

	void ProcessVar(ExprRec& e);
	// Declares the identifier in the token buffer and builds a
	// corresponding semantic record e.

	void ProcessLit(ExprRec& e);
	// Converts the literal found in the token buffer into numeric form
	// and builds a corresponding semantic record e.

	void ProcessOp(OpRec& o);
	// Produces an operator descriptor O for the operator in the token
	// buffer.

	void Listen(const ExprRec & InVar);
	// Produces the assembly code for reading a value for InVar.

	void Start();
	// Initializes the compiler.

	void Shout(const ExprRec & outExpr);
	// Produces the assembly code for writing outExpr according to types.

	void DefineVar(ExprRec& var);

	void SetCondition(const ExprRec & e1, const OpRec & op,
			const ExprRec & e2, ExprRec & e);

	void IfThen(const ExprRec & bool_cond);

	void IfElse();

	void IfEnd();

	void LoopBegin();

	void LoopEnd(const ExprRec& bool_cond);

	void WhileTag();

	void WhileBegin(const ExprRec& bool_cond);

	void WhileEnd();

	void Break();

	void ForTag();

	void ForAssign(const ExprRec & target, const ExprRec & source);

	void ForBegin(const ExprRec& bool_cond);

	void ForUpdate();

	void ForEnd();

/* _____________________________________________________________________________
*/

private:

	vector<SymbolEntry> symbolTable;
	stack<string> controlStatementLabels; //See Marty's control statement document (bottom of the last page)
	unsigned int lastControlStatementID = 0;
	unsigned int StrLabelID = 0;
	int  maxTemp;     // max temporary allocated so far; initially 0

	int RetrieveVar(const string & s);
	// Retrieve the variable's (called s) position from the symbol table

	int CalcTableSize();
	// Calculate the size of the symbol table

	void Enter(ExprRec& var);
	// Enters s unconditionally into the symbol table.

	void ExtractExpr(const ExprRec & e, string& s, int offset);
	// Returns an operand representation s for the expression e.

	string ExtractOpFloat(const OpRec & o);
	// Returns a representation for the operator o for floats.

	string ExtractOp(const OpRec& o);
	// Returns a representation for the operator o for integers.

	void Generate(const string & s1, const string & s2, const string & s3);
	// Produces the SAM assembly code for one or two operand instructions.
	// s1 is the opcode; s2 and s3 are operands.

	void GetTemp(ExprRec& var);
	// Creates a temporary variable.

	void IntToAlpha(int val, string& str);
	// Makes a string representation for a positive integer val.

	bool LookUp(const string &s);
	// Returns true if s is in the symbol table; otherwise,
	// false is returned.

	void SemanticError(string msg);
	// Trow a semantic error and stop execution.

	unsigned int NextControlStatementID();
	// Return the next integer that has not been used as a control statement ID
	// See StructuredStatementCodeGeneratation.doc

	void CheckNStoreCondition(const OpRec & op, const ExprRec & result);

	string NewStringLabel();

};

#endif
