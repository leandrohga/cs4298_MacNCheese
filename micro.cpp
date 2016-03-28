/**
 * Macc n Cheese Compiler
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

//global file streams
ifstream sourceFile; //source program
ofstream outFile,    //object program
         listFile;   //compiler listing

//includes
#include "mscan.h"   //scanner component definition
#include "mparse.h"	 //parser component definition
#include "mcode.h"   //code generator component definition

//global component objects
Scanner scan;        //scanner
Parser parse;        //parser
CodeGen code;        //code generator

/**
 * Returns the current date.
 */
string Date();

/**
 * Returns the current time.
 */
string Time();

int main(int argc, char* argv[]) {
	string sourceName, outName, listName;

	cout << "\n"
		 << " M A C C   N   C H E E S E   C O M P I L E R\n"
		 << " ___________________________________________\n"
		 << endl;

	//Get the source file; if an argument was passed on the command-line, use it as the source file name
	cout << " Source file (.mnc extension is assumed): ";
	if (argc > 1) {
		sourceName = argv[1];
		cout << sourceName;
	} else {
		getline(cin, sourceName);
	}

	//Add appropriate extensions to file names.
	outName = sourceName + ".asm";
	listName = sourceName + ".lst";
	sourceName += ".mnc";

	// Open and initialize all files.
	sourceFile.open(sourceName.data());
	if (!sourceFile.is_open()) {
		cout << "\n File not found. Compilation aborted!\n\n";
		cin.get();
		exit(1);
	}
	outFile.open(outName.data());
	listFile.open(listName.data());

	listFile
		<< "\n\n  M N C   C O M P I L E R   L I S T I N G\n\n"
		<< " Generated code is SAM assembly language for\n"
		<< "         the MACC2 virtual computer.\n"
		<< " ___________________________________________\n\n";
	listFile << ' ' + Date() << " at " << Time() << endl;
	listFile << " Source file: " << sourceName << endl << endl;
	listFile << " LINE #" << endl;

	parse.SystemGoal();

	cout << endl
		<< "\n Successful Compilation\n"
		<< "\n Object code: " << outName << endl
		<< "\n Listing file: " << listName << endl << endl;

	return 0;
}

string Date() {
	const time_t current_time = time (NULL);
	char the_date[20];
	strftime (the_date, 20, "%B %d, %Y", localtime (&current_time));
	return the_date;
}

string Time() {
	const time_t current_time = time (NULL);
	char the_time[10];
	strftime (the_time, 10, "%I:%M %p", localtime (&current_time));
	return the_time;
}
