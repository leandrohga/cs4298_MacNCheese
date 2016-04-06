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
#include <algorithm>
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
	cheese_size = 1; /* Null char */
}

// ********************************
// **  Private Member Functions  **
// ********************************

void Scanner::BufferChar(char c)
{
	if (tokenBuffer.length() < ID_STRING_LEN){
		tokenBuffer += c;
	} else {
		cout << endl << " *** Fatal error: token is bigger then "
			<< "the TokenBuffer." << endl;
		cout << " *** Error at line " << this->lineNumber << endl;
		cout << " *** Please consider spliting cheese variables "
			<< "or recompiling the MaccNCheese compiler "
			<< "with a bigger TokenBuffer." << endl;
		exit(1);
	}
}

Token Scanner::CheckReserved()
{
	/* Convert the string to lower case */
	transform(tokenBuffer.begin(), tokenBuffer.end(), \
				tokenBuffer.begin(), ::tolower);
	/* Check the converted words */
	if ((tokenBuffer) == "bool") return BOOL_SYM;
	if ((tokenBuffer) == "break") return BREAK_SYM;
	if ((tokenBuffer) == "case") return CASE_SYM;
	if ((tokenBuffer) == "cheese") return CHEESE_SYM;
	if ((tokenBuffer) == "decs") return DECS_SYM;
	if ((tokenBuffer) == "do") return DO_SYM;
	if ((tokenBuffer) == "else") return ELSE_SYM;
	if ((tokenBuffer) == "end") return END_SYM;
	if ((tokenBuffer) == "false") return FALSE_SYM;
	if ((tokenBuffer) == "float") return FLOAT_SYM;
	if ((tokenBuffer) == "for") return FOR_SYM;
	if ((tokenBuffer) == "hiphip") return HIPHIP_SYM;
	if ((tokenBuffer) == "if") return IF_SYM;
	if ((tokenBuffer) == "int") return INT_SYM;
	if ((tokenBuffer) == "listen") return LISTEN_SYM;
	if ((tokenBuffer) == "otherwise") return OTHERWISE_SYM;
	if ((tokenBuffer) == "until") return UNTIL_SYM;
	if ((tokenBuffer) == "select") return SELECT_SYM;
	if ((tokenBuffer) == "shout") return SHOUT_SYM;
	if ((tokenBuffer) == "then") return THEN_SYM;
	if ((tokenBuffer) == "true") return TRUE_SYM;
	if ((tokenBuffer) == "while") return WHILE_SYM;

	return ID;
}

void Scanner::ClearBuffer()
{
	tokenBuffer = "";
}

void Scanner::LexicalError(char& c)
{	
	cout << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber + 1 << '.' << endl;
	listFile << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber + 1 << '.' << endl;

	c = NextChar();
}
void Scanner::LexicalError(char& c, const string& errorExp)
{
	cout << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber + 1 << '.' << '\n'
		<< errorExp << endl;
	listFile << " *** Lexical Error: '" << c
		<< "' ignored at position " << int(lineBuffer.size())
		<< " on line #" << lineNumber + 1 << '.' << '\n'
		<< errorExp << endl;

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
		if (isspace(currentChar)) {
			/* do nothing */
			currentChar = NextChar();
		} else if (isalpha(currentChar)) {
			/* identifier */
			BufferChar(currentChar);
			c = sourceFile.peek();
			while (isalnum(c) || c == '_') {
				currentChar = NextChar();
				BufferChar(currentChar);
				c = sourceFile.peek();
			}
			return CheckReserved();
		} else if (isdigit(currentChar)) {
			/* integer or float literals */
			BufferChar(currentChar);
			c = sourceFile.peek();
			while (isdigit(c)) {
				currentChar = NextChar();
				BufferChar(currentChar);
				c = sourceFile.peek();
			}
			/* check if it is a float */
			if (c == '.') {
				currentChar = NextChar();
				c = sourceFile.peek();
				/* check for a digit after the '.' */
				if (!isdigit(c)) 
					LexicalError(currentChar, to_string(c) \
						+ " Float needs a digit" \
						" after the '.'");
				BufferChar(currentChar);
				while (isdigit(c)) {
					currentChar = NextChar();
					BufferChar(currentChar);
					c = sourceFile.peek();
				}
				/* check for power of 10 multipliers */
				if (c == 'e' || c == 'E') {
					currentChar = NextChar();
					c = sourceFile.peek();
					if (c != '+' && c!= '-') {
						LexicalError(currentChar, \
							to_string(c) + \
							" Float needs a "
							"'+'/'-' after 'E'");
					}
					BufferChar(currentChar);
					currentChar = NextChar();
					c = sourceFile.peek();
					if (!isdigit(c)) 
						LexicalError(currentChar, \
							to_string(c) + \
							" Float needs a " \
							"digit after '+'/'-'");
					BufferChar(currentChar);
					while (isdigit(c)) {
						currentChar = NextChar();
						BufferChar(currentChar);
						c = sourceFile.peek();
					}
				}
				return FLOAT_LIT;
			}
			return INT_LIT;
		} else if (currentChar == '"') {
			/* string literal */
			BufferChar(currentChar);
			c = sourceFile.peek();
			cheese_size = 1; /* Null Char */
			/* while not end of string */
			while (c != '"') {
				/* escape sequences */
				if (c == '\\') {
					/*
					 * Replace the '\' for a ':'
					 * which's SAM's escape char
					 */
					currentChar = NextChar();
					c = sourceFile.peek();
					currentChar = ':';
					if (c == '\\') {
						/* '\\' sequence */
						/* Just remove one '\' */
						currentChar = NextChar();
						BufferChar(currentChar);
						c = sourceFile.peek();
						cheese_size += 1;
					} else if (c == '"') {
						/* '\"' sequence */
						/* replace '\' for ':' */
						BufferChar(currentChar);
						currentChar = NextChar();
						BufferChar(currentChar);
						c = sourceFile.peek();
						cheese_size += 1;
					} else if (c == 'n') {
						/* \n sequence */
						/* replace for ascii '\n'(012)*/
						BufferChar(currentChar);
						currentChar = NextChar();
						currentChar = '0';
						BufferChar(currentChar);
						currentChar = '1';
						BufferChar(currentChar);
						currentChar = '2';
						BufferChar(currentChar);
						c = sourceFile.peek();
						cheese_size += 1;
					} else if (isdigit(c)) {
						/* '\ddd' sequence */
						/* replace '\' for ':' */
						BufferChar(currentChar);

						int ind;
						for (ind = 0; ind < 3; ind++) {
							/* check for 3 digits */
							if (!isdigit(c))
								LexicalError(c, to_string(c) + " received. Expected three digits after \\.");
							currentChar = NextChar();
							BufferChar(currentChar);
							c = sourceFile.peek();
						}
						cheese_size += 1;
					} else {
						LexicalError(currentChar, to_string(currentChar) + \
						" was followed by the wrong character -options are \\ or \".");
					}
				} else if (c == ':') {
					/*
					 * ':' is the escape char used
					 * by the SAM assembler. So it
					 * needs to be escaped.
					 */
					currentChar = NextChar();
					BufferChar(currentChar);
					BufferChar(currentChar);
					c = sourceFile.peek();
					cheese_size += 1;
				} else {
					/* regular characters */
					currentChar = NextChar();
					BufferChar(currentChar);
					c = sourceFile.peek();
					cheese_size += 1;
				}
			}
			/* buffer the final '"' */
			currentChar = NextChar();
			BufferChar(currentChar);
			return CHEESE_LIT;
		} else if (currentChar == '(') {
			BufferChar(currentChar);
			return LBANANA;
		} else if (currentChar == ')') {
			BufferChar(currentChar);
			return RBANANA;
		} else if (currentChar == '[') {
			BufferChar(currentChar);
			return LSTAPLE;
		} else if (currentChar == ']') {
			BufferChar(currentChar);
			return RSTAPLE;
		} else if (currentChar == '{') {
			BufferChar(currentChar);
			return LMUSTACHE;
		} else if (currentChar == '}') {
			BufferChar(currentChar);
			return RMUSTACHE;
		} else if (currentChar == ';') {
			BufferChar(currentChar);
			return SEMICOLON;
		} else if (currentChar == ':') {
			BufferChar(currentChar);
			return COLON;
		} else if (currentChar == ',') {
			BufferChar(currentChar);
			return COMMA;
		} else if (currentChar == '+') {
			BufferChar(currentChar);
			return PLUS_OP;
		} else if (currentChar == '*') {
			BufferChar(currentChar);
			return MULT_OP;
		} else if (currentChar == '/') {
			/* check if it is a multiline comment */
			c = sourceFile.peek();
			if (c == ':') {
				do { /* skip comment */
					currentChar = NextChar();
					if (currentChar == ':') {
						currentChar = NextChar();
						if (currentChar == '/') {
							currentChar = NextChar();
							break;
						}
					}
				} while (!sourceFile.eof());
			} else if (c == '/') {
				/* single line commment */
				do { /* skip comment */
					currentChar = NextChar();
				} while (currentChar != '\n');
			} else {
				/* if it is a division operator */
				BufferChar(currentChar);
				return DIV_OP;
			}
		} else if (currentChar == '=') {
			BufferChar(currentChar);
			if (sourceFile.peek() == '=') {
				currentChar = NextChar();
				BufferChar(currentChar);
				return EQ_OP1;
			}
			currentChar = NextChar();
			return ASSIGN_OP;
		} else if (currentChar == '!') {
			BufferChar(currentChar);
			if (sourceFile.peek() == '!') {
				currentChar = NextChar();
				BufferChar(currentChar);
				return EQ_OP2;
			} else if (sourceFile.peek() == '=') {
				currentChar = NextChar();
				BufferChar(currentChar);
				return NE_OP;
			} else {
				LexicalError(currentChar, to_string(c) + \
						" The not operator is not" \
						" supported by MnC");
			}
			currentChar = NextChar();
		} else if (currentChar == '<') {
			BufferChar(currentChar);
			if (sourceFile.peek() == '=') {
				currentChar = NextChar();
				BufferChar(currentChar);
				return LE_OP;
			}
			currentChar = NextChar();
			return LT_OP;
		} else if (currentChar == '>') {
			BufferChar(currentChar);
			if (sourceFile.peek() == '=') {
				currentChar = NextChar();
				BufferChar(currentChar);
				return GE_OP;
			}
			currentChar = NextChar();
			return GT_OP;
		} else if (currentChar == '-') {
			/* minus operator */
			BufferChar(currentChar);
			currentChar = NextChar();
			return MINUS_OP;
		} else {
			/* Unrecognized character */
			LexicalError(currentChar, to_string(c) + \
					" Unrecognized character");
		}
	} /* end while */

	return EOF_SYM;
}
