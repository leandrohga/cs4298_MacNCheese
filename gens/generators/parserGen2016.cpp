   /*********************************************************************
    *                         ParserGen.cpp                             *
    *                                                                   *
    *    G e n e r a t o r   f o r   P a r s i n g   R o u t i n e s    *
    *                                                                   *
    *                    - - O P T I M I Z E D - -                      *
    *                                                                   *
    *    Use PARSERGEN to automatically generate a C++ Parser class     *
    *    and its implementation for any LL(1) grammar. This program     *
    *    reads a .dat file containing the grammar required by LLGEN     *
    *    and it reads the .tbl file written by LLGEN. See the LLGEN     *
    *    documentation to properly format a grammar (.dat) file and     *
    *    the corresponding .tbl file produced by LLGEN.                 *
    *                                                                   *
    *    James L. Richards                            Version 5.0.3     *
    *                                   Last Update: September 2007     *
    *    Marty J. Wolf                  Last Update: February 2, 2016   *
    *                                                                   *
    *********************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include "Intset.h" // integer set class template

using namespace std;

const int
	MAX_PRODS    = 200,  // max number of productions 
	MAX_SYMS     =  50,  // max number of symbols per production 
	MAX_NONTERMS = 200,  // max number of nonterminals 
	MAX_TERMS    = 200,  // max number of terminals

	MAX_SYMBOLS  = MAX_TERMS + MAX_NONTERMS; 
						 // max number of terminals and nonterminals 


typedef int                Productions[MAX_SYMS+1];
typedef Productions        Grammar[MAX_PRODS+1];
typedef Intset<MAX_PRODS>  ProdSet;
typedef ProdSet            LhsProds[MAX_NONTERMS+1];
typedef int                LLtable[MAX_NONTERMS+1][MAX_TERMS+1];
typedef string             Symbols[MAX_SYMBOLS+1];
typedef string             TokenTable[MAX_TERMS+1][3];

int   nsymbols, // number of nonterminals 
		tnumber,  // number of terminals 
		snumber,  // number of semantic action symbols 
		pnumber;  // mumber of productions 

string inFile, 
       outFile, 
       lineBuffer, 
       header;

ofstream  hFile,    // stream for Parse class definition file
          cppFile;  // stream for Parse class implementation file

Grammar g;     
//  g[k] is the kth production; 1 <= k <= pnumber. 

LhsProds lhs;  
//  lhs[i] is the set of productions for which the ith nonterminal is 
//  the left hand side, 1 <= i <= nsymbols. 

LLtable t;     
//  t[i][j] is an LL(1) parse table; 1 <= i <= nsymbols, 
//  1 <= j <= tnumber. 

Symbols a;    
//  a[k] is the kth semantic action symbol,  1 <= k <= snumber. 

Symbols s;     
//  s[j] is the jth terminal symbol, 1 <= j <= tnumber, and 
//  s[tnumber + i] is the ith nonterminal symbol, 1 <= i <= nsymbols. 

TokenTable tok;

string PrepName(const string& str)
//
// Returns the string that results from capitalizing the first character
// in str, eliminating all underscores, and capitalizing each letter 
// following an underscore.
//
{
   string name = "";
   bool   capitalize = true;

   for (unsigned i = 0; i < str.length(); i++)
      if (str[i] == '_')
         capitalize = true;
      else if (capitalize)
      {
         name += toupper(str[i]);
         capitalize = false;
      }
      else
         name += str[i];
   return name;
}

void InitTbl()
//
//  Reads parsing information from the .tbl file.
//
{
	int      j, k, n;
	ifstream tblFile;

	tblFile.open((inFile+".tbl").data());
	if (!tblFile.is_open())
	{
		cout << "\nFile \"" + inFile + ".tbl\" not found!\n" << endl;
		exit(1);
	}
	tblFile >> nsymbols >> tnumber >> snumber >> pnumber;
	tblFile.ignore(256, '\n');

	// Read grammar productions
	for (int i = 1; i <= pnumber; i++)  
	{
		k = 0;
		do
		{
         k++;
         tblFile >> g[i][k];
		} while (g[i][k] != 1000);
    }

	// Initialize parse table
   for (int i = 1; i <= MAX_NONTERMS; i++)  
      for (int j = 1; j <= MAX_TERMS; j++)
         t[i][j] = 0;

	// Fill parse table
	tblFile >> n >> j >> k;
	tblFile.ignore(256, '\n');
	t[n-tnumber-1][j] = 1;
	lhs[n-tnumber-1].Add(1);
	while (n > 0)   
   {
      int i = n - tnumber;
      t[i][j] = k;
      if (!lhs[i].Contains(k))
         lhs[i].Add(k);
      tblFile >> n >> j >> k;
      tblFile.ignore(256, '\n');
	}

   // Fill list of semantic actions
	for (int i = 1; i <= snumber; i++)
   {
      tblFile >> k;
		tblFile >> a[k];
		tblFile.ignore(256, '\n');
      a[k] = PrepName(a[k]);
	}

	// Fill symbol list.
	for (int i = 1; i <= nsymbols + tnumber; i++)
	{
		tblFile >> k;
		tblFile >> s[k];
		tblFile.ignore(256, '\n');
      if (i > tnumber)
         s[k] = PrepName(s[k]);
	}

	tblFile.close();
}

void InitDat()
//
//  Reads token information from the .dat grammar file.
//
{
	ifstream datFile;
	string   lineBuffer;
	int      k;

	datFile.open((inFile+".dat").data());
	getline(datFile, lineBuffer);
	k = 0;
	while (lineBuffer[k] != ' ') k++;
   while (lineBuffer[k] == ' ' || lineBuffer[k] == '\t') k++;
	header = lineBuffer.substr(k, lineBuffer.length() - k);
	getline(datFile, lineBuffer);
	getline(datFile, lineBuffer);
	for (int n = 1; n <= tnumber; n++)
	{
		k = 1;
		tok[n][1] = "";
		while (lineBuffer[k] != '"')
		{
			tok[n][1] += lineBuffer[k];
			k++;
		}
		do k++; while (lineBuffer[k] == ' ');
		tok[n][2] = "";
		while (lineBuffer[k] != '.')
		{
			tok[n][2] += lineBuffer[k];
			k++;
		}
		getline(datFile, lineBuffer);
	}

	datFile.close();
}

string MakeId (int n)
//
//  Creates the name for a token or function corresponding to the nth
//  symbol in list s.
//
{
	if (n <= tnumber) return tok[n][2];
	if (s[n] == "<goal>") return "SystemGoal";
   return s[n];
}

void GenActions(int p, int indent)
//
//  Generates function calls for parsing actions.
//
{
	int i;

	if (g[p][1] < 1000)
	{
		i = 1;
		do
		{
			for (int j = 1; j <= indent; j++) cppFile << "\011";
			if (g[p][i] < 0)
				cppFile << "// code." << a[-g[p][i]] << "();" << endl;
			else if (g[p][i] <= tnumber)
				cppFile << "Match(" << MakeId(g[p][i]) << ");" << endl;
			else
				cppFile << MakeId(g[p][i]) << "();" << endl;
			i++;
		} while (g[p][i] != 1000);
	}
}

void CaseLabels (int a, int p, int indent)
{
	for (int x = 1; x <= tnumber; x++)
		if (t[a-tnumber][x] == p)
		{
			for (int j = 1; j <= indent; j++) cppFile << "\011";
			cppFile << "case " << MakeId(x) << ':' << endl;
		}
}

void MakeParsingProc(int a)
//
//  Generates a parsing function for the nonterminal s[a].
//
{
	int p;

	hFile << endl;
	if (MakeId(a) != "SystemGoal")
		hFile << "\011void " << MakeId(a) << "();" << endl;
	cppFile << endl;
	cppFile << "void Parser::" << MakeId(a) << "()" << endl;
	cppFile << '{' << endl;
	if (lhs[a-tnumber].Size() > 1)  // Generate case statement
	{
		cppFile << "\011switch (NextToken())" << endl;
		cppFile << "\011{" << endl;
		for (int p = 1; p <= pnumber; p++)
			if (lhs[a-tnumber].Contains(p))
			{
				CaseLabels(a, p, 1);
				GenActions(p, 2);
				cppFile << "\011\011break;" << endl;
			}
		cppFile << "\011default:" << endl;
		cppFile << "\011\011SyntaxError(NextToken(), \"\");" << endl;
		cppFile << "\011}" << endl;
	}
	else
	{
		p = 0;
		do p++; while (!lhs[a-tnumber].Contains(p));
		GenActions(p, 1);
	}
	cppFile << '}' << endl;
}

void StartClassDefinition()
// 
//  Starts building the Parser class definition file.
//
{
	hFile.open((outFile+".h").data());
	hFile << "// " << header << endl;
	hFile << "// " << endl;
	hFile << "// Parser Class Definition File\n" << endl;
	hFile << "// SHELL GENERATED BY PARSERGEN VERSION 5.0.3" << endl;
	hFile << endl;
	hFile << "#ifndef PARSER" << endl;
	hFile << "#define PARSER" << endl;
	hFile << endl;
	hFile << "#include <string>" << endl;
	hFile << "using namespace std;\n" << endl;
	hFile << "#include \"_____.h\" // scanner class definition\n"
         << endl;
	hFile << "#include \"_____.h\" // code generator class definition\n"
         << endl;
	hFile << "class Parser" << endl;
	hFile << '{' << endl;
	hFile << "public:" << endl;
	hFile << endl;
	hFile << "\011Parser();" << endl;
	hFile << endl;
	hFile << "\011void SystemGoal();" << endl;
	hFile << endl;
	hFile << "private:" << endl;
	hFile << endl;
	hFile << "\011Token savedToken;" << endl;
	hFile << "\011bool  tokenAvailable;" << endl;
	hFile << endl;
	hFile << "\011void SyntaxError(Token t, string msg);" << endl;
	hFile << endl;
	hFile << "\011Token NextToken();" << endl;
	hFile << endl;
	hFile << "\011void Match(Token t);" << endl;
}

void EndClassDefinition()
// 
// Finishes buildiing the Parser class implementation file.
//
{
	hFile << "}; // end of Parser class" << endl;
	hFile << endl;
	hFile << "#endif" << endl;
	hFile.close();
}

void StartClassImplementation()
// 
//  Starts building the Parser class implementation file.
//
{
	cppFile.open((outFile+".cpp").data());
	cppFile << "// " << header << endl;
	cppFile << "// " << endl;
	cppFile << "// Parser Class Implementation File\n" << endl;
	cppFile << "// SHELL GENERATED BY PARSERGEN VERSION 5.0.3\n" << endl;
	cppFile << "#include <iostream>" << endl;
	cppFile << "#include <string>" << endl;
	cppFile << "using namespace std;\n" << endl;
	cppFile << "#include \"" + outFile + ".h\"\n" << endl;
	cppFile << "Parser::Parser()" << endl;
	cppFile << '{' << endl;
	cppFile << "\011tokenAvailable = false;" << endl;
	cppFile << '}' << endl;
	cppFile << endl;
 	cppFile << "void Parser::SyntaxError(Token t, string msg)" << endl;
	cppFile << '{' << endl;
	cppFile << "\011cout << \"Syntax Error: \" + msg << endl;" << endl;
	cppFile << "\011exit(1); // abort on any syntax error" << endl;
	cppFile << '}' << endl;
	cppFile << endl;
	cppFile << "Token Parser::NextToken()" << endl;
	cppFile << '{' << endl;
	cppFile << "\011if (!tokenAvailable)" << endl;
	cppFile << "\011{" << endl;
	cppFile << "\011\011savedToken = scan.GetNextToken();" << endl;
	cppFile << "\011\011tokenAvailable = true;" << endl;
	cppFile << "\011}" << endl;
	cppFile << "\011return savedToken;" << endl;
 	cppFile << '}' << endl;
	cppFile << endl;
	cppFile << "void Parser::Match(Token t)" << endl;
	cppFile << '{' << endl;
	cppFile << "\011if (t != NextToken())" << endl;
	cppFile << "\011\011SyntaxError(t, \"\");" << endl;
	cppFile << "\011else" << endl;
	cppFile << "\011\011tokenAvailable = false;" << endl;
	cppFile << '}' << endl;
	cppFile << endl;
}

void EndClassImplementation()
{
	cppFile.close();
}

/*************************************************************************/

int main()
{
	cout << setw(53) << "P a r s e r   G e n e r a t o r" << endl 
		 << endl;
	cout << setw(54) << "  Version 5.0.3  (February 2016) " << endl;
	cout << endl;
	cout << "You must have your grammar in an LLGEN file (.DAT) and run"
		<< " LLGEN to produce a" << endl;
	cout << "parsing table file (.TBL) before using this generator to"
		<< " write C++ code for a" << endl;
	cout << "Parser class.\n\n" << endl;
	cout << "Name of your LLGEN table file (.tbl extension assumed): ";
	getline(cin, inFile);

	InitTbl();
	InitDat();

	cout << "\n\nName for the parser file (no extension): ";
	getline(cin, outFile);

	StartClassDefinition();
	StartClassImplementation();

	for (int k = tnumber + nsymbols; k >= tnumber + 1; k--)
		MakeParsingProc(k);

	EndClassDefinition();
	EndClassImplementation();

	cout << endl;
	cout << "\nParser class definition file ==> " 
		 << outFile + ".h\n\n" 
		 << "Parser class implementation file ==> " 
		 << outFile + ".cpp\n\n"
		 << endl;

//	cin.get();
	return 0;
}
