/*	____________________________________________________________________________

	    Semantics and Coding Component Implementation for the Micro Compiler

	                               mcode.cpp

	                              Version 2007

	                           James L. Richards
	                     Last Update: August 28, 2007
	                     Update by M. J. Wolf: January 21,2016

	The routines in this unit are based on those provided in the book
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr.,
	Benjamin Cummings Publishing Co. (1991).

	See Section 2.3-2.4, pp. 31-40.
	____________________________________________________________________________
*/

#include <iostream>
#include <fstream>
using namespace std;

extern ifstream sourceFile;
extern ofstream outFile, listFile;

#include "mscan.h"   // Scanner class definition
#include "mcode.h"   // CodeGen class definition

extern Scanner scan; // global Scanner object declared in micro.cpp

// *******************
// **  Constructor  **
// *******************

CodeGen::CodeGen() {
	maxTemp = 0;
}

// *******************************
// ** Private Member Functions  **
// *******************************

void CodeGen::Enter(ExprRec& var) {
	/* Create the key and fill it */
	symbol_node_t variable;
	variable.name = var.name;
	variable.type = var.var_type;
	/* Check variable size */
	switch (var.var_type) {
	case BOOL:
		variable.size = 2; /* all operations are 16 bits */
		variable.ival = 0; /* init with boolean "False" value */
		break;
	case INT:
		variable.size = 2; /* 2x8 = 16 bits */
		variable.ival = 0; /* init with integer 0 value */
		break;
	case FLOAT:
		variable.size = 4; /* 4x8 = 32 bits */
		if (var.s_fval.size() > 0) {
			variable.s_fval = var.s_fval; /* init with float 0 value */
		} else {
			variable.s_fval = "0.0"; /* init with float 0 value */
		}
		break;
	default:
		/* TODO: check what to do. Check for cheese? */
		break;
	}
	/* Add the record to the symbol table */
	symbolTable.push_back(variable);
}

void CodeGen::ExtractExpr(const ExprRec & e, string& s, int offset) {
	string t;
	int k, n;

	switch (e.kind) {
	case ID_EXPR:
	case TEMP_EXPR:  // operand form: +k(R15)
		s = e.name;
		k = n = 0;
		while (symbolTable[n].name != s) {
			k += symbolTable[n].size;
			n++;
		}
		k = k + offset; /* add offset bytes to k */
		IntToAlpha(k, t);
		s = "+" + t + "(R15)";
		break;
	case LITERAL_EXPR:
		switch (e.var_type) {
		case BOOL:
			IntToAlpha(e.ival, t);
			s = "#" + t;
			break;
		case INT:
			IntToAlpha(e.ival, t);
			s = "#" + t;
			break;
		case FLOAT:
			/* Float operations don't allow immediate
			 * addressing. They are treated as TEMP_EXPR.
			 */
			// operand form: +k(R15)
			s = e.name;
			k = n = 0;
			while (symbolTable[n].name != s) {
				k += symbolTable[n].size;
				n++;
			}
			k = k + offset; /* add offset bytes to k */
			IntToAlpha(k, t);
			s = "+" + t + "(R15)";
			break;
		default:
			/* TODO: check what to do. Check for cheese? */
			break;
		}
	}
}

string CodeGen::ExtractOp(const OpRec & o) {
	if (o.op == PLUS) {
		return "IA        ";
	} else if (o.op == MINUS) {
		return "IS        ";
	} else if (o.op == MULT) {
		return "IM        ";
	} else {
		return "ID        ";
	}
}

string CodeGen::ExtractOpFloat(const OpRec & o) {
	if (o.op == PLUS) {
		return "FA        ";
	} else if (o.op == MINUS) {
		return "FS        ";
	} else if (o.op == MULT) {
		return "FM        ";
	} else {
		return "FD        ";
	}
}

void CodeGen::Generate(const string & s1, const string & s2, const string & s3) {
	listFile.width(20);
	listFile << ' ' << s1;
	outFile << s1;
	if (s2.length() > 0) {
		listFile << s2;
		outFile << s2;
		if (s3.length() > 0) {
			listFile << ',' << s3;
			outFile << ',' << s3;
		}
	}
	listFile << endl;
	outFile << endl;
}

void CodeGen::GetTemp(ExprRec& var) {
	string s;
	static string t;

	t = "Temp&";
	IntToAlpha(++maxTemp, s);
	t += s;
	var.name = t;
	if (!LookUp(var.name)) { // variable not declared yet
		Enter(var);
	} else {
		SemanticError("temporary variable " + var.name + \
				" was already declared before.");
	}
}

void CodeGen::IntToAlpha(int val, string& str) {
	int k;
	char temp;

	str = "";
	if (val == 0) str = "0";
	while (val > 0) {
		str.append(1, (char)(val % 10 + (int)'0'));
		val /= 10;
	}
	k = int(str.length());
	for (int i = 0; i < k/2; i++) {
		temp = str[i];
		str[i] = str[k-i-1];
		str[k-i-1] = temp;
	}
}

bool CodeGen::LookUp(const string & s) {
	for (unsigned i = 0; i < symbolTable.size(); i++)
	if (symbolTable[i].name == s) {
		return true;
	}
	return false;
}

// ******************************
// ** Public Member Functions  **
// ******************************

void CodeGen::Assign(const ExprRec & target, const ExprRec & source) {
	string s;

	switch (target.var_type) {
	case BOOL:
	case INT:
		ExtractExpr(source, s, 0);
		Generate("LD        ", "R0", s);
		ExtractExpr(target, s, 0);
		Generate("STO       ", "R0", s);
		break;
	case FLOAT:
		/* Load the 32 bist into registers R0:R1*/
		ExtractExpr(source, s, 0);
		Generate("LD        ", "R0", s);
		ExtractExpr(source, s, 2);
		Generate("LD        ", "R1", s);
		/* Store registers R0:R1 in the memory */
		ExtractExpr(target, s, 0);
		Generate("STO       ", "R0", s);
		ExtractExpr(target, s, 2);
		Generate("STO       ", "R1", s);
		break;
	default:
		/* TODO: check for cheeses? */
		break;
	}
}
vector<string> str_vect;
int str_cnt = 0;

int CodeGen::CalcTableSize() {
	unsigned int i, index = 0;
	for (i = 0; i < symbolTable.size(); i++) {
		index += symbolTable[i].size;
	}
	return index;
}

void CodeGen::Finish() {
	string s;
	unsigned int i;

	listFile.width(6);
	listFile << ++scan.lineNumber << "  " << scan.lineBuffer << endl;
	Generate("HALT      ", "", "");
	/* Integers, floats and bools */
	Generate("LABEL     ", "VARS", "");
	for (i = 0; i < symbolTable.size(); i++) {
		switch (symbolTable[i].type) {
		case BOOL:
		case INT:
			IntToAlpha(symbolTable[i].ival, s);
			Generate("INT       ", s, "");
			break;
		case FLOAT:
			s = symbolTable[i].s_fval;
			Generate("REAL      ", s, "");
			break;
		default:
			/* TODO: check what to do. Check for cheese? */
			break;
		}
	}
	/* Strings */
	Generate("LABEL     ", "STRS", "");
	while (!str_vect.empty()) {
		s = str_vect.front();
		str_vect.erase(str_vect.begin());
		Generate("STRING    ", s, "");
	}
	/* Boolean strings "False" and "True" */
	Generate("LABEL     ", "BOOL", "");
	Generate("STRING    ", "\"False\"", "");
	Generate("STRING    ", "\"True\"", "");
	outFile.close();
	listFile << endl << endl;
	listFile << " _____________________________________________\n";
	listFile << " <><><><>   S Y M B O L   T A B L E   <><><><>\n"
		<< endl;
	listFile << " Relative" << endl;
	listFile << " Address      Identifier" << endl;
	listFile << " --------     --------------------------------"
		<< endl;
	unsigned int index = 0;
	for (i = 0; i < symbolTable.size(); i++) {
		listFile.width(7);
		listFile << index << "       " << symbolTable[i].name
			<< endl;
		index += symbolTable[i].size;
	}
	listFile << " _____________________________________________"
		<< endl;
	listFile << endl;
	listFile << " Normal successful compilation." << endl;
	listFile.close();
}

void CodeGen::GenInfix(const ExprRec & e1, const OpRec & op, const ExprRec & e2, ExprRec& e) {

	if (e1.var_type != e2.var_type) {
		SemanticError("mixed-mode arithmetic operations"
				" are not allowed.");
	} else if ((e1.var_type != INT) && (e1.var_type != FLOAT)) {
		SemanticError("arithmetic opertions are allowed only for"
				" INTs and FLOATs.");
	}
	/* Result type = operands types */
	e.var_type = e1.var_type;

	/* Literals */
	if ((e1.kind == LITERAL_EXPR && e2.kind == LITERAL_EXPR)
			&& (e.var_type != FLOAT)) {
		/* FLOAT literal expressions are calculated at runtime */
		e.kind = LITERAL_EXPR;
		switch (op.op) {
		case PLUS:
			e.ival = e1.ival + e2.ival;
			break;
		case MINUS:
			e.ival = e1.ival - e2.ival;
			break;
		case MULT:
			e.ival = e1.ival * e2.ival;
			break;
		case DIV:
			e.ival = e1.ival / e2.ival;
			break;
		}
	} else { /* Variables */
		string opnd;
		GetTemp(e);
		if (e.var_type == INT) {
			e.kind = TEMP_EXPR;
			ExtractExpr(e1, opnd, 0);
			Generate("LD        ", "R0", opnd);
			ExtractExpr(e2, opnd, 0);
			Generate(ExtractOp(op), "R0", opnd);
			ExtractExpr(e, opnd, 0);
			Generate("STO       ", "R0", opnd);
		} else { /* FLOAT */
			/* Load the 32 bist into registers R0:R1*/
			ExtractExpr(e1, opnd, 0);
			Generate("LD        ", "R0", opnd);
			ExtractExpr(e1, opnd, 2);
			Generate("LD        ", "R1", opnd);
			/* 32 bits arithmetic operation */
			ExtractExpr(e2, opnd, 0);
			Generate(ExtractOpFloat(op), "R0", opnd);
			/* Store registers R0:R1 in the memory */
			ExtractExpr(e, opnd, 0);
			Generate("STO       ", "R0", opnd);
			ExtractExpr(e, opnd, 2);
			Generate("STO       ", "R1", opnd);
		}
	}
}

void CodeGen::NewLine() {
	Generate("WRNL      ", "", "");
}

int CodeGen::RetrieveVar(const string & s)
{
	unsigned int i;
	for (i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i].name == s) {
			return i;
		}
	}
	return -1;
}

void CodeGen::ProcessVar(ExprRec& e)
{
	if (!LookUp(e.name)) { /* variable not declared yet */
		SemanticError("variable " + e.name + \
				" was not declared before usage.");
	} else {
		e.kind = ID_EXPR;
		/* Retrieve the variable type */
		int varnum = RetrieveVar(e.name);
		e.var_type = symbolTable[varnum].type;
	}
}

void CodeGen::ProcessLit(ExprRec& e) {
	e.kind = LITERAL_EXPR;
	switch (e.var_type) {
	case BOOL:
		/* Reserved words are converted to lower in the scanner */
		e.ival = (scan.tokenBuffer == "true");
		break;
	case INT:
		e.ival = atoi(scan.tokenBuffer.data());
		break;
	case FLOAT:
		e.s_fval = scan.tokenBuffer;
		/* Create a temporary variable */
		GetTemp(e);
		break;
	case CHEESE:
		e.sval = scan.tokenBuffer;
		break;
	}
}

void CodeGen::ProcessOp(OpRec& o) {
	string c = scan.tokenBuffer;
	if (c == "+") {
		o.op = PLUS;
	} else if (c == "-") {
		o.op = MINUS;
	} else if (c == "*") {
		o.op = MULT;
	} else {
		o.op = DIV;
	}
}

void CodeGen::Listen(const ExprRec & inVar) {
	/* Addressing for variable - doesn't depend on type */
	string s;
	ExtractExpr(inVar, s, 0);
	/* Check variable type */
	switch (inVar.var_type) {
	case BOOL:
		/* Can't read a bool from keyboard */
		SemanticError(" listening to a BOOL is not supported.");
		break;
	case INT:
		Generate("RDI       ", s, "");
		break;
	case FLOAT:
		Generate("RDF       ", s, "");
		break;
	case CHEESE: /* TODO: check how to read strings */
		Generate("RDST      ", s, "");
		break;
	}
}

void CodeGen::Start() {
	Generate("LDA       ", "R15", "VARS");
	Generate("LDA       ", "R14", "STRS");
	Generate("LDA       ", "R13", "BOOL");
}

void CodeGen::Shout(const ExprRec & outExpr) {
	switch (outExpr.var_type) {
		case CHEESE:
			WriteString(outExpr);
			break;
		default:
			WriteExpr(outExpr);
			break;
	}
}

void CodeGen::WriteExpr(const ExprRec & outExpr) {
	string s;
	switch (outExpr.var_type) {
		case BOOL: /* Prints "False" or "True" strings */
			/* Load variable's address or literal's value */
			ExtractExpr(outExpr, s, 0);
			/* Compare variable to 0 */
			Generate("LD        ", "R0", s);
			Generate("IC         ", "R0", "#0");
			/* Jumps consider 4 bytes per instruction */
			/* skip 2 next instructions if variable is true */
			Generate("JNE        ", "&8", "");//s);
			/* String "False" */
			Generate("WRST       ", "+0(R13)", "");
			/* skip next instruction */
			Generate("JMP        ", "&4", "");
			/* String "True" */
			Generate("WRST       ", "+6(R13)", "");
			break;
		case INT:
			ExtractExpr(outExpr, s, 0);
			Generate("WRI       ", s, "");
			break;
		case FLOAT:
			/* There is no immediate addressing for FLOATs
			 * so the outExpr must be treated as a TEMP_EXPR
			 * even for the case of literals */
			/* Write the FLOAT value */
			ExtractExpr(outExpr, s, 0);
			Generate("WRF       ", s, "");
			break;
		default:
			SemanticError("There are no other options besides Bool, Cheese, Int or Float, please check  this out.");
			break;
	}
}

void CodeGen::WriteString(const ExprRec & outExpr) {
	string s, t;
	/* Save the string */
	s = outExpr.sval;
	str_vect.push_back(s);
	/* Update counter and Generate ASM */
	IntToAlpha(str_cnt, t);
	str_cnt += scan.cheese_size;
	if (str_cnt % 2) {
		str_cnt++;
	}
	s = "+" + t + "(R14)";
	Generate("WRST       ", s, "");
}

void CodeGen::DefineVar(ExprRec& var) {
	string varname = scan.tokenBuffer;
	if (LookUp(varname)) {
		SemanticError("variable " + varname + \
				" was already declared before.");
	} else { /* variable not declared yet */
		var.name = varname;
		Enter(var); /* declare it */
	}
}

void CodeGen::SemanticError(string msg) {
	cout << endl << " *** Semantic Error: " + msg << endl;
	cout << " *** Error on line " << scan.lineNumber + 1 << endl;
	exit(1); // abort on any semantic error
}

unsigned int CodeGen::NextControlStatementID() {
	return lastControlStatementID++;
}

void CodeGen::IfThen() {
	unsigned int id = NextControlStatementID();
	controlStatementLabels.push("IFEND" + to_string(id));
}

void CodeGen::IfElse() {
	string topLabel = controlStatementLabels.top();
	string elseLabel = "IFELSE" + topLabel.substr(5, topLabel.length() - 5);
}

void CodeGen::IfEnd() {
	string endLabel = controlStatementLabels.pop();
}
