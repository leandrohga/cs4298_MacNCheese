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

void CodeGen::CheckId(ExprRec& var) {
	if (!LookUp(var.name)) { // variable not declared yet
		/* FIXME: check variable type */
		Enter(var);
	}
}

void CodeGen::Enter(ExprRec& var) {
	/* Create the key and fill it */
	symbol_node_t variable;
	variable.name = var.name;
	variable.type = var.var_type;
	/* Check variable size */
	switch (var.var_type) {
	case BOOL:
		variable.size = 2; /* all operations are 16 bits */
		break;
	case INT:
		variable.size = 2; /* 2x8 = 16 bits */
		break;
	case FLOAT:
		variable.size = 4; /* 4x8 = 32 bits */
		break;
	default:
		/* TODO: check what to do. Check for cheese? */
		break;
	}
	/* Add the record to the symbol table */
	symbolTable.push_back(variable);
}

void CodeGen::ExtractExpr(const ExprRec & e, string& s) {
	string t;
	int k, n;

	/* TODO: check variable type */
	switch (e.kind) {
	case ID_EXPR:
	case TEMP_EXPR:  // operand form: +k(R15)
		s = e.name;
		k = n = 0;
		while (symbolTable[n].name != s) {
			n++;
			k += symbolTable[n].size;
		}
		/* FIXME: check what to do for other types */
		IntToAlpha(k, t);
		s = "+" + t + "(R15)";
		break;
	case LITERAL_EXPR:
		IntToAlpha(e.ival, t);
		s = "#" + t;
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
/* TODO: please check this for float + - / *    */
string CodeGen::ExtractOpFloat(const OpRec & o) {
	if (o.op == PLUS) {
		return "FADD        ";
	} else if (o.op == MINUS) {
		return "FSUB        ";
	} else if (o.op == MULT) {
		return "FMUL        ";
	} else {
		return "FDIV        ";
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

string CodeGen::GetTemp() {
	/* FIXME: check what to do for other types */
	string s;
	static string t;

	t = "Temp&";
	IntToAlpha(++maxTemp, s);
	t += s;
	ExprRec var;
	var.name = t;
	var.var_type = INT;
	CheckId(var);
	return t;
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
	/* TODO check variable types, add other types */
	string s;
	ExtractExpr(source, s);
	Generate("LD        ", "R0", s);
	ExtractExpr(target, s);
	Generate("STO       ", "R0", s);
}

vector<string> str_vect;
int str_cnt = 0;

int CodeGen::CalcTableSize() {
	int i, index = 0;
	for (i = 0; i < symbolTable.size(); i++) {
		index += symbolTable[i].size;
	}
	return index;
}

void CodeGen::Finish() {
	string s;

	listFile.width(6);
	listFile << ++scan.lineNumber << "  " << scan.lineBuffer << endl;
	Generate("HALT      ", "", "");
	Generate("LABEL     ", "VARS", "");
	int table_size = CalcTableSize();
	IntToAlpha(table_size, s);
	Generate("SKIP      ", s, "");
	Generate("LABEL     ", "STRS", "");
	while (!str_vect.empty()) {
		s = str_vect.front();
		str_vect.erase(str_vect.begin());
		Generate("STRING    ", s, "");
	}
	outFile.close();
	listFile << endl << endl;
	listFile << " _____________________________________________\n";
	listFile << " <><><><>   S Y M B O L   T A B L E   <><><><>\n"
		<< endl;
	listFile << " Relative" << endl;
	listFile << " Address      Identifier" << endl;
	listFile << " --------     --------------------------------"
		<< endl;
	int i, index = 0;
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
		SemanticError(" mixed-mode arithmetic operations"
				" are not allowed.");
	} else if ((e1.var_type != INT) && (e1.var_type != FLOAT)) {
		SemanticError(" arithmetic opertions are allowed only for"
				" INTs and FLOATs.");
	}
	/* Result type = operands types */
	e.var_type = e1.var_type;

	/* Literals */
	if (e1.kind == LITERAL_EXPR && e2.kind == LITERAL_EXPR) {
		e.kind = LITERAL_EXPR;
		switch (op.op) {
		case PLUS:
			e.ival = e1.ival + e2.ival;
			e.fval = e1.fval + e2.fval;
			break;
		case MINUS:
			e.ival = e1.ival - e2.ival;
			e.fval = e1.fval - e2.fval;
			break;
		case MULT:
			e.ival = e1.ival * e2.ival;
			e.fval = e1.fval * e2.fval;
			break;
		case DIV:
			e.ival = e1.ival / e2.ival;
			e.fval = e1.fval / e2.fval;
			break;
		}
	} else { /* Variables */
		string opnd;
		/* TODO: check variable type */
		e.kind = TEMP_EXPR;
		e.name = GetTemp();
		ExtractExpr(e1, opnd);
		Generate("LD        ", "R0", opnd);
		ExtractExpr(e2, opnd);
		Generate(ExtractOp(op), "R0", opnd);
		ExtractExpr(e, opnd);
		Generate("STO       ", "R0", opnd);
	}
}

void CodeGen::NewLine() {
	Generate("WRNL      ", "", "");
}

int CodeGen::RetrieveVar(const string & s)
{
	int i;
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
		SemanticError("Variable " + e.name + \
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
		/* TODO: check if this works */
		e.bval = (scan.tokenBuffer == "True");
		break;
	case INT:
		e.ival = atoi(scan.tokenBuffer.data());
		break;
	case FLOAT:
		/* TODO: check how to scan floats, they are 32bits */
		e.fval = atof(scan.tokenBuffer.data());
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
	VarKind var_type;
	/* Check if variable was declared before usage */
	if (!LookUp(inVar.name)) { /* variable not declared yet */
		SemanticError("Variable " + inVar.name + \
				" was not declared before usage.");
	} else {
		/* Retrieve the variable type */
		int varnum = RetrieveVar(inVar.name);
		var_type = symbolTable[varnum].type;
	}

	/* Addressing for variable - doesn't depend on type */
	string s;
	ExtractExpr(inVar, s);
	/* Check variable type */
	switch (var_type) {
	case BOOL: /* TODO: check how to read a bool */
	case INT:
		Generate("RDI       ", s, "");
		break;
	case FLOAT: /* TODO: check for MnC dd.ddesdd format?? */
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
		case BOOL:
			/*TODO: please check this statement and check if it is
			 * right how I am vverifying if it is true or false */
			//ExtractExpr(outExpr, s);
			cerr << outExpr.bval << endl;
			if(outExpr.bval){
				Generate("WRI       ", "bl", "1");
			}else{
				Generate("WRI       ", "bl", "0");
			}
			break;
		case INT:
			ExtractExpr(outExpr, s);
			Generate("WRI       ", s, "");
			break;
		case FLOAT:
			/*TODO FIXME: fix ExtractExpr for the case of LITs */
			ExtractExpr(outExpr, s);
			Generate("WRF       ", s, "");
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
	/* TODO Start checking variable type */
	string varname = scan.tokenBuffer;
	if (LookUp(varname)) {
		/* FIXME is this a semantic error? */
		SemanticError("Variable " + varname + \
				" was already declared before.");
	} else { /* variable not declared yet */
		var.name = varname;
		Enter(var); /* declare it */
		/* TODO Assign 0 to the variable, check if SAM does. */
	}
}

void CodeGen::SemanticError(string msg) { /* FIXME should this be here? */
	cout << " *** Semantic Error: " + msg << endl;
	cout << " *** Error on line " << scan.lineNumber << endl;
	exit(1); // abort on any semantic error
}
