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

void CodeGen::CheckId(const string & s) {
	if (!LookUp(s))  // variable not declared yet
		Enter(s);
}

void CodeGen::Enter(const string & s) {
	symbolTable.push_back(s);
}

void CodeGen::ExtractExpr(const ExprRec & e, string& s) {
	string t;
	int k, n;

	switch (e.kind) {
	case ID_EXPR:
	case TEMP_EXPR:  // operand form: +k(R15)
		s = e.name;
		n = 0;
		while (symbolTable[n] != s) n++;
		k = 2 * n;  // offset: 2 bytes per variable
		IntToAlpha(k, t);
			s ="+";
			s += t;
			s += "(R15)";
		break;
	case LITERAL_EXPR:
		IntToAlpha(e.val, t);
		s = "#";
		s += t;
	}
}
void CodeGen::ExtractStr(const ExprRec & e, string& s) {
	string t;
	int k, n;
	s = e.name;
	t = e.val;
	n = 0;
	while (symbolTable[n] != s){n++;}
	k = 2 * n;  // offset: 2 bytes per variable
	s = "#" + t + "(R15)";
}

string CodeGen::ExtractOp(const OpRec & o) {
	if (o.op == PLUS)
		return "IA        ";
	else
		return "IS        ";
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
	string s;
	static string t;

	t = "Temp&";
	IntToAlpha(++maxTemp, s);
	t += s;
	CheckId(t);
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
	if (symbolTable[i] == s)
		return true;

	return false;
}

// ******************************
// ** Public Member Functions  **
// ******************************

void CodeGen::Assign(const ExprRec & target, const ExprRec & source) {
	string s;

	ExtractExpr(source, s);
	Generate("LD        ", "R0", s);
	ExtractExpr(target, s);
	Generate("STO       ", "R0", s);
}

void CodeGen::Finish() {
	string s;

	listFile.width(6);
	listFile << ++scan.lineNumber << "  " << scan.lineBuffer << endl;
	Generate("HALT      ", "", "");
	Generate("LABEL     ", "VARS", "");
	IntToAlpha(int(2*(symbolTable.size()+1)), s);
	Generate("SKIP      ", s, "");
	Generate("LABEL     ", "VARS2", "");
//	cout << "HERE" << endl;
//	cout << stringsHolder[0] << " " << stringsHolder.size() << endl;
	for(int i=0; i < stringsHolder.size(); i++ ){
		string newString = "";
		for(int j=0; j<stringsHolder[i].length();j++){
			if(stringsHolder[i][j]==':' || stringsHolder[i][j] =='"'){
//				if(j!=0 && j!=stringsHolder[i].length()-1) {
					newString += ':';
//				}
			}
			newString+=stringsHolder[i][j];
//			cerr << stringsHolder[i][j] << " " << i << " " << j << endl;
		}
//		cerr << newString << endl;
		Generate("STRING ", '"'+newString+'"',"");
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
	for (unsigned i = 0; i < symbolTable.size(); i++) {
		listFile.width(7);
		listFile << 2*i << "       " << symbolTable[i] << endl;
	}
	listFile << " _____________________________________________"
		<< endl;
	listFile << endl;
	listFile << " Normal successful compilation." << endl;
	listFile.close();
}

void CodeGen::GenInfix(const ExprRec & e1, const OpRec & op,
                       const ExprRec & e2, ExprRec& e) {
	string opnd;

	if (e1.kind == LITERAL_EXPR && e2.kind == LITERAL_EXPR) {
		e.kind = LITERAL_EXPR;
		switch (op.op) {
		case PLUS:
			e.val = e1.val + e2.val;
			break;
		case MINUS:
			e.val = e1.val - e2.val;
		}
	} else {
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

void CodeGen::ProcessId(ExprRec& e) {
	CheckId(scan.tokenBuffer);
	e.kind = ID_EXPR;
	e.name = scan.tokenBuffer;
}

void CodeGen::ProcessLiteral(ExprRec& e) {
	e.kind = LITERAL_EXPR;
	e.val = atoi(scan.tokenBuffer.data());
}

void CodeGen::ProcessOp(OpRec& o) {
	if (scan.tokenBuffer == "+")
		o.op = PLUS;
	else
		o.op = MINUS;
}

void CodeGen::ReadId(const ExprRec & inVar) {
	string s;

	ExtractExpr(inVar, s);
	Generate("RDI       ", s, "");
}

void CodeGen::Start() {
	Generate("LDA       ", "R15", "VARS");
	Generate("LDA       ", "R14", "VARS2");
}

void CodeGen::WriteExpr(const ExprRec & outExpr) {
	string s;
	ExtractExpr(outExpr, s);
	Generate("WRI       ", s, "");
}

int CodeGen::CountMyChars(const string & countMe ) {
	int p = countMe.length();
	if (p % 2 == 0) {
		return p+2;
	}
	return p+1;
}

void CodeGen::WriteString() {
	string s;
	s = scan.stringBuffer;
//	cout << "write string" << endl;
//	cout << s << endl;
	stringsHolder.push_back(s);
	string a =  to_string(stringCounter);
	a += "(R14)";
	Generate("WRST       ",  "+"+a, "");
	stringCounter += CountMyChars(s);
}
