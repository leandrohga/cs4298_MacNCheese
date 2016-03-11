/*	____________________________________________________________________________

	                 Scanner Component for the Micro A Compiler

	                                mscan.h

	                              Version 2007 - 2016
 
	                           James L. Richards
	                     Last Update: August 28, 2007
	                      	   Nelson A Castillo
	                     Last Update: January 31, 2016

	The routines in this unit are based on those provided in the book 
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr., 
	Benjamin Cummings Publishing Co. (1991).

	See Section 2.2, pp. 25-29.
	____________________________________________________________________________
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

extern ifstream sourceFile;
extern ofstream outFile, listFile;

#include "mscan.h"

// *******************
// **  Constructor  **
// *******************

Scanner::Scanner()
{
	tokenBuffer = "";
	lineBuffer = "";
	lineNumber = 0;
}

// ********************************
// **  Private Member Functions  **
// ********************************

void Scanner::BufferChar(char c)
{
	if (tokenBuffer.length() < ID_STRING_LEN){
		tokenBuffer += c;
	}
	//cerr << tokenBuffer << endl;
}
void Scanner::BufferString(char c)
{
		stringBuffer += c;
	//cerr << tokenBuffer << endl;
}

Token Scanner::CheckReserved()
{
	if (tolower(tokenBuffer) == "bool") return BOOL_SYM;
	if (tolower(tokenBuffer) == "break") return BREAK_SYM;
	if (tolower(tokenBuffer) == "case") return CASE_SYM;
	if (tolower(tokenBuffer) == "cheese") return CHEESE_SYM;
	if (tolower(tokenBuffer) == "decs") return DECS_SYM;
	if (tolower(tokenBuffer) == "do") return DO_SYM;
	if (tolower(tokenBuffer) == "else") return ELSE_SYM;
	if (tolower(tokenBuffer) == "end") return END_SYM;
	if (tolower(tokenBuffer) == "false") return FALSE_SYM;
	if (tolower(tokenBuffer) == "float") return FLOAT_SYM;
	if (tolower(tokenBuffer) == "for") return FOR_SYM;
	if (tolower(tokenBuffer) == "hiphip") return HIPHIP_SYM;
	if (tolower(tokenBuffer) == "if") return IF_SYM;
	if (tolower(tokenBuffer) == "int") return INT_SYM;
	if (tolower(tokenBuffer) == "listen") return LISTEN_SYM;
	if (tolower(tokenBuffer) == "otherwise") return OTHERWISE_SYM;
	if (tolower(tokenBuffer) == "select") return SELECT_SYM;
	if (tolower(tokenBuffer) == "shout") return SHOUT_SYM;
	if (tolower(tokenBuffer) == "then") return THEN_SYM;
	if (tolower(tokenBuffer) == "true") return TRUE_SYM;
	if (tolower(tokenBuffer) == "while") return WHILE_SYM;

	return ID;
}

void Scanner::ClearBuffer()
{
	tokenBuffer = "";
	stringBuffer = "";
}

void Scanner::LexicalError(char& c, string& errorExp="")
{	
	cout << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber+1 << '.' << endl;
	listFile << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber+1 << '.' << endl;
	if(errorExp!=""){
		//TODO :: I was starting with the comment but 
		//I need to ampliate the code 
	}
	c = NextChar();
}

char Scanner::NextChar()
{
	char c;

	sourceFile.get(c);
	if (c == '\n')
	{
		listFile.width(6);
		listFile << ++lineNumber << "  " << lineBuffer << endl;
		lineBuffer = "";
	} else{
		lineBuffer += c;
	}
	return c;
}

// *******************************
// **  Public Member Functions  **
// *******************************

Token Scanner::GetNextToken()
{
	char currentChar, c;

	ClearBuffer();
	currentChar = NextChar();
	while (!sourceFile.eof())
	{
		if (isspace(currentChar)){
			currentChar = NextChar();     // do nothing
		} else if (isalpha(currentChar))
		{                                // identifier
			BufferChar(currentChar);
			c = sourceFile.peek();
			while (isalnum(c) || c == '_')
			{
				currentChar = NextChar();
				BufferChar(currentChar);
				c = sourceFile.peek();
			}
			return CheckReserved();
		} else if (isdigit(currentChar)){                                // integer or float literals
			BufferChar(currentChar);
			c = sourceFile.peek();
			while (isdigit(c))
			{
				currentChar = NextChar();
				BufferChar(currentChar);
				c = sourceFile.peek();
			}
			/* Check if it is a float */
			if (c == '.')
			{
				currentChar = NextChar();
				c = sourceFile.peek();
				if (!isdigit(c)) 
					LexicalError(currentChar);
				BufferChar(currentChar);
				while (isdigit(c))
				{
					currentChar = NextChar();
					BufferChar(currentChar);
					c = sourceFile.peek();
				}
				 if (c == 'e' || c == 'E')
				{
					currentChar = NextChar();
					c = sourceFile.peek();
					if(c != '+' && c!= '-'){
						LexicalError(currentChar);
					}
					BufferChar(currentChar);
					currentChar = NextChar();
					c = sourceFile.peek();
					if (!isdigit(c)) 
						LexicalError(currentChar);
					BufferChar(currentChar);
					while (isdigit(c))
					{
						currentChar = NextChar();
						BufferChar(currentChar);
						c = sourceFile.peek();
					}
				}
				return FLOAT_LIT;
			}
			return INT_LIT;
		} else if (currentChar == '"'){
			// string literal
//			BufferString(currentChar);
			do
			{
				currentChar = NextChar();
//				cout << currentChar <<endl;
				if(currentChar == '"' & sourceFile.peek()!='"'){
//					BufferString(currentChar);
//					cout << "________________________" <<endl;
//					cout << stringBuffer <<endl;
//					cout << "________________________" <<endl;
					return CHEESE_LIT;
				}else if(currentChar == '"' & sourceFile.peek()=='"'){
					currentChar = NextChar();
				}
				BufferString(currentChar);
			} while (sourceFile.peek()!='\n');
			return CHEESE_LIT;
		} else if (currentChar == '('){
			return LBANANA;
		} else if (currentChar == ')'){
			return RBANANA;
		} else if (currentChar == '['){
			return LSTAPLE;
		} else if (currentChar == ']'){
			return RSTAPLE;
		} else if (currentChar == '{'){
			return LMUSTACHE;
		} else if (currentChar == '}'){
			return RMUSTACHE;
		} else if (currentChar == ';'){
			return SEMICOLON;
		} else if (currentChar == ':'){
			return COLON;
		} else if (currentChar == ','){
			return COMMA;
		} else if (currentChar == '+'){
			BufferChar(currentChar);
			return PLUS_OP;
		} else if (currentChar == '*'){
			BufferChar(currentChar);
			return MULT_OP;
		} else if (currentChar == '/'){
			if (sourceFile.peek() == ':') {// comment
				do{  // skip comment
					currentChar = NextChar();
					if(currentChar == ':'){
						currentChar = NextChar();
						if(currentChar == '/'){
							break;
						}
					}
				} while (!sourceFile.eof());
			}
			BufferChar(currentChar);
			return DIV_OP;
		} else if (currentChar == '=')
			if (sourceFile.peek() == '=')
			{
				currentChar = NextChar();
				return EQ_OP1;
			}
			currentChar = NextChar();
			return ASSIGN_OP;
		} else if (currentChar == '!')
			if (sourceFile.peek() == '!')
			{
				currentChar = NextChar();
				return EQ_OP2;
			}
			else if(sourceFile.peek() == '='){
			currentChar = NextChar();
			return NE_OP;
			}
		} else if (currentChar == '<')
			if (sourceFile.peek() == '=')
			{
				currentChar = NextChar();
				return LE_OP;
			}
			currentChar = NextChar();
			return LT_OP;
		} else if (currentChar == '>')
			if (sourceFile.peek() == '=')
			{
				currentChar = NextChar();
				return GE_OP;
			}
			currentChar = NextChar();
			return GT_OP;
		} else if (currentChar == '-'){
			if (sourceFile.peek() == '-') {// comment
				do{  // skip comment
					currentChar = NextChar();
				} while (currentChar != '\n');
			} else{
				BufferChar(currentChar);      // minus operator
				return MINUS_OP;
			}
		} else{
			LexicalError(currentChar);
		}
	} // end while

	return EOF_SYM;
}
