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
	SymbolEntry variable;
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
			variable.s_fval = var.s_fval; /* use assigned value */
		} else {
			variable.s_fval = "0.0"; /* init with 0.0 */
		}
		break;
	case CHEESE:
		variable.label = NewStringLabel();
		if (var.kind == ID_EXPR) {
			/* TODO: allow other sizes and check if it is even */
			variable.size = CHEESE_SIZE_DEF; /* default size */
		} else {
			int var_size = scan.cheese_size;
			/* Only even number of bytes are allowed */
			if (var_size % 2) {
				var_size++;
			}
			variable.size = var_size;
			/* Set the initial value of the string */
			variable.sval = var.sval;
		}
		break;
	default:
		SemanticError("this variable type does not exist.");
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
	case TEMP_EXPR:  /* operand form: +k(R15) */
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
		case CHEESE:
			/* Float and Cheese operations don't allow immediate
			 * addressing. They are treated as TEMP_EXPR.
			 */
			/* operand form: +k(R15) */
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
			SemanticError("this literal type does not exist.");
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
	if (!LookUp(var.name)) { /* variable not declared yet */
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

string CodeGen::NewStringLabel() {
	return ("STR" + to_string(StrLabelID++));
}

// ******************************
// ** Public Member Functions  **
// ******************************

void CodeGen::Assign(const ExprRec & target, const ExprRec & source) {
	string s;
	string t;
	string x;
	int maxLength = 1024;
	int singleWordLength = 0;
	string theWord = "";
	switch (target.var_type) {
	case BOOL:
	case INT:
		ExtractExpr(source, s, 0);
		Generate("LD        ", "R0", s);
		ExtractExpr(target, s, 0);
		Generate("STO       ", "R0", s);
		break;
	case FLOAT:
		/* Load the 32 bits into registers R0:R1*/
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
	case CHEESE: /* 1024 bits max in ascii if I am
		not wrong 1 char = 1 bit
		reference demo7 in sam folder */
		// we have to check the size of the cheese (1024)
		// and check the size of the string getting in
		// and cut it if is longer and register enough
		// space for the shorter one.
		// source is right side of assignment
		ExtractExpr(target, t, 0);
		ExtractExpr(source, s, 0);
		if(maxLength > s.length()){ //here we check for the size of the string, if the string is longer than the amount registered we go with the amount register, otherwise we go with the size of the string/cheese
			maxLength = s.length();
		}
		for(int i=0; i < maxLength; i++){ //we go through each char and we make the string be of the correct size (or intended size)
			x.push_back( s[i] );
		}
		if(maxLength % 2 != 0){ //we check for the length if this is even we need to add to chars, an empty char and the end of string
			maxLength +=1;
		}
		for(int i = 0; i < x.length(); i++){
			singleWordLength++;
			theWord.push_back( x[i] );
			if(x[i] == ' ' || i == (x.length()-1)){
				theWord = "\"" + theWord + "\"";
				Generate("LD        ", "R0", t);
				Generate("STO       ", "R0", theWord);
				Generate("JMP        ", "&"+(to_string(singleWordLength)), "");

				singleWordLength = 0;
				theWord = "";
			}
			Generate("STO       ", "R0", "\"" + x + "\"");
			Generate("JMP        ", "&"+(to_string(maxLength)), "");
			/* FIXME */
		}
		break;

	default:
		SemanticError("this variable type does not exist." \
				" It cannot be assigned.");
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
		case CHEESE:
			s = symbolTable[i].label;
			Generate("LABEL     ", s, "");
			/* FIXME: is there a better way to check this? */
			/* Check if it is a temporary/literal variable */
			if (symbolTable[i].name.find("Temp&", 0) == 0) {
				s = symbolTable[i].sval;
				Generate("STRING    ", s, "");
			} else { /* ID variable */
				s = to_string(symbolTable[i].size);
				Generate("SKIP      ", s, "");
			}
			break;
		}
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
		default:
			SemanticError("This operator is not supported");
			break;
		}
	} else { /* Variables */
		string opnd;
		/* TODO: set e.kind to TEMP_EXPR?? */
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
		/* Create a temporary variable */
		GetTemp(e);
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
	} else if (c == "/") {
		o.op = DIV;
	} else if (c == "<") {
		o.op = LT;
	} else if (c == "<=") {
		o.op = LE;
	} else if (c == ">") {
		o.op = GT;
	} else if (c == ">=") {
		o.op = GE;
	} else if (c == "==") {
		o.op = EQ;
	} else if (c == "!!") {
		o.op = EQ;
	} else if (c == "!=") {
		o.op = NE;
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
	Generate("LDA       ", "R13", "BOOL");
}

void CodeGen::Shout(const ExprRec & outExpr) {
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
		Generate("JNE        ", "&8", "");
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
	case CHEESE:
		/* There is no immediate addressing for CHEESEs
		 * so the outExpr must be treated as a TEMP_EXPR
		 * even for the case of literals */
		/* Write the CHEESE value */
		ExtractExpr(outExpr, s, 0);
		Generate("WRST      ", s, "");
		break;
	}
}

void CodeGen::DefineVar(ExprRec& var) {
	string varname = scan.tokenBuffer;
	if (LookUp(varname)) {
		SemanticError("variable " + varname + \
				" was already declared before.");
	} else { /* variable not declared yet */
		var.name = varname;
		var.kind = ID_EXPR;
		Enter(var); /* declare it */
	}
}

void CodeGen::SemanticError(string msg) {
	cout << endl << " *** Semantic Error: " + msg << endl;
	cout << " *** Error on line " << scan.lineNumber + 1 << endl;
	exit(1); /* abort on any semantic error */
}

void CodeGen::CheckNStoreCondition(const OpRec & op, const ExprRec & result) {
	/* Do the comparison */
	switch (op.op) {
	case LT:
		/* Skip 2 instruction case Less Then */
		Generate("JLT       ", "&8", "");
		break;
	case LE:
		/* Skip 2 instruction case Less or Equal */
		Generate("JLE       ", "&8", "");
		break;
	case GT:
		/* Skip 2 instruction case Greater Then */
		Generate("JGT       ", "&8", "");
		break;
	case GE:
		/* Skip 2 instruction case Greater or Equal */
		Generate("JGE       ", "&8", "");
		break;
	case EQ:
		/* Skip 2 instruction case Equal */
		Generate("JEQ       ", "&8", "");
		break;
	case NE:
		/* Skip 2 instruction case Not Equal */
		Generate("JNE       ", "&8", "");
		break;
	default:
		/* Nothing to do */
		/* Maybe an error? */
		break;
	}
	/* Store the results */
	string opnd;
	ExtractExpr(result, opnd, 0); /* Extract output variable address */
	Generate("LD        ", "R0", "#0"); /* Load 0-False into R0 */
	Generate("JMP       ", "&4", ""); /* Skip the next instruction */
	Generate("LD        ", "R0", "#1"); /* Load 1-True into R0 */
	Generate("STO       ", "R0", opnd); /* Store R0 into output variable */
}

void CodeGen::SetCondition(const ExprRec & e1, const OpRec & op,
		const ExprRec & e2, ExprRec & e) {
	if (e1.var_type != e2.var_type) {
		SemanticError("mixed-mode relational operations"
				" are not allowed.");
	} else if ((e1.var_type != INT) && (e1.var_type != FLOAT)) {
		/* FIXME TODO Add support to cheese and bools */
		SemanticError("relational opertions are allowed only for"
				" INTs and FLOATs.");
	}
	/* Result type = BOOL */
	e.var_type = BOOL;
	e.kind = TEMP_EXPR; /* Result is always a temp variable */
	GetTemp(e);
	string opnd;

	/* Literals */
	if ((e1.kind == LITERAL_EXPR && e2.kind == LITERAL_EXPR)
			&& (e.var_type != FLOAT)) {
		/* FLOAT literal expressions are calculated at runtime */
		switch (op.op) {
		case LT: //LESS THEN
			e.ival = (e1.ival < e2.ival);
			break;
		case LE: //LESS OR EQUAL
			e.ival = (e1.ival <= e2.ival);
			break;
		case GT: //GREATER THEN
			e.ival = (e1.ival > e2.ival);
			break;
		case GE: //GREATER OR EQUAL
			e.ival = (e1.ival >= e2.ival);
			break;
		case EQ: //EQUAL
			e.ival = (e1.ival == e2.ival);
			break;
		case NE: //NOT EQUAL
			e.ival = (e1.ival != e2.ival);
			break;
		default:
			/*
			 * There is nothing to be done here.
			 * Maybe an error?
			 */
			break;
		}
		/* Evaluate result and store it */
		if (e.ival) {
			/* Load 1-True into R0 */
			Generate("LD        ", "R0", "#1");
		} else {
			/* Load 0-False into R0 */
			Generate("LD        ", "R0", "#0");
		}
		/* Extract output variable address */
		ExtractExpr(e, opnd, 0);
		/* Store R0 into output variable */
		Generate("STO       ", "R0", opnd);
	} else { /* Variables */
		if (e1.var_type == INT) {/* INT */
			/* Load the 16 bits into register R0 */
			ExtractExpr(e1, opnd, 0);
			Generate("LD        ", "R0", opnd);
			/* 16 bits Int comparison */
			ExtractExpr(e2, opnd, 0);
			Generate("IC        ", "R0", opnd);
			/* Store the boolean result in the memory
			 * according to the operation */
			CheckNStoreCondition(op, e);
		} else { /* FLOAT */
			/* Load the 32 bits into registers R0:R1 */
			ExtractExpr(e1, opnd, 0);
			Generate("LD        ", "R0", opnd);
			ExtractExpr(e1, opnd, 2);
			Generate("LD        ", "R1", opnd);
			/* 32 bits Float comparison */
			ExtractExpr(e2, opnd, 0);
			Generate("FC        ", "R0", opnd);
			/* Store the boolean result in the memory
			 * according to the operation */
			CheckNStoreCondition(op, e);
		}
	}
}

unsigned int CodeGen::NextControlStatementID() {
	return lastControlStatementID++;
}

/*
 * How the controlStatementLabels stack works for if statements:
 * IfThen()
 * 	- Pushes an IFEND and an IFELSE label on the stack
 * IfElse()
 * 	- Pops the IFELSE label off the stack
 * IfEnd()
 *	- Looks at the top of the stack
 *	- If the top item was an IFELSE label, pop it
 *	- Pop the IFEND label
 */
void CodeGen::IfThen(const ExprRec& bool_cond) {
	unsigned int id = NextControlStatementID();
	controlStatementLabels.push("IFEND" + to_string(id));
	string elseLabel = "IFELSE" + to_string(id);
	controlStatementLabels.push(elseLabel);
	string cond_addr;
	ExtractExpr(bool_cond, cond_addr, 0);
	/* Load bool value (from a condition) */
	Generate("LD        ", "R0", cond_addr);
	/* Check the bool value */
	Generate("IC        ", "R0", "#0");
	/* Jump to else case the bool value is False */
	Generate("JEQ       ", elseLabel, "");
}

void CodeGen::IfElse() {
	string elseLabel = controlStatementLabels.top();
	controlStatementLabels.pop();
	string endLabel = controlStatementLabels.top();
	/* Jump to the end label */
	Generate("JMP       ", endLabel, "");
	/* Create an else label */
	Generate("LABEL     ", elseLabel, "");
}

void CodeGen::IfEnd() {
	string endLabel;
	string unknownLabel = controlStatementLabels.top();
	if (unknownLabel.find("IFELSE") != string::npos) {
		controlStatementLabels.pop();
		Generate("LABEL     ", unknownLabel, "");
		endLabel = controlStatementLabels.top();
		Generate("JMP       ", endLabel, "");
	} else {
		endLabel = unknownLabel;
	}
	controlStatementLabels.pop();
	/* Generate the end label */
	Generate("LABEL     ", endLabel, "");
}

void CodeGen::LoopBegin() {
	/* Generate the label for the do until statement */
	unsigned int id = NextControlStatementID();
	string doutLabel = "DOUTL" + to_string(id);
	controlStatementLabels.push(doutLabel);
	/* Generate the do until label */
	Generate("LABEL     ", doutLabel, "");
	/* Push do until end label (for the break statement to work) */
	controlStatementLabels.push("DOUTLEND" + to_string(id));
}

void CodeGen::LoopEnd(const ExprRec& bool_cond) {
	/* Read and pop the label */
	string doutEndLabel = controlStatementLabels.top();
	controlStatementLabels.pop();
	string doutLabel = controlStatementLabels.top();
	controlStatementLabels.pop();
	/* Check the condition */
	string cond_addr;
	ExtractExpr(bool_cond, cond_addr, 0);
	/* Load bool value (from a condition) */
	Generate("LD        ", "R0", cond_addr);
	/* Check the bool value */
	Generate("IC        ", "R0", "#0");
	/* Jump to Do case the bool value is False */
	Generate("JEQ       ", doutLabel, "");
	/* Provide an end label for the break statement */
	Generate("LABEL     ", doutEndLabel, "");
}

void CodeGen::WhileTag() {
	/* Generate the label for the (end of) while statement */
	unsigned int id = NextControlStatementID();
	string whlendLabel = "WHLEND" + to_string(id);
	controlStatementLabels.push(whlendLabel);
	/* Generate the whlbeg label */
	string whlbegLabel = "WHLBEG" + to_string(id);
	Generate("LABEL     ", whlbegLabel, "");
}

void CodeGen::WhileBegin(const ExprRec& bool_cond) {
	/* Read the end label */
	string whlendLabel = controlStatementLabels.top();
	/* Check condition */
	string cond_addr;
	ExtractExpr(bool_cond, cond_addr, 0);
	/* Load bool value (from a condition) */
	Generate("LD        ", "R0", cond_addr);
	/* Check the bool value */
	Generate("IC        ", "R0", "#0");
	/* Jump to end of While statement case the bool value is False */
	Generate("JEQ       ", whlendLabel, "");
}

void CodeGen::WhileEnd() {
	/* Read and pop the end label */
	string whlendLabel = controlStatementLabels.top();
	controlStatementLabels.pop();
	/* Recreate begin label */
	string whlbegLabel = "WHLBEG" + whlendLabel.substr(6, whlendLabel.length() - 6);
	/* Generate a Jump to the whlbegin label */
	Generate("JMP       ", whlbegLabel, "");
	/* Generate the whlend label */
	Generate("LABEL     ", whlendLabel, "");
}

void CodeGen::Break() {
	/* Copy the stack */
	stack<string> fakeStack = controlStatementLabels;
	/* Find the end of the most inner loop */
	string endLabel = fakeStack.top();
	/* while not a loop label */
	int end = endLabel.find("IF", 0);
	while (end == 0) {
		if (fakeStack.size() == 0) {
			SemanticError("break statement is not inside a loop");
		}
		fakeStack.pop();
		endLabel = fakeStack.top();
		end = endLabel.find("IF", 0);
	}
	/* Jump to the end label */
	Generate("JMP       ", endLabel, "");
}

void CodeGen::ForTag() {
	/* Generate the strings for the labels */
	unsigned int id = NextControlStatementID();
	string fortestLabel = "FORTST" + to_string(id);
	/* End label and stack */
	string forendLabel = "FOREND" + to_string(id);
	/* Push end label to the stack */
	controlStatementLabels.push(forendLabel);
	/* Generate code for fortest Label */
	Generate("LABEL     ", fortestLabel, "");
}

void CodeGen::ForAssign(const ExprRec & target, const ExprRec & source) {
	/* TODO: check if this works in every case. */
	/* FIXME: it will probably stop working when we fix Assign */
	Assign(target, source);
}

void CodeGen::ForBegin(const ExprRec& bool_cond) {
	/* Read the end label */
	string forendLabel = controlStatementLabels.top();
	/* Recreate for body label */
	string forbodyLabel = "FORBDY" + forendLabel.substr(6, forendLabel.length() - 6);

	/* Check condition */
	string cond_addr;
	ExtractExpr(bool_cond, cond_addr, 0);
	/* Load bool value (from a condition) */
	Generate("LD        ", "R0", cond_addr);
	/* Check the bool value */
	Generate("IC        ", "R0", "#0");
	/* Jump to FORBDY case the bool value is True */
	Generate("JNE       ", forbodyLabel, "");
	/* Jump to FOREND case the bool value is False */
	Generate("JMP       ", forendLabel, "");

	/* Update */
	string forupdLabel = "FORUPD" + forendLabel.substr(6, forendLabel.length() - 6);
	/* For update Label */
	Generate("LABEL     ", forupdLabel, "");
}

void CodeGen::ForUpdate() {
	/* Read the end label */
	string forendLabel = controlStatementLabels.top();
	/* Recreate for test label */
	string fortestLabel = "FORTST" + forendLabel.substr(6, forendLabel.length() - 6);
	/* Recreate for body label */
	string forbodyLabel = "FORBDY" + forendLabel.substr(6, forendLabel.length() - 6);

	/* Jump to for test */
	Generate("JMP       ", fortestLabel, "");
	/* Generate the for body label */
	Generate("LABEL     ", forbodyLabel, "");
}

void CodeGen::ForEnd() {
	/* Read and pop the end label */
	string forendLabel = controlStatementLabels.top();
	controlStatementLabels.pop();
	/* Recreate for update label */
	string forupdLabel = "FORUPD" + forendLabel.substr(6, forendLabel.length() - 6);

	/* Jump to for test */
	Generate("JMP       ", forupdLabel, "");
	/* Generate the for body label */
	Generate("LABEL     ", forendLabel, "");
}
