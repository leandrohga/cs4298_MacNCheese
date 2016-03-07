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
	if (tokenBuffer.length() < ID_STRING_LEN)
		tokenBuffer += toupper(c);
	//cerr << tokenBuffer << endl;
}
void Scanner::BufferString(char c)
{
		stringBuffer += c;
	//cerr << tokenBuffer << endl;
}

Token Scanner::CheckReserved()
{
	if (tokenBuffer == "BEGIN") return BEGIN_SYM;
	if (tokenBuffer == "END") return END_SYM;
	if (tokenBuffer == "READ") return READ_SYM;
	if (tokenBuffer == "WRITE") return WRITE_SYM;
	return ID;
}

void Scanner::ClearBuffer()
{
	tokenBuffer = "";
	stringBuffer = "";
}

void Scanner::LexicalError(char& c)
{
	cout << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber+1 << '.' << endl;
	listFile << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber+1 << '.' << endl;
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
	}
	else
		lineBuffer += c;
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
		if (isspace(currentChar))
			currentChar = NextChar();     // do nothing
		else if (isalpha(currentChar))
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
		}
		else if (isdigit(currentChar))
		{                                // integer literal
			BufferChar(currentChar);
			c = sourceFile.peek();
			while (isdigit(c))
			{
				currentChar = NextChar();
				BufferChar(currentChar);
				c = sourceFile.peek();
			}
			return INT_LITERAL;
		}else if (currentChar == '"')
		{
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
					return STR_LITERAL;
				}else if(currentChar == '"' & sourceFile.peek()=='"'){
					currentChar = NextChar();
				}
				BufferString(currentChar);
			} while (sourceFile.peek()!='\n');
			return STR_LITERAL;
		}
		else if (currentChar == '(')
			return LPAREN;
		else if (currentChar == ')')
			return RPAREN;
		else if (currentChar == ';')
			return SEMICOLON;
		else if (currentChar == ',')
			return COMMA;
		else if (currentChar == '+')
		{
			BufferChar(currentChar);
			return PLUS_OP;
		}
		else if (currentChar == ':')
			if (sourceFile.peek() == '=')
			{                             // := operator
				currentChar = NextChar();
				return ASSIGN_OP;
			}
			else
				LexicalError(currentChar);
		else if (currentChar == '-')  
			if (sourceFile.peek() == '-') // comment
				do  // skip comment
					currentChar = NextChar();
				while (currentChar != '\n');
			else
			{
				BufferChar(currentChar);      // minus operator
				return MINUS_OP;
			}

		else
			LexicalError(currentChar);
	} // end while
	return EOF_SYM;
}
