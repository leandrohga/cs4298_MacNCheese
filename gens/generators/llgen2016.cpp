 /*_________________________________________________________________________
                                                                     
     LLL           LLL           GGGGGGGGG    EEEEEEEEEEE   NNN      NNN
     LLL           LLL          GGGGGGGGGGG   EEEEEEEEEEE   NNNN     NNN
     LLL           LLL          GGG     GGG   EEE           NNNNN    NNN
     LLL           LLL          GGG           EEE           NNNNNN   NNN
     LLL           LLL          GGG  GGGGGG   EEEEEEE       NNN NNN  NNN
     LLL           LLL          GGG  GGGGGG   EEEEEEE       NNN  NNNNNNN
     LLL           LLL          GGG     GGG   EEE           NNN   NNNNNN
     LLL           LLL          GGG     GGG   EEE           NNN    NNNNN
     LLLLLLLLLLL   LLLLLLLLLL    GGGGGGGGGG   EEEEEEEEEEE   NNN     NNNN
     LLLLLLLLLLL   LLLLLLLLLL     GGGGGGGG    EEEEEEEEEEE   NNN      NNN
   ________________________________________________________________________
 
                               LLGEN2007.cpp                            

               P a r s i n g    T a b l e s   G e n e r a t o r 

   This program produces parsing tables for an LL(1)grammar. The
   grammar must be provided in a file that has a .dat extension and it
   must have the format described below. LLGEN2007 generates a .tbl 
   file that can be read by the PARSERGEN2007 arser generator and an
   optional .gen file that provides information from the .tbl file
   in more human-readable form.

   LLGEN2007 is a C++ version of a program originally coded in Pascal.
   Its development history is summarized below.

   Written by Roy J. Mongiovi (1/10/1979) and Lee McGraw (3/20/1980).
   Changes made by Dana Eckart (1/23/1986 and 10/23/1986).
   C++ version written by James L. Richards (3/24/1996).

   Last Update: James L. Richards (10/1/2007).
   Last Update: Marty J. Wolf (2/2/2016).

   --------------------------------------------------------------------

   Input (.dat) File Format:

   *parser,options    30-character title
   *tokens
   list of terminal symbols
   *grammar
   list of productions


   Options Following *parser
   =========================
   The options are specified as single characters. A specified option
   activates listing some parsing data in the human-readable .gen file.

   option
   ------
      f     Show first sets for each nonterminal in .gen file.
      g     Show augmented grammar in .gen file.
      m     Generate machine-readable tables in .tbl file.
      q     Just read input, do not create parsing tables.
      s     Show statistics in .gen file.
      t     Generate human-readable parsing information in .gen file.
      v     Show all terminal and nonterminal symbols in .gen file.
      x     Show symbol cross reference information in the .gen file.

   NOTE: The lines "*tokens" through "*grammar" are optional, but if 
         one is specified, the other must also be specified.

   Lines Following *tokens
   =======================
   Terminal symbols are bracketed by quotation marks. If a quotation 
   marks character must be placed in the symbol, it must be entered
   twice for each occurrence. Token names cannot exceed 30 characters.
   
   The empty production is represented by an empty string (i.e., "").

   Nonterminals are bracketed by less than and greater than signs;
   e.g., <token>.

   Semantic symbols are bracketed by square brackets; e.g., [action].

   Lines Following *grammar
   ========================
   The left hand side of a production must start in column one. The 
   right hand side is entered to the right of the left hand symbol, but
   must not extend past column 80. If the production does not fit on  
   one line, column one of the next line may be left blank and the 
   right hand side continued on that line. If an alternate right hand  
   side is to be entered, it may be entered on the same line by placing  
   an alternation meta-symbol (|) between the end of the previous right  
   hand side and the start of the new right hand side. It may also be 
   entered on another line by using the same left hand side symbol as a
   previous production. If a semantic symbol is to be entered in a 
   right hand side,it is placed at the point the appropriate action is 
   to occur. Semantic symbols have no effect on the table generation, 
   but they will appear in the grammar listing.


   Tables for a Parser Generator in the .tbl File
   ==============================================

   nsymbols     tnumber     snumber     pnumber
   numerically coded productions (pnumber lines)
   transition table (remaining lines)

   nsymbols  = number of nonterminal symbols.
   tnumber   = number of terminal symbols.
   snumber   = number of semantic symbols.
   pnumber   = number of productions.

   The right hand side symbols for each production are integers
   identifying terminal, nonterminals, and semantic symbols followed
   by the integer 1000 to mark the end of the production. Semantic 
   symbol numbers are negative integers. 

   Each transition table line consists of three integers (n, t, p)
   that may or may not be preceded by an asterisk. When there is no 
   asterisk present on the line, the triple (n, t, p) predicts that
   production number p should be used to replace nonterminal n if
   the next terminal symbol is is the one whose number is t. The
   presence of an asterisk means that at least two different
   productions are predicted by the same n and t combination and
   this implies that the grammar is not LL(1).

   --------------------------------------------------------------------
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

const int
	NAME_SIZE      = 30,   // number of significant characters in a name
	TABLE_SIZE     = 61,   // number of buckets in hash table
	MAX_SYMBOL     = 1000, // maximum number of symbols
	MAX_PRODUCTION = 1000, // maximum number of productions
	MAX_ALTERNATES = 1000, // maximum number of rhsides
	MAX_SEMANTIC   = 1000, // maximum semantic routine number
	MAX_RHS_LENGTH = 1000, // maximum number of symbols on rhs
	LINE_SIZE      = 81,   // number of characters in an input line
	PAGE_SIZE      = 57,   // number of lines per page
	MAX_COLUMN     = 80;   // maximum line printer column to use

const char ALTERNATION = '|';

enum SymbolType {TERMINAL, NONTERMINAL, SEMANTIC};
enum XrefType   {LHS, RHS};

struct TableNode;
typedef TableNode* TablePtr;

struct SymbolNode;
typedef SymbolNode* SymbolPtr;

struct XrefNode;
typedef XrefNode* XrefPtr;

struct RuleNode;
typedef RuleNode* RulePtr;

struct RhsNode;
typedef RhsNode* RhsPtr;

struct MatrixNode;
typedef MatrixNode* MatrixPtr;

struct TableNode   // entry in hash table
{
	string      symbolName;    // name associated with node
	int         number;        // number assigned to symbol
	int         digitCount;    // number of digits in name
	SymbolType  symbolKind;    // type of the symbol
	bool        used;          // true if symbol appeared in a rhs
	XrefPtr     xRefs[2];
	RulePtr     definition;    // definition if nonterminal
	SymbolPtr   firstSet;      // symbols first set
	SymbolPtr   followSet;     // symbols follow set
	TablePtr    next;          // next node in hash chain
};

struct XrefNode
{
	int     production;   // production number
	int     count;        // number of occurrences
	XrefPtr next;       
};

struct SymbolNode        // node in a list of symbols 
{
	TablePtr  symbol;
	SymbolPtr next;       // next symbol in list
};

struct HeaderNode
{
	SymbolPtr first,        // pointer to head of list
             last;         // pointer to tail
};

struct RuleNode // header node for a production
{
	bool      visited;      // true if rule already used
	int       count;        // number of right hand sides
	TablePtr  symbol;       // symbol on lhs of production
	RhsPtr    alternates;   // right hand sides
	RulePtr   next;         // next production in list
};

struct RhsNode // header node for production right hand sides
{
	int       number;       // production number
	SymbolPtr rightSide;    // actual right hand side
	int       count;        // number of symbols in rhs
	RhsPtr    next;         // next rhs in list
};

struct MatrixNode
{
	TablePtr  nonterm;
	TablePtr  term;
	int       prod;
	char      mark;
	MatrixPtr next;       
};

//      Global Variables
//      ****************

char        ch;          // current character of input

int         column;      // current column on output line

TablePtr    endMarker;   // $eof$

bool        endOfFile,   // true when eof reached
			   endOfLine;   // true when line exhausted

int         entNumber;   // number of non-zero entries in parse table

TablePtr    epsilon;     // NULL string

bool        errorFlag,   // true if grammar not LL(1)
			   fFlag;       // list first sets flag

string      fileName;

bool        gFlag;       // list augmented grammar flag

ifstream    grammar;     // input for the generator

bool        incomplete;  // existing non-terminal with no rhs

int         inputLine;   // number of current line of input

string      line;        // one line of input

ofstream    listing;     // human readable output

int         longestRhs;

MatrixPtr   matrixFirst,
			   matrixLast;

bool        mFlag;       // list machine readable tables

HeaderNode  nontermList; // list of nonterminal symbols

int         nSegments;   // number of array lengths needed for symbols

int         nTNumber;    // number of nonterminals

int         out;         // current position in line

bool        pFlag;       // list ancestors flag

RulePtr     productions; // grammar production list

bool        qFlag;       // quick option

int         rhsLength;   // total of all rhs lengths

int         rhsNumber;   // number of right hand sides

HeaderNode  semList;

bool        sFlag;       // list statistics flag

int         shortestRhs;

int         sNumber;

HeaderNode  symbols;     // list of all symbols in unsorted order

TablePtr    symbolTable[61];

ofstream    tables;      // machine readable parser tables

HeaderNode  termList;    // list of terminal symbols

bool        tFlag;       // list parsing tables flag

string      title;

int         tNumber;     // number of terminals

bool        vFlag,       // list vocabulary flag
            xFlag;       // list cross reference

void Error (int errType)
//  +---------------------------------------------------------------------+
//  |  Print error message and stop generating parsing table              |
//  +---------------------------------------------------------------------+
{
	switch (errType)
	{
	case 1: cout << " missing header line.";
			break;
	case 2: cout << " missing '>', ']', or '\"'.";
			break;
	case 3: cout << " illegal character detected.";
			break;
	case 4: cout << " missing production left hand side.";
			break;
	case 5: cout << " left hand sides must be nonterminal.";
			break;
	case 6: cout << " premature end of file encountered.";
			break;
	case 7: cout << " missing right hand side.";
			break;
	case 8: cout << " the NULL string may not be assigned a token"
				    << " number.";
			break;
	case 9: cout << " nonterminals may not be assigned token numbers.";
			break;
	default: cout << " unknown error.";
	}
	cout << "\n Error occurred on line " << inputLine << '.' << endl;
	exit(1); // terminate execution 
} // Error

SymbolPtr NewSymbol()
//  +---------------------------------------------------------------------+
//  |  Allocate and initialize a new symbol node.                         |
//  +---------------------------------------------------------------------+
{
	SymbolPtr ptr;

	ptr = new SymbolNode; // get a new node and initialize it
	ptr->symbol = NULL;
	ptr->next = NULL;
	return ptr;
} // NewSymbol

XrefPtr NewXref()
//  +---------------------------------------------------------------------+
//  |  Initialize a new cross reference node.                             |
//  +---------------------------------------------------------------------+
{
	XrefPtr ptr;

	ptr = new XrefNode;
	ptr->production = 0;
	ptr->count = 1;
	ptr->next = NULL;
	return ptr;
} // NewXref

RulePtr NewRule()
//  +---------------------------------------------------------------------+
//  |  Return address of newly allocated RuleNode.                        |
//  +---------------------------------------------------------------------+
{
	RulePtr ptr;

	ptr = new RuleNode;  // allocate node and initialize it
	ptr->visited = false;
	ptr->count = 1;
	ptr->symbol = NULL;
	ptr->alternates = NULL;
	ptr->next = NULL;
	return ptr;
} // NewRule

RhsPtr NewRhs()
//  +---------------------------------------------------------------------+
//  |  Allocate and initialize a new RhsNode.                             |
//  +---------------------------------------------------------------------+
{
	RhsPtr ptr;

	ptr = new RhsNode; // get a new node and initialize it
	ptr->number = 0;
	ptr->rightSide = NULL;
	ptr->count = 0;
	ptr->next = NULL;
	return ptr;
} // NewRhs

void GetCh()
//  +---------------------------------------------------------------------+
//  |  Get next character from line.                                      |
//  +---------------------------------------------------------------------+
{
	if (out == line.length()) // there is no more data in line
	{
		ch = ' '; // fake a blank for end of line
		endOfLine = true;
	}
	else //  place current character in global ch
	{  
		ch = line[out];
		out++;
	}
} // GetCh

void GetLine()
//  +---------------------------------------------------------------------+
//  |  Read the next line from input.  If there is no next line, set      |
//  |  endOfFile to true.                                                 |
//  +---------------------------------------------------------------------+
{
	out = 0; // initialize empty line 
	getline(grammar, line);
	if (grammar.eof()) // all input read
	{
		endOfLine = true;
		endOfFile = true;
	}
	else // set line now has data in it
	{
		endOfLine = false; 
		inputLine++;
	}
	GetCh();
} // GetLine

void ReadHeader()
//  +---------------------------------------------------------------------+
//  |  Read and decode '*parser,options' line.                            |
//  +---------------------------------------------------------------------+
{
	string header;
 
	header = "";
 	for (int i = 0; i <= 6; i++) // get characters from line into header
	{
      header += ch;
		GetCh();
	}
	if (header != "*parser") Error(1);
	GetCh(); // skip delimiter preceeding options
	while (isalpha(ch)) //  get LLGEN options 
	{
      switch (ch)
	   {
	   case 'f': fFlag = true; break;
	   case 'g': gFlag = true; break;
	   case 'm': mFlag = true; break;
	   case 'q': qFlag = true; break;
	   case 's': sFlag = true; break;
	   case 't': tFlag = true; break;
	   case 'v': vFlag = true; break;
	   case 'x': xFlag = true; break;
	   default:; // do nothing
	   }
		GetCh();
	}
	size_t n = line.find_first_not_of(' ', out);
	if (n == string::npos)
      title = "";
	else
      title = line.substr(n, line.length());

	GetLine(); // force the reading of line two
} // ReadHeader

int Hash (string name)
//  +---------------------------------------------------------------------+
//  |  Return hashed value of Name, 1 <= Hash(Name) <= TABLE_SIZE.        |
//  +---------------------------------------------------------------------+
{
	int number[3];

	while (name.length() < 6) name += ' ';
	for (int i = 0; i < 3; i++)
		number[i] = 256*int(name[2*i]) + int(name[2*i+1]);
	number[0] = abs(number[0] + number[1] + number[2]);
	number[1] = number[0] / 9999;  
	number[2] = (number[0] + number[1]) % 9999;
	return (number[2] % TABLE_SIZE);
} // Hash

void AddSymbol(TablePtr tablePtr, HeaderNode& list)
//  +---------------------------------------------------------------------+
//  |  Add a new symbol to list.                                          |
//  +---------------------------------------------------------------------+
{
	if (list.first == NULL)  // add the first symbol to the list
	{
		list.first = NewSymbol(); // get a symbol
		list.last = list.first;
		list.first->symbol = tablePtr;
	}
	else // add a symbol to the end of the list
	{ 
		list.last->next = NewSymbol();
		list.last = list.last->next;
		list.last->symbol = tablePtr;
	}
} // AddSymbol

TablePtr EnterId (int count, string name, SymbolType kind)
//  +---------------------------------------------------------------------+
//  |  Place name in symbol table.                                        |
//  +---------------------------------------------------------------------+
{
	int      i;                
   TablePtr ptr1, ptr2;  

	i = Hash(name);         // pick a bucket of the hash table
	ptr1 = symbolTable[i];  // search that chain of the hash table
	ptr2 = NULL;            // set to address of node if match found
	while (ptr1 != NULL && ptr2 == NULL)  // search chain
		if (ptr1->symbolName == name 
				&& ptr1->digitCount == count 
				&& ptr1->symbolKind == kind)
			ptr2 = ptr1; // name found
		else
			ptr1 = ptr1->next;  // try the next node in the chain

	if (ptr2 == NULL)   // the symbol was not already defined
    { // define it
		ptr2 = new TableNode;  // get a new node 
		ptr2->symbolName = name;
		ptr2->digitCount = count;
		ptr2->symbolKind = kind;
		ptr2->used = false;
		ptr2->xRefs[LHS] = NULL;
		ptr2->xRefs[RHS] = NULL;
		ptr2->definition = NULL;
		ptr2->firstSet = NULL;
		ptr2->followSet = NULL;
		ptr2->next = symbolTable[i]; // insert node into chain
 
		symbolTable[i] = ptr2;
		if (kind == TERMINAL)
		{
			if (count != 0)
			{
				AddSymbol(ptr2, termList);
				tNumber++;
				ptr2->number = tNumber;
				ptr2->used = true;
			}
		}
		else if (kind == SEMANTIC)
		{
			AddSymbol(ptr2, semList);
			sNumber++;
			ptr2->number = sNumber;
			ptr2->used = true;
		}
		else
			AddSymbol(ptr2, nontermList);
	}
	return ptr2;
} // EnterId

TablePtr ReadSymbol()
//  +---------------------------------------------------------------------+
//  |  Read one symbol from input.                                        |
//  +---------------------------------------------------------------------+
{
	int        count;  // number of characters in name
	string     name;   // name of symbol read
	SymbolType kind;   // type of symbol read

	count = 0;
	name = "";
	if (ch == '<')  // nonterminal found
	{
		kind = NONTERMINAL;
		GetCh(); // skip < 
		while (ch != '>' && !endOfLine)
		{ // get symbol Name
			if (count < NAME_SIZE)
			{// add the new character to name
				count++;
				name += ch;
			}
			GetCh();
		}
		if (ch == '>') // skip >
			GetCh();
		else
			Error(2);
	}
	else if (ch == '[') // semantic found
	{
        kind = SEMANTIC;
        GetCh(); // skip [
        while (ch != ']'&& !endOfLine)
		{ // get symbol Name }
			if (count < NAME_SIZE)
			{ // add the new character to name 
				count++;
                name += ch;
			}
			GetCh();
		}
        if (ch == ']') // skip ]
			GetCh();
		else
			Error(2);
	}

	else if (ch == '"') //  terminal found
	{
		kind = TERMINAL;
		do
		{
			GetCh(); // skip open quote
			while (ch != '"' && !endOfLine)
			{ // get the symbol Name
				if (count < NAME_SIZE)
				{
					count++;
					name += ch;
				}
				GetCh();
			}
			if (ch == '"') // skip close quote
				GetCh();
			else
				Error(2);
			if (ch == '"' && count < NAME_SIZE)
			{  // "" found 
				count++;
				name[count] = ch;
			}
		} while (ch == '"');
	}
	else
		Error(3);
	return EnterId(count, name, kind);
} // ReadSymbol

void ReadTokens()
//  +---------------------------------------------------------------------+
//  |  Read token value definitions.                                      |
//  +---------------------------------------------------------------------+
{
	string   command;   // command line characters
	TablePtr ptr;       // pointer to token
	string   name;      // name representing end of file

	command = "";  // clear command characters
	for (int i = 1; i <= 7; i++) // check for *tokens command
	{
		command += ch;  // accumulate characters from line
		GetCh();
	}
	if (command != "*tokens") // token values not assigned
	{
		if (endOfFile) Error(6);
		out = 0; // reset to beginning of line
		endOfLine = false;
		GetCh();
	}
	else // loop through token definitions
	{ 
		GetLine(); // get first line of token assignments
		command = "";
		for (int i = 1; i <= 8; i++)
		{
			command += ch;
			GetCh();
		}

		while (command != "*grammar")
		{
			if (endOfFile) Error(6);
			out = 0; // start at beginning of line 
			endOfLine = false;
			GetCh();
			while ((ch == ' ' || ch == '\011') && !endOfLine)
				GetCh();
			ptr = ReadSymbol(); // get symbol being defined
			if (ptr->digitCount == 0) Error(8);
			if (ptr->symbolKind == NONTERMINAL) Error(9);
			GetLine();  // load the next line
			command = "";
			for (int i = 1; i <= 8; i++)
			{
				command += ch;
				GetCh();
			}
		}
		GetLine();
	}
	name = "$eof$";
	endMarker = EnterId(5, name, TERMINAL);
} // ReadTokens

void ReadGrammar()
//  +---------------------------------------------------------------------+
//  |  Read the grammar productions from input.                           |
//  +---------------------------------------------------------------------+
{
   RulePtr   lastRulePtr; // last rule in list
   TablePtr  tablePtr;    // lhs symbol
   RulePtr   curRulePtr;  // current rule
   RhsPtr    rhsPtr;      // last rhs
   SymbolPtr symbolPtr;   // rhs symbol

	lastRulePtr = productions;
	while (!endOfFile) // read the grammar
	{
		tablePtr = ReadSymbol(); // get lhs symbol
		if (tablePtr->symbolKind != NONTERMINAL)
			Error(5); // lhs must be nonterminal
		curRulePtr = tablePtr->definition; // see if already defined
		if (curRulePtr == NULL) // this is a new lhs }
		{ // define new production }
			lastRulePtr->next = NewRule();
			lastRulePtr = lastRulePtr->next;
			lastRulePtr->symbol = tablePtr;
			tablePtr->definition = lastRulePtr;
			curRulePtr = lastRulePtr;
			rhsPtr = NewRhs();
			curRulePtr->alternates = rhsPtr;
		}
		else
		{
			rhsPtr = curRulePtr->alternates;
			while (rhsPtr->next != NULL) // tack new rhs to end of alternates
				rhsPtr = rhsPtr->next; // find end of list 
			rhsPtr->next = NewRhs();
			rhsPtr = rhsPtr->next;
			curRulePtr->count = curRulePtr->count + 1;
		}

		do // get lines of right hand sides
		{
			do // get symbols from line
			{
				while ((ch == ' ' || ch == '\011') && !endOfLine)
					GetCh(); // skip blanks
				if (!endOfLine) // get a symbol
					if (ch == ALTERNATION)  // start a new rhs
					{
						if (rhsPtr->rightSide == NULL) // NULL rhs
							Error(7);
						rhsPtr->next = NewRhs(); // add a new rhs
						rhsPtr = rhsPtr->next;
						curRulePtr->count = curRulePtr->count + 1;
						GetCh(); // skip /
					}
					else
					{ // add symbol to current rhs
						if (rhsPtr->rightSide == NULL) // first rhs symbol
						{
							symbolPtr = NewSymbol();
							rhsPtr->rightSide = symbolPtr;
						}
						else
						{ // add symbol to end of rhs
							symbolPtr->next = NewSymbol();
							symbolPtr = symbolPtr->next;
						}
						symbolPtr->symbol = ReadSymbol();
						symbolPtr->symbol->used = true;
						if (symbolPtr->symbol->digitCount != 0)
							rhsPtr->count = rhsPtr->count + 1;
					}
			} while (!endOfLine);
			GetLine(); // advance to next line
		} while ((ch == ' ' || ch == '\011') && !endOfFile);
		if (rhsPtr->rightSide == NULL) // NULL rhs
			Error(7);
	}
} // ReadGrammar

void Sort (SymbolPtr ptr1, int count)
//  +---------------------------------------------------------------------+
//  |  Sort List of symbol pointers into nondecreasing order.             |
//  +---------------------------------------------------------------------+
{
	SymbolPtr ptr2;   // current symbol in list while searching
	SymbolPtr ptr3;   // minimum symbol remaining
	SymbolNode temp;  // temporary used to switch two list entries
	bool flag;        // temporary 

	for (int i = 1; i <= count; i++) // sort list entries
	{
		ptr2 = ptr1; // set current list entry
		ptr3 = ptr1; // assume current entry is the smallest
		for (int j = i + 1; j <= count; j++) // search for smallest entry
		{
			ptr2 = ptr2->next; // advance to next entry
			flag = ptr2->symbol->symbolName < ptr3->symbol->symbolName;
			if (flag) ptr3 = ptr2; // new smallest found
		}
		temp.symbol = ptr1->symbol; // switch current entry with smallest}
		ptr1->symbol = ptr3->symbol;
		ptr3->symbol = temp.symbol;
		ptr1 = ptr1->next;
	}
} // Sort

void CompleteGrammar()
//  +---------------------------------------------------------------------+
//  |  Augment grammar, count and number symbols, and check that all      |
//  |  nonterminals have at least one rhs.                                |
//  +---------------------------------------------------------------------+
{
	SymbolPtr symbolPtr; // temporary
	RulePtr   rulePtr;   // current rule
	RhsPtr    rhsPtr;    // current rhs in rule

	symbolPtr = NewSymbol();
	symbolPtr->symbol = productions->next->symbol;
	symbolPtr->symbol->used = true;
	symbolPtr->next = NewSymbol();
	symbolPtr->next->symbol = endMarker;
	productions->alternates = NewRhs();
	productions->alternates->count = 2;
	productions->alternates->rightSide = symbolPtr;
	rulePtr = productions; // trace productions list

	while (rulePtr != NULL)
	{
		rhsPtr = rulePtr->alternates; // trace through all rh sides
		while (rhsPtr != NULL)
		{
			rhsLength = rhsLength + rhsPtr->count; // accum. total length
			if (rhsPtr->count > longestRhs)
				longestRhs = rhsPtr->count;
			if (rhsPtr->count < shortestRhs)
				shortestRhs = rhsPtr->count;
			rhsNumber++; // increment number of rh sides
			rhsPtr->number = rhsNumber; // give this rh side its number
			rhsPtr = rhsPtr->next;
		}
		rulePtr = rulePtr->next;
	}
	symbolPtr = nontermList.first; // count nonterminal symbols
	while (symbolPtr != NULL)
	{
		nTNumber++;
		if (symbolPtr->symbol->definition == NULL)
			incomplete = true;
		symbolPtr->symbol->number = tNumber + nTNumber;
		symbolPtr = symbolPtr->next;
	}
	symbolPtr = termList.first; // add terminals to total symbol list

	while (symbolPtr != NULL)
	{
		AddSymbol(symbolPtr->symbol, symbols); // add one symbol to list
		symbolPtr = symbolPtr->next;
	}
	symbolPtr = nontermList.first; // add nonterminals to total symbol list
	while (symbolPtr != NULL)
	{
		AddSymbol(symbolPtr->symbol, symbols); // add one symbol to list
		symbolPtr = symbolPtr->next;
	}
	symbolPtr = semList.first; // add semantics to total symbol list }
	while (symbolPtr != NULL)
	{
		AddSymbol(symbolPtr->symbol, symbols);
		symbolPtr = symbolPtr->next;
	}
	Sort(termList.first, tNumber);
	Sort(nontermList.first, nTNumber);
	Sort(semList.first, sNumber);
} // CompleteGrammar

void SetXref (TablePtr tablePtr, int prodNo, XrefType kind)
//  +---------------------------------------------------------------------+
//  |  Set reference to symbol tablePtr in production prodNo.             |
//  +---------------------------------------------------------------------+
{
   XrefPtr xRefPtr; // cross reference List

   if (tablePtr->xRefs[kind] == NULL)
   {
      tablePtr->xRefs[kind] = NewXref(); // set initial reference
      tablePtr->xRefs[kind]->production = prodNo;
   }
   else
   {
      xRefPtr = tablePtr->xRefs[kind];
      while (xRefPtr->next != NULL) // find the end of the xref List 
         xRefPtr = xRefPtr->next;
      if (xRefPtr->production != prodNo)
		{
         xRefPtr->next = NewXref(); // add new reference to end
         xRefPtr->next->production = prodNo;
		}
      else
         xRefPtr->count = xRefPtr->count + 1;
	}
} // SetXref

void CrossRef()
//  +---------------------------------------------------------------------+
//  |  Set cross reference for all symbols in grammar.                    |
//  +---------------------------------------------------------------------+
{
	RulePtr   rulePtr;   // current rule node 
	RhsPtr    rhsPtr;    // current rhs of rulePtr
	SymbolPtr symbolPtr; // current symbol of rhsPtr

	rulePtr = productions;  // start with initial production
	while (rulePtr != NULL) // move through production list
	{
		rhsPtr = rulePtr->alternates;
		while (rhsPtr != NULL) // move through all rhsides
		{
			SetXref(rulePtr->symbol, rhsPtr->number, LHS); // set lhs reference
			symbolPtr = rhsPtr->rightSide;
			while (symbolPtr != NULL) // move through each symbol of rhs
			{
				SetXref(symbolPtr->symbol, rhsPtr->number, RHS);
				symbolPtr = symbolPtr->next;
			}
			rhsPtr = rhsPtr->next;
		}
		rulePtr = rulePtr->next;
	}
} // CrossRef

void AddFirst(TablePtr tablePtr1, TablePtr tablePtr2, bool& flag)
//  +---------------------------------------------------------------------+
//  |  Add a symbol to the first set if it is not already there.          |
//  +---------------------------------------------------------------------+
{
   SymbolPtr symbolPtr1; // first set of tablePtr1 
   SymbolPtr symbolPtr2; // pointer to tablePtr2 symbol if found in 
                         // symbolPtr1 

   if (tablePtr1->firstSet == NULL) // add first symbol to the first set
	{
      tablePtr1->firstSet = NewSymbol();
      tablePtr1->firstSet->symbol = tablePtr2;
      flag = false; // indicate that a symbol has been added
   }
   else
	{ // determine if the symbol is already in the set
      symbolPtr1 = tablePtr1->firstSet;
		symbolPtr2 = NULL;
		while (symbolPtr1 != NULL && symbolPtr2 == NULL) // search the set
			if (symbolPtr1->symbol == tablePtr2)
				// the symbol is already in the set
				symbolPtr2 = symbolPtr1;
			else // try the next symbol in the set
				symbolPtr1 = symbolPtr1->next;
      if (symbolPtr2 == NULL) // the symbol must be added to the set
		{
         symbolPtr2 = NewSymbol();
         symbolPtr2->symbol = tablePtr2;
         symbolPtr2->next = tablePtr1->firstSet;
         tablePtr1->firstSet = symbolPtr2;
         flag = false;
		}
	}
} // AddFirst

void ComputeFirst()
//  +---------------------------------------------------------------------+
//  |  Compute first sets for all nonterminal symbols.                    |
//  +---------------------------------------------------------------------+
{
   SymbolPtr symbolPtr1; // symbol for which first set is being computed
   RhsPtr    rhsPtr;     // rhs of definition of symbolPtr1
   SymbolPtr symbolPtr2; // symbol of rhsPtr
   SymbolPtr symbolPtr3; // first set of symbolPtr2
   SymbolPtr symbolPtr4; // spare/working Ptr for symbolPtr2
   bool      eFlag, 
             fFlag, 
             done; // termination flags

	symbolPtr1 = termList.first;
	while (symbolPtr1 != NULL)  // set first(terminal) = [terminal]
	{
		AddFirst(symbolPtr1->symbol, symbolPtr1->symbol, eFlag);
		symbolPtr1 = symbolPtr1->next;
	}
	AddFirst(epsilon, epsilon, eFlag);
	symbolPtr1 = nontermList.first;
	while (symbolPtr1 != NULL)
	{
		if (symbolPtr1->symbol->definition == NULL 
            && symbolPtr1->symbol->symbolKind != SEMANTIC)
			AddFirst(symbolPtr1->symbol, symbolPtr1->symbol, eFlag);
      else if (symbolPtr1->symbol->definition != NULL)
		{
         rhsPtr = symbolPtr1->symbol->definition->alternates;
			while (rhsPtr != NULL)
			{
				symbolPtr2 = rhsPtr->rightSide;
				eFlag = true;
				while (eFlag)
				if (symbolPtr2 == NULL)
					eFlag = false;
				else if (symbolPtr2->symbol->symbolKind == SEMANTIC)
					symbolPtr2 = symbolPtr2->next;
				else
					eFlag = false;
				if (symbolPtr2 != NULL)
					if (symbolPtr2->symbol->definition == NULL)
						AddFirst(symbolPtr1->symbol, symbolPtr2->symbol, eFlag);
                rhsPtr = rhsPtr->next;
			}
		}
		symbolPtr1 = symbolPtr1->next;
	}
	do
	{
		done = true; // assume no new symbols will be added }
		symbolPtr1 = nontermList.first;
		while (symbolPtr1 != NULL) // attempt to propagate first sets
		{
			if (symbolPtr1->symbol->definition != NULL)
            // add symbols from rhs nonterminals }
			{
				rhsPtr = symbolPtr1->symbol->definition->alternates;
				while (rhsPtr != NULL) // check all alternate rh sides
				{
					symbolPtr2 = rhsPtr->rightSide;
               do // check each symbol of rhs
					{
						eFlag = true;
						while (eFlag)
							if (symbolPtr2 == NULL)
								eFlag = false;
							else if (symbolPtr2->symbol->symbolKind == SEMANTIC)
								symbolPtr2 = symbolPtr2->next;
							else
								eFlag = false;
						if (symbolPtr2 != NULL)
							symbolPtr3 = symbolPtr2->symbol->firstSet;
						else
							symbolPtr3 = NULL;
						eFlag = true;
						// assume the set does not contain epsilon

						while (symbolPtr3 != NULL) // copy first set to lhs symbol
						{
							if (symbolPtr3->symbol->digitCount != 0)
								AddFirst(symbolPtr1->symbol, 
                                 symbolPtr3->symbol, done);
							else
							{
								eFlag = false; // indicate epsilon found
								if (symbolPtr2->next == NULL)
									AddFirst(symbolPtr1->symbol, epsilon, done);
								else if (symbolPtr2->next->symbol->symbolKind ==
                                     SEMANTIC)
								{
									symbolPtr4 = symbolPtr2;
									fFlag = false;
									do
									{
										symbolPtr4 = symbolPtr4->next;
										if (symbolPtr4->next == NULL)
										{
											AddFirst(symbolPtr1->symbol,epsilon,
												     done);
											fFlag = true;
										}
										else
											if (symbolPtr4->next->symbol->symbolKind
												!= SEMANTIC)
												fFlag = true;
									} while (!fFlag);
								}
							}
							symbolPtr3 = symbolPtr3->next;
						}
						if (symbolPtr2 != NULL)
							symbolPtr2 = symbolPtr2->next;
               } while (symbolPtr2 != NULL && !eFlag);
               rhsPtr = rhsPtr->next;
				}
			}
         symbolPtr1 = symbolPtr1->next;
      }
   } while (!done);
} // ComputeFirst

void AddFollow (TablePtr tablePtr1, TablePtr tablePtr2, bool& flag)
//  +---------------------------------------------------------------------+
//  |  Add a symbol to the follow set if it is not already there.         |
//  +---------------------------------------------------------------------+
{
   SymbolPtr symbolPtr1; // follow set of Ptr1
   SymbolPtr symbolPtr2; // pointer to Ptr2 symbol if found in Ptr3

	if (tablePtr1->followSet == NULL)  // add follow symbol to follow set
	{
		tablePtr1->followSet = NewSymbol();
		tablePtr1->followSet->symbol = tablePtr2;
		flag = false;  // indicate that a symbol has been added
	}
	else
	{ // determine if the symbol is already in the set
		symbolPtr1 = tablePtr1->followSet;
		symbolPtr2 = NULL;
		while (symbolPtr1 != NULL && symbolPtr2 == NULL)  // search the set
			if (symbolPtr1->symbol == tablePtr2)  // symbol already in the set 
				symbolPtr2 = symbolPtr1;
			else // try the next symbol in the set
				symbolPtr1 = symbolPtr1->next;
		if (symbolPtr2 == NULL) // the symbol must be added to the set
		{
			symbolPtr2 = NewSymbol();
			symbolPtr2->symbol = tablePtr2;
			symbolPtr2->next = tablePtr1->followSet;
			tablePtr1->followSet = symbolPtr2;
			flag = false;
		}
	}
} // AddFollow

void ComputeFollow()
//  +---------------------------------------------------------------------+
//  |  Compute follow sets for all nonterminal symbols.                   |
//  +---------------------------------------------------------------------+
{
   SymbolPtr symbolPtr1;  // symbol for which rhs is being scanned
   RhsPtr    rhsPtr;      // rhs definition of symbolPtr1
   SymbolPtr symbolPtr2;  // rhsPtr for which follow set's computed
   SymbolPtr symbolPtr3;  // symbol following symbolPtr2
   SymbolPtr symbolPtr4;  // first set of symbolPtr3
   SymbolPtr symbolPtr5;  // follow set of symbolPtr1
   bool      done, 
             eFlag, 
             looking;     // termination flags

	AddFollow(productions->symbol, endMarker, done);
	do
	{
		done = true;
		symbolPtr1 = nontermList.first;
		while (symbolPtr1 != NULL)
		{
			if (symbolPtr1->symbol->definition != NULL)
			{
				rhsPtr = symbolPtr1->symbol->definition->alternates;
				while (rhsPtr != NULL)
				{
					symbolPtr2 = rhsPtr->rightSide;
					do
					{
						if (symbolPtr2->symbol->symbolKind == NONTERMINAL)
						{
							symbolPtr3 = symbolPtr2->next;
							looking = symbolPtr3 != NULL;
							while (looking)
								if (symbolPtr3->symbol->symbolKind == SEMANTIC)
								{
									symbolPtr3 = symbolPtr3->next;
									looking = symbolPtr3 != NULL;
								}
								else
									looking = false;
							if (symbolPtr3 == NULL)
							{
								symbolPtr5 = symbolPtr1->symbol->followSet;
								while (symbolPtr5 != NULL)
								{
									AddFollow(symbolPtr2->symbol,symbolPtr5->symbol,
									          done);
									symbolPtr5 = symbolPtr5->next;
								}
							}
							else
							{
								do
								{
									eFlag = true;
									symbolPtr4 = symbolPtr3->symbol->firstSet;
									while (symbolPtr4 != NULL)
									{
										if (symbolPtr4->symbol->digitCount != 0)
											AddFollow(symbolPtr2->symbol,
										          symbolPtr4->symbol,
												  done);
										else
										{
											eFlag = false;
											if (symbolPtr3->next == NULL)
											{
												symbolPtr5 = 
                                       symbolPtr1->symbol->followSet;
												while (symbolPtr5 != NULL)
												{
													AddFollow(symbolPtr2->symbol,
														symbolPtr5->symbol,done);
													symbolPtr5 = symbolPtr5->next;
												}
											}
										}
										symbolPtr4 = symbolPtr4->next;
									} // while
									symbolPtr3 = symbolPtr3->next;
									looking = symbolPtr3 != NULL;
									while (looking)
										if (symbolPtr3->symbol->symbolKind
											== SEMANTIC)
										{
											symbolPtr3 = symbolPtr3->next;
											looking = symbolPtr3 != NULL;
										}
										else
											looking = false;
								} while (symbolPtr3 != NULL && !eFlag);
							}
						} // if
						symbolPtr2 = symbolPtr2->next;
					} while (symbolPtr2 != NULL);
					rhsPtr = rhsPtr->next;
				} // while
			} // if
			symbolPtr1 = symbolPtr1->next;
		} // while
	} while (!done);
} // ComputeFollow

void AddTable (TablePtr tablePtr1, TablePtr tablePtr2, int prodNum)
//  +---------------------------------------------------------------------+
//  |  Add an entry to the parse table at nonterminal tablePtr1 and       |
//  |  and terminal tablePtr2.                                            |
//  +---------------------------------------------------------------------+
{
	MatrixPtr matrixPtr;

	matrixPtr = new MatrixNode;
	matrixPtr->nonterm = tablePtr1;
	matrixPtr->term = tablePtr2;
	matrixPtr->prod = prodNum;
	matrixPtr->mark = ' ';
	matrixPtr->next = NULL;
	matrixLast->next = matrixPtr;
	matrixLast = matrixPtr;
	entNumber++;
} // AddTable

void ComputeParseTable()
//  +---------------------------------------------------------------------+
//  |  Compute the LL(1) parse tables.                                    |
//  +---------------------------------------------------------------------+
{
   RulePtr   rulePtr;
   RhsPtr    rhsPtr;
   SymbolPtr symbolPtr1;
   SymbolPtr symbolPtr2;
   SymbolPtr symbolPtr3;
   bool      eFlag,
             fFlag;

	rulePtr = productions->next;
	while (rulePtr != NULL)
	{
		rhsPtr = rulePtr->alternates;
		while (rhsPtr != NULL)
		{
			symbolPtr1 = rhsPtr->rightSide;
			do
			{
				eFlag = symbolPtr1->symbol->symbolKind != SEMANTIC;
				symbolPtr2 = symbolPtr1->symbol->firstSet;

				while (symbolPtr2 != NULL)
				{
					if (symbolPtr2->symbol->digitCount != 0)
						AddTable(rulePtr->symbol, symbolPtr2->symbol, 
								rhsPtr->number);
					else
					{
						eFlag = false;
						fFlag = true;
						while (fFlag)
							if (symbolPtr1->next == NULL)
								fFlag = false;
							else if (symbolPtr1->next->symbol->symbolKind 
									== SEMANTIC)
								symbolPtr1 = symbolPtr1->next;
							else
								fFlag = false;
						if (symbolPtr1->next == NULL)
						{
							symbolPtr3 = rulePtr->symbol->followSet;
							while (symbolPtr3 != NULL)
							{
								AddTable(rulePtr->symbol,symbolPtr3->symbol,
										rhsPtr->number);
								symbolPtr3 = symbolPtr3->next;
							}
						}
					}
					symbolPtr2 = symbolPtr2->next;
				}
				symbolPtr1 = symbolPtr1->next;
			} while (symbolPtr1 != NULL && !eFlag);
			rhsPtr = rhsPtr->next;
		}
		rulePtr = rulePtr->next;
	} // while
}  // ComputeParseTable

void WriteTitle(const string& subtitle)
//  +---------------------------------------------------------------------+
//  |  Write title lines.                                                 |
//  +---------------------------------------------------------------------+
{
   listing << "\n\n  " << string(title.length()+subtitle.length()+9, '~')
           << "\n  ~~  " << title << " " << subtitle << "  ~~"
           << "\n  " << string(title.length()+subtitle.length()+9, '~')
           << endl << endl;
   column = 1;
} // WriteTitle

void Statistics()
//  +---------------------------------------------------------------------+
//  |  List statistics on grammar.                                        |
//  +---------------------------------------------------------------------+
{
	WriteTitle("Statistics");
	listing << setw(5) << rhsNumber << " productions\n"
	        << setw(5) << tNumber << " terminal symbols\n"
	        << setw(5) << nTNumber << " nonterminal symbols\n"
			  << setw(5) << sNumber << " semantic (action) symbols\n"
	        << "\n  The longest right hand side contains " 
           << longestRhs << " symbols.\n"
	        << "  The shortest right hand side contains " 
           << shortestRhs << " symbols.\n"
	        << endl;
	if (!qFlag)
		listing << setw(5) << "  There are " << entNumber 
              << " non-zero parse table entries." << endl;
} // Statistics

void WriteSymbol (TablePtr tablePtr)
//  +---------------------------------------------------------------------+
//  |  Write one symbol to the human-readable listing file.               |
//  +---------------------------------------------------------------------+
{
	if (tablePtr->symbolKind == TERMINAL)
	{
		listing << " \"";
		listing << tablePtr->symbolName;
		listing << '"';
	}
	else if (tablePtr->symbolKind == SEMANTIC)
	{
		listing << " [";
		listing << tablePtr->symbolName;
		listing << ']';
	}	
	else
	{
		listing << " <";
		listing << tablePtr->symbolName;
		listing << '>';
	}
	column = column + tablePtr->digitCount + 3;	
} // WriteSymbol

void ListBadNonterms()
//  +---------------------------------------------------------------------+
//  |  List out all nonterminals that do not have at least one rhs.       |
//  +---------------------------------------------------------------------+
{
	SymbolPtr symbolPtr;

	WriteTitle("Bad Non-terminals");
	listing << " The following List of non-terminals have no "
			<< "right hand sides:" << endl << endl;
	symbolPtr = nontermList.first;
	while (symbolPtr != NULL)
	{
		if (symbolPtr->symbol->definition == NULL)
		{
         listing << "     " << symbolPtr->symbol->number << ".  ";
			WriteSymbol(symbolPtr->symbol);
			listing << endl;
		}
		symbolPtr = symbolPtr->next;
	}
} // ListBadNonterms

void MWriteSymbol (TablePtr tablePtr)
//  +---------------------------------------------------------------------+
//  | Insert one symbol into the tables stream.                           |
//  +---------------------------------------------------------------------+
{
   tables << setw(10) << tablePtr->number 
          << "  " << tablePtr->symbolName << endl;
} // MWriteSymbol

void CheckColumn (SymbolPtr symbolPtr, int length)
//  +---------------------------------------------------------------------+
//  |  Ensure that the current output line will not wrap.                 |
//  +---------------------------------------------------------------------+
{
	if (MAX_COLUMN < column + symbolPtr->symbol->digitCount + 3)
	{
		listing << endl; // terminate Line
		listing << setw(length+3) << ' '; // write continuation spaces
		column = length + 3;
	}
} // CheckColumn

void WriteGrammar()
//  +---------------------------------------------------------------------+
//  |  Print formatted grammar productions                                |
//  +---------------------------------------------------------------------+
{
	int longest;          // length of longest lhs
	RulePtr   rulePtr;    // current rule
	RhsPtr    rhsPtr;     // current rhs in rulePtr
	SymbolPtr symbolPtr;  // current symbol in rhsPtr

	WriteTitle("Augmented Productions");
	longest = 0; // find longest lhs
	rulePtr = productions; // trace production list
	while (rulePtr != NULL)
    { // check character count of symbol
		if (rulePtr->symbol->digitCount > longest)  //  new longest found
			longest = rulePtr->symbol->digitCount;
		rulePtr = rulePtr->next; // check next production
	}
	rulePtr = productions; // now list the grammar 
	while (rulePtr != NULL) // trace through production list
	{
		rhsPtr  =  rulePtr->alternates; // write the rh sides
		while (rhsPtr != NULL)
		{
			listing << '\n' << setw(6) << rhsPtr->number << ".  ";
			WriteSymbol(rulePtr->symbol); // write the lhs
			listing << setw(longest-rulePtr->symbol->digitCount+1) << ' ' 
					<< "-->";
			column = longest - rulePtr->symbol->digitCount + 20;
			symbolPtr = rhsPtr->rightSide;
			while (symbolPtr != NULL)  // write one rhs
			{
				CheckColumn(symbolPtr, longest + 20);
				WriteSymbol(symbolPtr->symbol);
				symbolPtr = symbolPtr->next;
			}
			listing << endl;
			rhsPtr = rhsPtr->next;
		}
		rulePtr = rulePtr->next;
	}
} // WriteGrammar

void Encode (int intValue, string& digits)
//  +---------------------------------------------------------------------+
//  |  Create a string representation for intValue.                       |
//  +---------------------------------------------------------------------+
{
	while (intValue != 0)
	{
      digits = char((intValue % 10) + int('0')) + digits;
      intValue /= 10;
	}
} // Encode

void WriteXrefs (TablePtr tablePtr1, XrefType kind, int length)
//  +---------------------------------------------------------------------+
//  |  Display cross references for the specified kind of symbol.         |
//  +---------------------------------------------------------------------+
{
   XrefPtr xRefPtr; // current cross reference in tablePtr1
   string  digits,
           digits2;

	xRefPtr = tablePtr1->xRefs[kind]; // start of cross reference list
	while (xRefPtr != NULL) // print reference
	{
		if (MAX_COLUMN < column + 5)
		{
         listing << endl; // terminate current line
         listing << setw(length+3) << ' ';
         // space to beginning of next line }
         column = length + 3;
		}
		digits = ""; // clear digits
		Encode(xRefPtr->production, digits); // encode prod. number
		if (xRefPtr->count != 1)  // Encode repetition count
		{
			Encode(xRefPtr->count, digits2);
			digits2 += "*";
		}
		else
			digits2 = "";
		listing << setw(5) << digits2 + digits;
		column = column + 5;
		xRefPtr = xRefPtr->next;
	}
} // WriteXrefs

void WriteList (SymbolPtr symbolListPtr)
//  +---------------------------------------------------------------------+
//  |  Write one of the vocabulary lists.                                 |
//  +---------------------------------------------------------------------+
{
   SymbolPtr curSymbolPtr; // current symbol in list
   int       longest; // length of longest symbol in list

	if (xFlag)
	{
		curSymbolPtr = symbolListPtr; // find longest symbol in list
		longest = 0;
		while (curSymbolPtr != NULL) // search list
		{
			if (curSymbolPtr->symbol->digitCount > longest) // new longest
				longest = curSymbolPtr->symbol->digitCount;
			curSymbolPtr = curSymbolPtr->next;
		}
	}
	curSymbolPtr = symbolListPtr; // show each symbol in list

	while (curSymbolPtr != NULL)
	{
		listing << setw(6) << curSymbolPtr->symbol->number << ".  ";
		// write symbol number
		WriteSymbol(curSymbolPtr->symbol);
		listing << setw(longest - curSymbolPtr->symbol->digitCount + 1) 
              << ' ';
		column = longest + 10;
		if (curSymbolPtr->symbol->used)
			listing << "  ";
		else
			listing << "* ";
		column = column + 2;
		if (xFlag) // list cross reference for symbol
		{
			if (curSymbolPtr->symbol->xRefs[LHS] != NULL)
			// List symbol definitions 
			{
            listing << "  lhs:";
				column = column + 6;
				WriteXrefs(curSymbolPtr->symbol, LHS, longest + 12);
			}
			if (curSymbolPtr->symbol->xRefs[RHS] != NULL)
			// list symbol definitions 
			{
				if (MAX_COLUMN < column + 6)
				{   
               listing << endl; // terminate current line
               listing << setw(longest + 15) << ' ';
               // space to beginning of next line
               column = longest + 15;
				}
            listing << "  rhs:";
				column = column + 6;
				WriteXrefs(curSymbolPtr->symbol, RHS, longest + 12);
			}
		}
		listing << endl;
		curSymbolPtr = curSymbolPtr->next;
	}
} // WriteList

void MWriteList (SymbolPtr symbolListPtr)
//  +---------------------------------------------------------------------+
//  |  Write one of the vocabulary lists.                                 |
//  +---------------------------------------------------------------------+
{
	SymbolPtr curSymbolPtr; // current symbol in the list

	curSymbolPtr = symbolListPtr;    
	while (curSymbolPtr != NULL) // show each symbol in list
	{
		MWriteSymbol(curSymbolPtr->symbol);
		curSymbolPtr = curSymbolPtr->next;
	}
} // MWriteList

void Vocabulary()
//  +---------------------------------------------------------------------+
//  |  List the vobcabulary symbols.                                      |
//  +---------------------------------------------------------------------+
{
	WriteTitle("Terminal Symbols");
	WriteList(termList.first);
	WriteTitle("Nonterminal Symbols");
	WriteList(nontermList.first);
	if (sNumber > 0)
	{
      WriteTitle("Semantic Symbols");
      WriteList(semList.first);
	}
} // Vocabulary

void WriteFirst()
//  +---------------------------------------------------------------------+
//  |  Display first sets for all nonterminals.                           |
//  +---------------------------------------------------------------------+
{
   SymbolPtr symbolPtr1;    // current nonterminal
   SymbolPtr symbolPtr2;    // current first set element of symbolPtr1
   int longest;             // length of longest nonterminal

	longest = 0; // find longest nonterminal
	symbolPtr1 = nontermList.first;
	while (symbolPtr1 != NULL) // check the size of all nonterminals
	{
		if (symbolPtr1->symbol->digitCount > longest) // new longest found
			longest = symbolPtr1->symbol->digitCount;
		symbolPtr1 = symbolPtr1->next;
	}
	WriteTitle("First Sets");
	symbolPtr1 = nontermList.first;
	while (symbolPtr1 != NULL)  // print all nonterminals first sets
	{
		listing << "  First(";
		WriteSymbol(symbolPtr1->symbol);
		listing << " )" << setw(longest-symbolPtr1->symbol->digitCount+1)
              << ' ' << "= [";
		column = longest - symbolPtr1->symbol->digitCount + 16;
		symbolPtr2 = symbolPtr1->symbol->firstSet;
		while (symbolPtr2 != NULL)  // write the actual set
		{
			WriteSymbol(symbolPtr2->symbol);
			symbolPtr2 = symbolPtr2->next;
			if (symbolPtr2 != NULL)
			{
				listing << ',';
				column++;
				CheckColumn(symbolPtr2, longest + 16);
			}
		}
		listing << " ]";
		listing << endl;
		symbolPtr1 = symbolPtr1->next;
	}
} // WriteFirst

void WriteFollow()
//  +---------------------------------------------------------------------+
//  |  Display follow sets for all nonterminals.                          |
//  +---------------------------------------------------------------------+
{
   SymbolPtr symbolPtr1;  // current nonterminal
   SymbolPtr symbolPtr2;  // current follow set element of symbolPtr1
   int       longest;     // length of longest nonterminal

	longest = 0; // find longest nonterminal
	symbolPtr1 = nontermList.first;
	while (symbolPtr1 != NULL) // check the size of all nonterminals
	{
		if (symbolPtr1->symbol->digitCount > longest) // new longest found
			longest = symbolPtr1->symbol->digitCount;
		symbolPtr1 = symbolPtr1->next;
	}
	WriteTitle("Follow Sets");
	symbolPtr1 = nontermList.first;
	while (symbolPtr1 != NULL)  // print all nonterminals follow sets
	{
		listing << "  Follow(";
		WriteSymbol(symbolPtr1->symbol);
		listing << " )" << setw(longest - symbolPtr1->symbol->digitCount+1)
              << ' ' << "= [";
		column = longest - symbolPtr1->symbol->digitCount + 16;
		symbolPtr2 = symbolPtr1->symbol->followSet;
		while (symbolPtr2 != NULL)  // write the actual set
		{
			WriteSymbol(symbolPtr2->symbol);
			symbolPtr2 = symbolPtr2->next;
			if (symbolPtr2 != NULL)
			{
				listing << ',';
				column++;
				CheckColumn(symbolPtr2, longest + 16);
			}
		}
		listing << " ]";
		listing << endl;
		symbolPtr1 = symbolPtr1->next;
	}
} // WriteFollow

void CheckLL1()
//  +---------------------------------------------------------------------+
//  |  Check generated parse table for LL(1).                             |
//  +---------------------------------------------------------------------+
{
   MatrixPtr matrixPtr1,
             matrixPtr2;
   bool      firstPrint, 
             print;

	matrixPtr1 = matrixFirst->next;
	firstPrint = true;
	print = !tFlag;
	do
	{
		matrixPtr2 = matrixPtr1->next;
		while (matrixPtr2 != NULL)
			if (matrixPtr1->nonterm == matrixPtr2->nonterm
				&& matrixPtr1->term == matrixPtr2->term)
			{
				if (print)
				{
					if (firstPrint)
					{
						WriteTitle("LL(1) conflicts");
						firstPrint = false;
					}
					if (matrixPtr1->mark != '*')
					{
						listing << "   * [";
						WriteSymbol(matrixPtr1->nonterm);
						listing << ", ";
						WriteSymbol(matrixPtr2->term);
						listing << "] = " << matrixPtr1->prod;
						listing << endl;
					}
					listing << "   * [";
					WriteSymbol(matrixPtr2->nonterm);
					listing << ", ";
					WriteSymbol(matrixPtr2->term);
					listing << "] = " << matrixPtr2->prod;
					listing << endl;
				}
				matrixPtr1->mark = '*';
				matrixPtr2->mark = '*';
				matrixPtr2 = matrixPtr2->next;
				errorFlag = true;
			}
			else if (matrixPtr1->nonterm == matrixPtr2->nonterm)
				matrixPtr2 = matrixPtr2->next;
			else
				matrixPtr2 = NULL;
			matrixPtr1 = matrixPtr1->next;
	} while (matrixPtr1 != NULL);
} // CheckLL1

void WriteTables()
//  +---------------------------------------------------------------------+
//  |  Write generated tables in human and machine readable tables.       |
//  +---------------------------------------------------------------------+
{
	MatrixPtr matrixPtr;
	RulePtr   rulePtr;
	RhsPtr    rhsPtr;
	SymbolPtr symbolPtr;
	TablePtr  lastNontermPtr;
	int       count;

	matrixPtr = matrixFirst->next;
	count = 0;
	lastNontermPtr = NULL;
	if (tFlag)
      WriteTitle("Parse Table");
	if (mFlag)
	{
		tables << setw(10) << nTNumber << setw(10) << tNumber 
			    << setw(10) << sNumber << setw(10) << rhsNumber << endl;
		rulePtr = productions; // list then grammar
		while (rulePtr != NULL)
		{
         rhsPtr = rulePtr->alternates; // write the right hand sides
			while (rhsPtr != NULL)
			{
				count = 0;
				symbolPtr = rhsPtr->rightSide;
				while (symbolPtr != NULL)
				{
               if (symbolPtr->symbol->digitCount > 0)
						if (symbolPtr->symbol->symbolKind != SEMANTIC)
						{
                     if (symbolPtr->symbol->number != 0)
                        tables << setw(6) << symbolPtr->symbol->number;
						}
					else
                  tables << setw(6) << -(symbolPtr->symbol->number);
					count++;
					if (count == 12)
					{
						tables << endl;
						count = 0;
					}
					symbolPtr = symbolPtr->next;
				}
				tables << setw(6) << 1000 << endl;
				rhsPtr = rhsPtr->next;
			}
			rulePtr = rulePtr->next;
		}
	} // if
	do
	{
		if (lastNontermPtr != matrixPtr->nonterm)
		{
			lastNontermPtr = matrixPtr->nonterm;
			if (tFlag)
				listing << endl; // << endl << endl;
		}
		if (tFlag)
		{
			listing << matrixPtr->mark << " [";
			WriteSymbol(matrixPtr->nonterm);
			listing << ',';
			WriteSymbol(matrixPtr->term);
			listing << " ] = " << matrixPtr->prod;
			listing << endl;
		}
		if (mFlag)
			tables << ' ' << matrixPtr->mark 
				    << setw(5) << matrixPtr->nonterm->number
				    << setw(5) << matrixPtr->term->number 
				    << setw(5) << matrixPtr->prod << endl;
		matrixPtr = matrixPtr->next;
	} while (matrixPtr != NULL);
	if (mFlag)
	{
      tables << "  " << setw(5) << 0 << setw(5) << 0 
             << setw(5) << 0 << endl;
      MWriteList(semList.first);
      MWriteList(termList.first);
      MWriteList(nontermList.first);
      tables << endl;
      tables.close();
      cout << setw(5) << ' ' << "Generator parsing tables are in \""
			  << fileName+".tbl" << '"' << endl << endl;
	}
} // WriteTables

void Initialize()
//  +---------------------------------------------------------------------+
//  |  Initialize global variables.                                       |
//  +---------------------------------------------------------------------+
{
   string name; // name being entered into symbol table

	inputLine = 0;
	endOfLine = true;         // assume line is empty 
	endOfFile = false;        // set eof not yet reached 
	termList.first = NULL;    // initialize symbol lists 
	nontermList.first = NULL;
	symbols.first = NULL;
	semList.first = NULL;
	rhsLength = 0;            // initialize totals
	rhsNumber = 0;
	entNumber = 0;
	longestRhs = 0;
	shortestRhs = 0;
	tNumber = 0;
	nTNumber = 0;
	sNumber = 0;
	fFlag = false;            // initialize flags 
	gFlag = false;
	mFlag = false;
	qFlag = false;
	sFlag = false;
	tFlag = false;
	vFlag = false;
	xFlag = false;
	errorFlag = false; // set no conflicts
	incomplete = false; // assume all non-terminals have at least one rhs
	for (int i = 1; i <= TABLE_SIZE; i++) // Initialize symbol table
		symbolTable[i] = NULL;
	name = "";
	epsilon = EnterId(0, name, TERMINAL); // place NULL symbol in table
	name = "<goal>";
	productions = NewRule(); // initialize productions list
	productions->symbol = EnterId(6, name, NONTERMINAL);
	productions->symbol->definition = productions;
	matrixFirst = new MatrixNode;
	matrixLast = matrixFirst;
	GetLine(); // get the first line of input
} // Initialize

/*_________________________________________________________________________
**_________________________________________________________________________
*/                                                                           

int main()
{
	cout << "L L G E N   2 0 1 6\n" << endl;
	cout << "\nGrammar file name and path (.dat assumed): ";
	getline(cin, fileName);
   cout << endl;
	grammar.open((fileName+".dat").data());
	if (!grammar.is_open())
	{
		cout << "File not found!" << endl;
		cin.get();
		exit(1);
	}
	listing.open((fileName+".gen").data());
	Initialize();
	ReadHeader();  // decode parser header line
	ReadTokens();  // assign preset token values
	if (ch == ' ') // there is no production lhs
		Error(4);
	ReadGrammar();
	if (productions->next == NULL) // no productions were read
		Error(6);
	CompleteGrammar(); // complete augmented grammar 
	if (!qFlag && !incomplete)
	{
		ComputeFirst();  // compute the nonterminal's first sets
		ComputeFollow(); // compute follow sets 
		ComputeParseTable();
		CheckLL1();
      if (errorFlag && !(mFlag || tFlag))
         cout << " LL(1) conflicts" << endl;
	}
	if (sFlag)
		Statistics();   // list statistics about grammar
	if (xFlag)
		CrossRef();     // create cross reference lists
	if (gFlag)
		WriteGrammar(); // list augmented bnf
	if (vFlag || xFlag)
		Vocabulary();   // list vocabulary symbols
	if (!qFlag && !incomplete)
	{
		if (fFlag)
		{
			WriteFirst();  // list constructed first sets
			WriteFollow(); // list constructed follow sets
		}
		if (mFlag || tFlag)
		{
			if (mFlag)
				tables.open((fileName+".tbl").data());
			WriteTables(); // build and display parsing tables
		}
	}
	else if (incomplete)
		ListBadNonterms();
	grammar.close();
	listing.close();
	cout << setw(5) << ' ' << "Human-readable parsing tables are in \""
        << fileName+".gen" << '"' << endl;
	//cin.get();
	return 0;
}
