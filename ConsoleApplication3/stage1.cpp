//Anh Minh Le, Minh Huynh
// CS 4301
// Stage 1

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "stage1.h"
using namespace std;

//HELPER FUCTION FOR STAGE 1 ------------------------------------------
bool isRel_Op(string name)
{
	if (name == "=" || name == "<>" ||
		name == "<=" || name == ">=" ||
		name == "<" || name == ">")
	{
		return true;
	}
	return false;
}
bool isAdd_Level_Op(string name) {
	if (name == "+" || name == "-" || name == "or") {
		return true;
	}
	return false;
}

bool isMult_Level_Op(string token) {
	if (token == "*" || token == "div" || token == "mod" || token == "and") {
		return true;
	}
	return false;
}
time_t now = time(NULL);	// get the time in seconds
// ctime(&now) will turn is to date, time, year
bool endF = false;

Compiler::Compiler(char **argv)	 // constructor
{
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

Compiler::~Compiler()			// destructor
{
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
	// output to the .lst file
	listingFile << "STAGE1:  Anh Minh Le      " << ctime(&now) << endl;
	listingFile << "LINE NO.              SOURCE STATEMENT" << endl;
}

void Compiler::parser()
{
	//ch must be initialized to the first character of the source file
	nextChar();

	if (nextToken() != "program")
		processError("Syntatic Fail: keyword \"program\" expected: "+ token);

	//parser implements the grammar rules, calling first rule
	prog();
}

void Compiler::createListingTrailer()
{
	listingFile << "\nCOMPILATION TERMINATED      ";

	if (errorCount == 1)
		listingFile << errorCount << " ERROR ENCOUNTERED" << endl;
	else
		listingFile << errorCount << " ERRORS ENCOUNTERED" << endl;
}

void Compiler::processError(string err)
{
	++errorCount;
	listingFile << endl << "Error: Line " << lineNo << ": " << err << endl;
	createListingTrailer();
	exit(1);	// exit with an error flag ON
}

// Methods implementing the grammar productions
void Compiler::prog()	        // stage 0, production 1
{
	if (token != "program")
		processError("Syntatic Fail: keyword \"program\" expected: " + token);

	progStmt();

	if (token == "const")
		consts();

	if (token == "var")
		vars();

	if (token != "begin")
		processError("Syntatic Fail: keyword \"begin\" expected: " + token);

	beginEndStmt();

	if (token[0] != END_OF_FILE)
		processError("Syntatic Fail: no text may follow \"end\": " + token);
}

void Compiler::progStmt()		// stage 0, production 2
{
	string x;

	if (token != "program")
		processError("Syntatic Fail: keyword \"program\" expected: " + token);

	x = nextToken();	// x is name of program

	if (!isNonKeyId(x))
		processError("Syntatic Fail: program name expected: " + token);

	if (nextToken() != ";")
		processError("Syntatic Fail: semicolon expected: " + token);

	nextToken();

	if (x.length() > 15)
		code("program", x.substr(0, 15));
	else
		code("program", x);

	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}

void Compiler::consts()         // stage 0, production 3
{
	if (token != "const")
		processError("Syntatic Fail: keyword \"const\" expected: " + token);

	// nextToken() is not a NON_KEY_ID
	if (!isNonKeyId(nextToken()))
		processError("Syntatic Fail: non-keyword identifier must follow \"const\": " + token);

	constStmts();
}

void Compiler::vars()           // stage 0, production 4
{
	if (token != "var")
		processError("Syntatic Fail: keyword \"var\" expected: " + token);

	// nextToken() is not a NON_KEY_ID
	if (!isNonKeyId(nextToken()))
		processError("Syntatic Fail: non-keyword identifier must follow \"var\": " + token);

	varStmts();
}

void Compiler::beginEndStmt()   // stage 0, production 5
{
	if (token != "begin")
		processError("Syntatic Fail: keyword \"begin\" expected: " + token);
	nextToken();
	if (token != "read" && token != "write" && !isNonKeyId(token)) {
		processError("Syntatic Fail: Non_key_id, \"read\", or \"write\" expected: " + token);
	}
	execStmts();
	if (token != "end")
		processError("Syntatic Fail: keyword \"end\" expected: " + token);

	if (nextToken() != ".")
		processError("Syntatic Fail: period expected: " + token);

	nextToken();

	if (token[0] == END_OF_FILE)
		code("end", ".");
}

void Compiler::constStmts()
{
	string x, y;

	//Send error if token is not a NON_KEY_ID
	if (isNonKeyId(token) == false)
		processError("Syntatic Fail: non-keyword identifier expected: " + token);

	//Set x to the value of token
	x = token;

	//Send error if next token is not "="
	if (nextToken() != "=")
		processError("Syntatic Fail:  \"=\" expected: " + token);

	//Set y to the value of nextToken
	y = nextToken();

	//Determine if token after "=" is legal, send error on false
	if (y != "+" && y != "-" && y != "not" && isNonKeyId(y) == false && y != "true" && y != "false" && isInteger(y) == false)
		processError("Syntatic Fail: token to right of \"=\" illegal: " + token);

	//Determine if y is one of "+","-"
	if (y == "+" || y == "-")
	{
		//Send error if the next token is not an integer
		nextToken();
		if (!isdigit(token[0]))
			processError("Syntatic Fail: digit expected after sign: " + token);

		//Combine y and token
		y = y + token;
	}

	//Determine if y is "not"
	if (y == "not")
	{
		//Send error if next token is not boolean
		if (!isBoolean(nextToken()))
			processError("Syntatic Fail: boolean expected after \"not\": " + token);

		// Determine how the "not" should be applied
		if (token == "true")
			y = "false";
		else
			y = "true";
	}

	// Send error if y is not an integer or boolean
	if (!isInteger(y) && !isBoolean(y))
		processError("Syntatic Fail: data type of token on the right-hand side must be INTEGER or BOOLEAN: " + token);

	if (isNonKeyId(y) && symbolTable.find(y) != symbolTable.end())
		insert(x, symbolTable.find(y)->second.getDataType(), CONSTANT, symbolTable.find(y)->second.getValue(), YES, 1);
	else
		// Insert the constant onto the symbol table
		insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);

	// Send error if next token is not ';'
	if (nextToken() != ";")
		processError("Syntatic Fail: semicolon expected: " + token);

	// Set x to the value of the next token
	x = nextToken();
	// Send error if x is not one of "begin", "var", NON_KEY_ID
	if (x != "begin" && x != "var" && isNonKeyId(x) == false)
	{
		if (x.find("__") != string::npos)
			processError("Syntatic Fail: '_' must be followed by a letter or number: " + token);
		else
			processError("Syntatic Fail: non-keyword identifier, \"begin\", or \"var\" expected: " + token);
	}

	//Determine if x is a NON_KEY_ID
	if (isNonKeyId(x))
		constStmts();
}

void Compiler::varStmts()       // stage 0, production 7
{
	string x, y;

	//Send error if token is not a NON_KEY_ID
	if (isNonKeyId(token) == false)
		processError("Syntatic Fail: non-keyword identifier expected: " + token);

	x = ids();
	//Send error if token is not ":"
	if (token != ":")
		processError("Syntatic Fail: \":\" expected: " + token);

	//Send error if next token is not an integer or boolean
	nextToken();
	if (isInteger(token) == false && isBoolean(token) == false)
		processError("Syntatic Fail: illegal type follows \":\": " + token);

	//Set y equal to token
	y = token;

	//Send error if next token is not ';'
	if (nextToken() != ";")
		processError("Syntatic Fail: semicolon expected: " + token);

	//Insert the variable onto the symbol table
	insert(x, whichType(y), VARIABLE, "1", YES, 1);  // I altered this line from the psuedocode.
												   // I think Dr. Motl forgot the whichType().

	nextToken();
	if (token != "begin" && isNonKeyId(token) == false)
		processError("Syntatic Fail: non-keyword identifier or \"begin\" expected: " + token);

	//Determine if token is a NON_KEY_ID
	if (isNonKeyId(token))
		varStmts();
}
string Compiler::ids()          // stage 0, production 8
{
	string temp, tempString;

	// if token is not a NON_KEY_ID
	if (!isNonKeyId(token))
		processError("Syntatic Fail: non-keyword identifier expected: " + token);

	tempString = token;
	temp = token;

	// getting the multiple ids on the same line, seperated by ','
	// If nextToken() == ","
	if (nextToken() == ",")
	{
		// if (nextToken() is not a NON_KEY_ID)
		if (!isNonKeyId(nextToken()))
			processError("Syntatic Fail: non-keyword identifier expected: " + token);

		tempString = temp + "," + ids();	// recursively call it to get the ids
	}

	return tempString;
}

// Action routines

void Compiler::insert(string externalName, storeTypes inType, modes inMode,
	string inValue, allocation inAlloc, int inUnits)
{
	string name = "";
	uint i, u, temp;
	//break name from list of external names
	uint count = 0;
	//[a,b]
	for (i = 0; i < externalName.length(); i++)
	{
		if (externalName[i] != ',')
		{
			if (count >= 15)
				continue;

			name += externalName[i];
			count++;
		}
		else
			break;
	}
	temp = i + 1;
	while (name != "")
	{
		//symbolTable[name] is defined
		if (symbolTable.find(name) != symbolTable.end())
			processError("Semantic Fail: symbol " + name + " is multiply defined");
		else if (isKeyword(name))
			processError("Semantic Fail: illegal use of keyword " + name);
		//create table entry
		else
		{
			if (isupper(name[0]))
				symbolTable.insert({ name,
					SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits) });

			else
				symbolTable.insert({ name,
					SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits) });

		}
		name = "";
		count = 0;
		for (u = temp; u < externalName.length(); u++)
		{
			if (externalName[u] != ',')
			{
				if (count >= 15)
				{
					continue;
				}
				name += externalName[u];
				count++;
			}
			else
				break;
		}
		temp = u + 1;
	}

	if (symbolTable.size() > 256)
		processError("Semantic Fail: symbol table overflow");
}

storeTypes Compiler::whichType(string name)		// tells which data type a name has
{
	storeTypes dataType;

	if (isLiteral(name))
	{
		if (isBoolean(name))
			dataType = BOOLEAN;
		else
			dataType = INTEGER;
	}
	else	// name is an identifier and hopefully a constant
	{
		// find where the name is
		map<string, SymbolTableEntry>::iterator finding;
		finding = symbolTable.find(name);

		// if it did find the name
		if (finding != symbolTable.end())
			dataType = finding->second.getDataType();
		else
			processError("Semantic Fail: reference to undefined constant "+ name);
	}

	return dataType;
}

string Compiler::whichValue(string name)		// tells which value a name has
{
	string value;
	if (name[0] == 'T')
		value = "1";
	else if (isLiteral(name))
	{
		if (name == "false")
			value = "0";

		else if (name == "true")
			value = "-1";

		else
			value = name;
	}
	else	//name is an identifier and hopefully a constant
	{
		// find where the name is
		map<string, SymbolTableEntry>::iterator finding;
		finding = symbolTable.find(name);

		// if it did find the name
		if (finding != symbolTable.end() && finding->second.getValue() != "")
			value = finding->second.getValue();
		else
			processError("Semantic Fail:  reference to undefined constant "+ name);
	}

	return value;
}

void Compiler::code(string op, string operand1, string operand2)//FIX ME edit again with al the case @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
{
	if (op == "program")
		emitPrologue(operand1);
	else if (op == "end")
		emitEpilogue();
	else if (op == "read")
		emitReadCode(operand1);
	else if (op == "write")
		emitWriteCode(operand1);
	else if (op == "+") // this must be binary '+'
		emitAdditionCode(operand1, operand2);
	else if (op == "-") // this must be binary '-'
		emitSubtractionCode(operand1, operand2);
	else if (op == "neg") // this must be unary '-'
		emitNegationCode(operand1);
	else if (op == "not")
		emitNotCode(operand1);
	else if (op == "*")
		emitMultiplicationCode(operand1, operand2);
	else if (op == "div")
		emitDivisionCode(operand1, operand2);
	else if (op == "mod")
		emitModuloCode(operand1, operand2);
	else if (op == "and")
		emitAndCode(operand1, operand2);
	else if (op == "=") {
		emitEqualityCode(operand1, operand2);
	}
	else if (op == "<>") {
		emitInequalityCode(operand1, operand2);
	}
	else if (op == "or") {
		emitOrCode(operand1, operand2);
	}
	else if (op == "<") {
		emitLessThanCode(operand1, operand2);
	}
	else if (op == ">") {
		emitGreaterThanCode(operand1, operand2);
	}
	else if (op == "<=") {
		emitLessThanOrEqualToCode(operand1, operand2);
	}
	else if (op == ">=") {
		emitGreaterThanOrEqualToCode(operand1, operand2);
	}
	else if (op == ":=") {
		emitAssignCode(operand1, operand2);
	}
	else
		processError("Semantic Fail: compiler error since function code should not be called with illegal arguments "+ token);
}

// Helper functions for the Pascallite lexicon
bool Compiler::isKeyword(string s) const		// determines if s is a keyword
{
	//stage 1 add 6 more key-word [ mod, div, and, or, read, write ]
	return (s == "program" || s == "begin" || s == "end" || s == "var"
		|| s == "const" || s == "integer" || s == "boolean"
		|| s == "true" || s == "false" || s == "not"
		|| s == "mod" || s == "div" || s == "and" || s == "or"
		|| s == "read" || s == "write");
}

bool Compiler::isSpecialSymbol(char c) const	// determines if c is a special symbol
{
	return c == '=' || c == ':' || c == ',' || c == ';'
		|| c == '.' || c == '+' || c == '-' || c == '*'
		|| c == '(' || c == ')' || c == '<' || c == '>';
}

bool Compiler::isNonKeyId(string s) const		// determines if s is a non_key_id
{
	if (isKeyword(s))
		return false;

	// '_' cannot be at the beginning or the end
	if (s[s.length() - 1] == '_')
		return false;
	if (!islower(s[0]) || isdigit(s[0]))
		return false;
	if (s.length() > 1) {
		for (uint i = 1; i < s.length() - 1; ++i)
			// it must satisfy: start with a lowercase character
			// and all the remaining characters must be lowercase or a digit
		//if (!(islower(s[0]) && (isdigit(s[i]) || islower(s[i]) || !(s[i] == '_' && s[i + 1] == '_'))))
		//	return false;
		{             //good             /good                       GOOD <-  !---bad----
			if (!(islower(s[i]) || isdigit(s[i]) || !(s[i] == '_' && s[i + 1] == '_')))
				return false;
		}
	}

	return true;
}

bool Compiler::isInteger(string s) const		// determines if s is an integer
{
	if (s == "integer")
		return true;

	else if (symbolTable.find(s) != symbolTable.end())
		if ((symbolTable.find(s))->second.getDataType() == INTEGER)
			return true;

	// go over each char to check if it is a digit
	// what if that number is +123 or -123 or 123
	for (int i = 0, numChar = s.length(); i < numChar; ++i)
		if (s[0] != '+' && s[0] != '-' && !isdigit(s[i]))
			return false;

	return true;
}

bool Compiler::isBoolean(string s) const		// determines if s is a boolean
{
	if (s == "true" || s == "false" || s == "boolean")
		return true;
	else if (isNonKeyId(s)) {
		if (symbolTable.find(s) != symbolTable.end()) {
			if (symbolTable.at(s).getDataType() == BOOLEAN)
				return true;
		}
	}
	return false;
}

bool Compiler::isLiteral(string s) const		// determines if s is a literal
{                                                                                                        //It looks right according to the grammer, but it may need revision.
	if (isInteger(s) || isBoolean(s) || s.substr(0, 3) == "not" || s[0] == '+' || s[0] == '-')
	{
		if (s[0] == '+' || s[0] == '-')
		{
			if (isInteger(s.substr(1, s.length())))
			{
				return true;
			}

			return false;   //The string began with '+'/'-', but was not followed by an integer
		}
		else if (s.substr(0, 3) == "not ")
		{
			if (isBoolean(s.substr(4, s.length() - 1)))
			{
				return true;
			}

			return false;   //The string began with 'not', but was not followed by a boolean
		}

		return true;        //The string is either an integer or boolean literal
	}

	return false;           //If first condition is failed, the string is not a literal
}

// Emit Functions
void Compiler::emit(string label, string instruction, string operands, string comment)
{
	objectFile << left << setw(8) << label;
	objectFile << setw(8) << instruction;
	objectFile << setw(24) << operands << comment << endl;
}

void Compiler::emitPrologue(string progName, string)
{
	objectFile << "; Anh Minh Le, Minh Huynh      " << ctime(&now);
	objectFile << "%INCLUDE \"Along32.inc\"" << endl << "%INCLUDE \"Macros_Along.inc\"" << endl << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	objectFile << endl;
	emit("_start:");
}

void Compiler::emitEpilogue(string, string)
{
	emit("", "Exit", "{0}");
	emitStorage();
}

void Compiler::emitStorage()
{
	map<string, SymbolTableEntry>::iterator entries;

	objectFile << endl;

	emit("SECTION", ".data");
	/*
		for those entries in the symbolTable that have
		an allocation of YES and a storage mode of CONSTANT
		{ call emit to output a line to objectFile }
	*/
	for (entries = symbolTable.begin(); entries != symbolTable.end(); ++entries)
		if (entries->second.getAlloc() == YES && entries->second.getMode() == CONSTANT && entries->second.getDataType() != PROG_NAME)
			emit(entries->second.getInternalName(), "dd", entries->second.getValue(), "; " + entries->first);

	objectFile << endl;

	emit("SECTION", ".bss");
	/*
	for those entries in the symbolTable that have
	an allocation of YES and a storage mode of VARIABLE
	{ call emit to output a line to objectFile }
	*/
	for (entries = symbolTable.begin(); entries != symbolTable.end(); ++entries)
		if (entries->second.getAlloc() == YES && entries->second.getMode() == VARIABLE)
			emit(entries->second.getInternalName(), "resd", entries->second.getValue(), "; " + entries->first);
	objectFile << endl;
	objectFile << "-------------------------------------------------------------------Table-----------------------------------------------------------------------\n";
	objectFile << setw(20) << "exName" << setw(20) << "| inName" << setw(20) << "| stype" << setw(20) << "| smode" << setw(20) << "| value" << setw(20) << "| salloc" << setw(20) << "| unit" << endl;
	objectFile << "-------------------------------------------------------------------------------------------------------------------------------------------------\n";
	
	for (auto row : symbolTable) {
		printTableLine(row.first, row.second.getInternalName(),
			row.second.getDataType(), row.second.getMode(),
			row.second.getValue(), row.second.getAlloc(), row.second.getUnits());
	}
}

void Compiler::printTableLine(string exName, string inName, storeTypes dataType, modes mode, string value, allocation alloc , int unit) {
	string stype;
	string smode;
	string salloc;
	if (dataType == INTEGER) {
		stype = "INTEGER";
	}
	else if (dataType == BOOLEAN) {
		stype = "BOOLEAN";
	}
	else if (dataType == PROG_NAME) {
		stype = "PROG_NAME";
	}
	else if (dataType == UNKNOWN) {
		stype = "UNKNOWN";
	}

	if (mode == VARIABLE) {
		smode = "VARIABLE";
	}
	else smode = "CONSTANT";
	if (alloc == YES) salloc = "YES"; else salloc = "NO";
	string sunit = to_string(unit);
	objectFile << setw(20) << exName << setw(20) <<"|"+  inName << setw(20) << "|" + stype << setw(20) << "|" + smode << setw(20) << "|" + value << setw(20) << "|" + salloc << setw(20) << "|" + sunit << endl;
}

// Lexical routines
//Huan version

string Compiler::nextToken() //returns the next token or end of file marker
{
	token = "";
	while (token == "")
	{
		if (ch == '{') //process comment
		{
			while (nextChar() != END_OF_FILE && ch != '}')
			{ //empty body 
			}
			if (ch == END_OF_FILE)
				processError("Lexical Fail: unexpected end of file: " + ch);
			else
				nextChar();
		}

		else if (ch == '}') processError("Lexical Fail '}' cannot begin token: "+ ch);
		else if (isspace(ch)) nextChar();
		else if (isSpecialSymbol(ch)) {
			//update as of stage 1

			token = ch;
			nextChar();
			if (token == ":" && ch == '=') {
				token += ch;
				nextChar();
			}
			else if ((token == "<" && ch == '>') || (token == "<" && ch == '=') || (token == ">" && ch == '=')) {
				token += ch;
				nextChar();
			}
		}
		else if (islower(ch)) {
			token = ch;
			while ((nextChar() == '_' || (ch >= 'a' && ch <= 'z')
				|| (ch >= '0' && ch <= '9')) && ch != END_OF_FILE)
			{
				token += ch;
			}
			if (ch == END_OF_FILE)
				processError("Lexical Fail: unexpected end of file: "+ ch);
		}
		else if (isdigit(ch)) {
			token = ch;
			while (nextChar() != END_OF_FILE && ch >= '0' && ch <= '9')
			{
				token += ch;
			}
			if (ch == END_OF_FILE)
				processError("Lexical Fail: unexpected end of file: " + ch);
		}

		else if (ch == END_OF_FILE) token = ch;
		else processError("Lexical Fail: illegal symbol: "+ ch);
	}
	return token;
}

char Compiler::nextChar() //returns the next character or end of file marker
{
	sourceFile.get(ch);

	static char prevChar = '\n';

	if (sourceFile.eof()) {
		ch = END_OF_FILE;
		return ch;
	}
	else {
		if (prevChar == '\n') {
			listingFile << setw(5) << ++lineNo << '|';
		}
		listingFile << ch;
	}

	prevChar = ch;
	return ch;
}
// Other routines
string Compiler::genInternalName(storeTypes stype) const
{
	static int countI = 0, countB = 0, countProg = 0;
	string name;

	if (stype == INTEGER)
		name = 'I' + to_string(countI++);
	else if (stype == BOOLEAN)
		name = 'B' + to_string(countB++);
	else
		name = "P" + to_string(countProg++);

	return name;
}


//------------------------------------------------------------
//STAGE 1-----------------------------------------------------
//-------------------------------------------------------------

//RULE: After Push_operand(token) -> move NextTOken
void Compiler::execStmts()// stage 1, production 2 
{ 
	if (isNonKeyId(token) || token == "read" || token == "write")
	{
		execStmt();
		nextToken();
		execStmts(); 
	} 
}

void Compiler::execStmt() {//production 3
	if (isNonKeyId(token)) {
		assignStmt();
	}
	else if (token == "read")
	{
		readStmt();
	}
	else if (token == "write") {
		writeStmt();
	}

}

void Compiler::assignStmt() {//production 4
	if (isNonKeyId(token)) {
		pushOperand(token);// a := b ; + c
		nextToken();
		if (token != ":=") {
			processError("Syntactic: expected \":=\" after non-key-ID: "+ token);
		}
		pushOperator(token);
		nextToken();
		express();
		if (token != ";") {
			processError("Syntactic:  expected a ';': " + token);
		}
		string second, first;
		second = popOperand().substr(0, 15);
		first = popOperand().substr(0, 15);
		code(popOperator(), second, first);
		//Correct logic code 
	}
	else { processError("Syntactic: non-keyword id expected\n: " + token); }
}

void Compiler::readStmt() {
	string readList;
	if (nextToken() != "(") {
		processError("Syntactic: '(' expected after \"read\": " + token);
	}
	nextToken();
	readList = ids();
	if (token != ")") {
		processError("Syntactic: ')' expected after non_key_id in \"read\": " + token);
	}
	if (nextToken() != ";") {
		processError("Syntactic: semicolon expected: " + token);
	}
	code("read", readList);

}

void Compiler::writeStmt() {
	if (token == "write") {
		nextToken();
		string x;

		if (token == "(") {
			nextToken();
			x = ids();
			if (token != ")") {
				processError("Syntactic: ';' or ')' expected after non_key_id in \"write\": " + token);
			}
		}
		else {
			processError("'(' expected after \"write\": " + token);
		}
		code("write", x);
		//but the logic is right, we need to check read -> (  -> write List -> ) 
		if (nextToken() != ";") {
			processError("Syntactic:  ; expected: " + token);
		}
	}
	else {
		processError("Syntactic: expected \"write\" for write statement: " + token);
	}
}


void Compiler::express() {

	term();
	expresses();


}
//term -> factor -> part -> b-> )
void Compiler::expresses() {
	if (token == "=" || token == "<>" || token == "<="
		|| token == ">=" || token == "<" || token == ">")
	{
		pushOperator(token);

		nextToken();
		term();

		string second = popOperand().substr(0, 15);
		string first = popOperand().substr(0, 15);
		code(popOperator(), second, first);

		expresses();
	}
}

void Compiler::term() {
	if (token == "not" || token == "true" ||
		token == "false" || token == "(" ||
		token == "+" || token == "-" ||
		isInteger(token) || isNonKeyId(token))
	{
		factor();
		terms();
	}
	else {
		processError("Syntactic: Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected in factor: " + token);
	}
}

void Compiler::terms() {
	if (isAdd_Level_Op(token)) {
		pushOperator(token);
		nextToken();
		factor();
		string second = popOperand().substr(0, 15);
		string first = popOperand().substr(0, 15);
		code(popOperator(), second, first);
		terms();
	}
	else if (isRel_Op(token) || token == ";" || token == ")")
	{
		//do nothing
	}
	else { processError("Syntactic: +, -, or expected: " + token); }
}
//a = notr
void Compiler::factor() {
	if (token == "not" || isBoolean(token) || token == "(" || token == "+" || token == "-" ||
		isInteger(token) || isNonKeyId(token)) {
		part();
		factors();
	}
	else {
		processError("Syntactic: Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected in part: " + token);
	}
}
//a = a div b and c div m;
void Compiler::factors() {
	if (isMult_Level_Op(token)) {
		pushOperator(token);
		nextToken();
		part();
		string second = popOperand().substr(0, 15);
		string first = popOperand().substr(0, 15);
		code(popOperator(), second, first);
		factors();
	}
	else if (token == "=" || token == "<>" || token == "<=" ||
		token == ">=" || token == "<" || token == ">" ||
		token == ")" || token == ";" || token == "-" ||
		token == "+" || token == "or")
	{
		// do nothing - epsilon sign
	}
	else {
		processError("Syntactic: one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected: " + token);
	}
}

//a = (b);
void Compiler::part()			// stage 1, production 15
{
	if (token == "not")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();
			express();

			if (token != ")")
				processError("Syntactic: ')' expected: " + token);

			code("not", popOperand().substr(0, 15));
		}

		else if (isNonKeyId(token))
			code("not", token);

		else if (isBoolean(token))
		{
			if (token == "true")	// not true = false
				pushOperand("false");
			else					// not false = true
				pushOperand("true");
		}

		else
			processError("Syntactic: expected '(', boolean or non-keyword id; found " + token);
	}

	else if (token == "+")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();
			express();

			if (token != ")")
				processError("Syntactic: ')' expected: " + token);
		}

		else if (isNonKeyId(token) || isInteger(token))
			pushOperand(token);

		else
			processError("Syntactic: expected '(', integer, or non-keyword id; found " + token);
	}

	else if (token == "-")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();
			express();

			if (token != ")")
				processError("Syntactic: ')' expected: " + token);

			code("neg", popOperand().substr(0, 15));
		}

		else if (isNonKeyId(token))
			code("neg", token);

		else if (isInteger(token))
			pushOperand('-' + token);

		else
			processError("Syntactic: expected '(', integer, or non-keyword id; found " + token);
	}

	else if (isNonKeyId(token) || isInteger(token) || isBoolean(token))
		pushOperand(token);

	else if (token == "(")
	{
		nextToken();
		express();

		if (token != ")")
			processError("Syntactic: ')' expected: " + token);
	}

	else
		processError("Syntactic: one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected; found " + token);

	nextToken();
}
void Compiler::pushOperator(string name) {
	operatorStk.push(name);
}
void Compiler::pushOperand(string name) {
	/*if name is a literal and has no symbol table entry
	 insert symbol table entry, call whichType to determine the data type of the literal
	 push name onto stack;
	 */
	 //find in the SymbolTableEntry return end -> mean not found name in STE 

	if (isLiteral(name) && symbolTable.find(name) == symbolTable.end()) {
		if (name == "true")
		{
			symbolTable.insert({ name,SymbolTableEntry("TRUE",
													whichType(name),
													CONSTANT, whichValue(name), YES, 1) });
		}
		else if (name == "false") {
			symbolTable.insert({ name,SymbolTableEntry("FALSE",
													whichType(name),
													CONSTANT, whichValue(name), YES, 1) });
		}
		else
			symbolTable.insert({ name,SymbolTableEntry(genInternalName(whichType(name)),
													whichType(name),
													CONSTANT, whichValue(name), YES, 1) });
	}



	operandStk.push(name);
}
string Compiler::popOperator() {
	string topElement = "";
	if (!operatorStk.empty()) {
		topElement = operatorStk.top();
		operatorStk.pop();
		return topElement;
	}
	else {
		processError("compiler error; operator stack underflow- tryied to pop() from empty Operator stack");

	}
}
string Compiler::popOperand() {
	string topElement = "";
	if (!operandStk.empty()) {
		topElement = operandStk.top();
		operandStk.pop();
		return topElement;
	}
	else {
		processError("compiler error; operator stack underflow- tryied to pop() from empty Operand stack");

	}
}



void Compiler::freeTemp() {
	currentTempNo--;
	if (currentTempNo < -1) {
		processError("Compiler error, currentTempNo should be greater or equal than -1");
	}
}
string Compiler::getTemp() {
	string temp;
	currentTempNo++;
	temp = "T" + to_string(currentTempNo);
	if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "1", NO, 1);
		maxTempNo++;
	}
	return temp;
}
//All Emit functions 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//:LongSG VERISON


void Compiler::emitAdditionCode(string operand1, string operand2)       // op2 +  op1
{
	// emit("operand1 = " + operand1, " operand2 = " + operand2, " AReg = " + contentsOfAReg);

	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand2);

	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("Semantic Fail: binary '+' requires integer operands: " + operand2 + "+" + operand1);

	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{	// emit code to load operand2 into A register
		emit("", "mov", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + ']', "; AReg = " + whichValue(operand2));
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		emit("", "add", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + ']',
			"; AReg = " + operand2 + " + " + operand1);

	else
		emit("", "add", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + ']',
			"; AReg = " + operand1 + " + " + operand2);

	// free the temporary being used
	if (isTemporary(operand1))
		freeTemp();

	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);

	pushOperand(contentsOfAReg);
}

//**************************************************************************************

void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 *  op1
{
	// emit("operand1 = " + operand1, " operand2 = " + operand2, " AReg = " + contentsOfAReg);

	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand2);

	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("Semantic Fail: binary '*' requires integer operands: " + operand2 + "*" + operand1);

	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{	// emit code to load operand2 into A register
		emit("", "mov", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + ']', "; AReg = " + whichValue(operand2));
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		emit("", "imul", "dword [" + symbolTable.find(operand1)->second.getInternalName() + ']',
			"; AReg = " + operand2 + " * " + operand1);

	else
		emit("", "imul", "dword [" + symbolTable.find(operand2)->second.getInternalName() + ']',
			"; AReg = " + operand1 + " * " + operand2);

	// free the temporary being used
	if (isTemporary(operand1))
		freeTemp();

	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);

	pushOperand(contentsOfAReg);
}


//-------------------------------------------------------------------------------------------------------------------
void Compiler::emitSubtractionCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand2);

	if (!isInteger(operand1) || !isInteger(operand2)) {
		processError("Semantic Fail: binary '-' requires integer operands: " + operand2 + "-" + operand1);
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(allocation::YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" &&  contentsOfAReg[0] != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		// deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != operand2) {
		//emit code to load operand2 into A register
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;

	}
	//emit code to perform register-memory subtraction
	if (contentsOfAReg == operand2)
		emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
			"; AReg = " + operand2 + " - " + operand1);
	else {
		emit("", "sub", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
			"; AReg = " + operand1 + " - " + operand2);
	}


	//deassign all temporaries involved in the addition and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to integer
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	//push the name of the result onto operandStk
	operandStk.push(contentsOfAReg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													T0						[I2] //b
void Compiler::emitDivisionCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand2);

	if (!isInteger(operand1) || !isInteger(operand2)) {
		processError("Semantic Fail: binary 'div' requires integer operands: " + operand2 + "div" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg[0] == 'T'   && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	//PSU: if A register holds a non-temp not operand2 then deassign it
	if (contentsOfAReg != "" &&  contentsOfAReg[0] != 'T' && contentsOfAReg != operand2) {
		// deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != operand2) { //op2 / op1 -> gan op2-> eax 
		//emit code to load operand2 into A register
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
		//"eax" = op2
	}
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	//deassign all temporaries involved and free those names for reuse T2 / T1 eax, free T0,-> getTemp(T0) ->
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to integer
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

void Compiler::emitModuloCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined symbol " + operand2);

	if (!isInteger(operand1) || !isInteger(operand2)) {
		processError("Semantic Fail: binary 'mod' requires integer operands: " + operand2 + "mod" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg[0] == 'T'   && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	//PSU: if A register holds a non-temp not operand2 then deassign it
	if (contentsOfAReg != "" &&  contentsOfAReg[0] != 'T' && contentsOfAReg != operand2) {
		// deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != operand2) { //op2 / op1 -> gan op2-> eax 
		//emit code to load operand2 into A register
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
		//"eax" = op2
	}
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	//deassign all temporaries involved and free those names for reuse T2 / T1 eax, free T0,-> getTemp(T0) ->
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to integer
	emit("", "xchg", "eax,edx", "; exchange quotient and remainder");
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

//:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=
void Compiler::emitAssignCode(string operand1, string operand2)         // op2 = op1
{
	//emit("operand1 = " + operand1, " operand2 = " + operand2, " AReg = " + contentsOfAReg);

	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable" + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable" + operand2);

	// the that non_key_id was not initialized before
	if (symbolTable.find(operand2)->second.getMode() == CONSTANT)
		processError("Semantic Fail: symbol on left-hand side of assignment must have a storage mode of VARIABLE: "+ operand2+ " := " + operand1);

	if (whichType(operand1) != whichType(operand2))
		processError("Semantic Fail: incompatible types for operator ':=': " + operand2 + ":=" + operand1);

	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable '" + operand1 + "'");

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable '" + operand2 + "'");

	if (symbolTable.find(operand2)->second.getMode() != VARIABLE)
		processError("Semantic Fail: symbol on left-hand side of assignment must have a storage mode of VARIABLE: "+ operand2 + " := " + operand1);

	if (operand1 == operand2)
		return;

	if (contentsOfAReg != operand1)
	{
		// emit code to load operand1 into the A register

		if (operand1 == "true") {
			emit("", "mov", "eax,[TRUE]", "; AReg = " + operand1);
			contentsOfAReg = "TRUE";
		}
		else if (operand1 == "false") {
			emit("", "mov", "eax,[FALSE]", "; AReg = " + operand1);
			contentsOfAReg = "FALSE";


		}
		else {
			emit("", "mov", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + ']', "; AReg = " + operand1);
			contentsOfAReg = operand1;
		}

	}

	emit("", "mov", '[' + symbolTable.find(operand2)->second.getInternalName() + "],eax", "; " + operand2 + " = AReg");
	contentsOfAReg = operand2;

	// free any temp being used
	if (isTemporary(operand1))
		freeTemp();
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void Compiler::emitAndCode(string operand1, string operand2) // op2 && op1			
{
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: "+ operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);

	if (whichType(operand1) != BOOLEAN && whichType(operand2) != BOOLEAN) {
		processError("Semantic Fail: binary 'and' requires operands of the same type: " + operand2 + "and" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		if (operand1 == "true") {
			emit("", "and", "eax,[TRUE]", "; AReg = " + operand2 + " and " + operand1);
		}
		else if (operand1 == "false")
			emit("", "and", "eax,[FALSE]", "; AReg = " + operand2 + " and " + operand1);
		else
			emit("", "and", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + ']', "; AReg = " + operand2 + " and " + operand1);

	else
		if (operand2 == "true") {
			emit("", "and", "eax,[TRUE]", "; AReg = " + operand1 + " and " + operand2);
		}
		else if (operand2 == "false")
			emit("", "and", "eax,[FALSE]", "; AReg = " + operand1 + " and " + operand2);
		else
			emit("", "and", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + ']', "; AReg = " + operand1 + " and " + operand2);

	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// This is 2-way operation so order doesnot matter
// as AReg can represent either operand 1 or 2
void Compiler::emitOrCode(string operand1, string operand2)             // op2 || op1
{
	// emit("operand1 = " + operand1, " operand2 = " + operand2, " AReg = " + contentsOfAReg);

	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable");

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable");

	if (whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN)
		processError("Semantic Fail: binary 'or' requires operands of the same type: " + operand2 + "or" + operand1);

	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{	// emit instruction to do a register-memory load of operand2 into the A registe
		emit("", "mov", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + ']', "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}

	if (contentsOfAReg == operand2)
		if (operand1 == "true") {
			emit("", "or", "eax,[TRUE]", "; AReg = " + operand2 + " or " + operand1);
		}
		else if (operand1 == "false")
			emit("", "or", "eax,[FALSE]", "; AReg = " + operand2 + " or " + operand1);
		else
			emit("", "or", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + ']', "; AReg = " + operand2 + " or " + operand1);

	else
		if (operand2 == "true") {
			emit("", "or", "eax,[TRUE]", "; AReg = " + operand1 + " or " + operand2);
		}
		else if (operand2 == "false")
			emit("", "or", "eax,[FALSE]", "; AReg = " + operand1 + " or " + operand2);
		else
			emit("", "or", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + ']', "; AReg = " + operand1 + " or " + operand2);

	// free the temporary being used
	if (isTemporary(operand1))
		freeTemp();

	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}

void Compiler::emitNegationCode(string operand1, string)           // -op1
{
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable" + operand1);

	if (whichType(operand1) != INTEGER)
		processError("Semantic Fail: unary '-' requires an integer operand: -" + operand1);

	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
	{
		// emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1)
	{	// emit instruction to do a register-memory load of operand1 into the A registe
		emit("", "mov", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + ']', "; AReg = " + operand1);
		contentsOfAReg = operand1;
	}

	emit("", "neg", "eax", "; AReg = -AReg");

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);

	pushOperand(contentsOfAReg);
}
void Compiler::emitNotCode(string operand1, string)						// !op1
{
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable" + operand1);

	if (whichType(operand1) != BOOLEAN)
		processError("Semantic Fail: unary 'not' requires a boolean operand: !"+ operand1);

	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
	{
		// emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
		contentsOfAReg = "";

	if (contentsOfAReg != operand1)
	{	// emit instruction to do a register-memory load of operand2 into the A registe
		emit("", "mov", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + ']', "; AReg = " + operand1);
		contentsOfAReg = operand1;
	}

	emit("", "not", "eax", "; AReg = !AReg");

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);
}

//a = "12" -> aat(0))=-> '1' T0
void Compiler::emitEqualityCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);
	if (whichType(operand1) != whichType(operand2)) {
		processError("Semantic Fail: binary '=' requires operands of the same type: " + operand2 + "=" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	//if neither operand is in A register then
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	//emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	string labelN = getLabel(); //L0
	//je                 .L4          ; h = f then jump to set eax to TRUE
	if (contentsOfAReg == operand2)
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "je", labelN, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	}
	else {
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
		emit("", "je", labelN, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	}
	//  mov     eax,[FALSE]             ; else set eax to FALSE
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	string labelN_plus_one = getLabel();
	//jmp     .L5                     ; unconditionally jump
	emit("", "jmp", labelN_plus_one, "; unconditionally jump");
	emit(labelN + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	//insert TRUE in symbol table with value -1 and external name true
	symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	//emit code to label the next instruction with the second acquired label L(n+1)
	emit(labelN_plus_one + ":");
	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}
//opern 118.asm 118.dat
void Compiler::emitInequalityCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);
	if (whichType(operand1) != whichType(operand2)) {
		processError("Semantic Fail: binary '<>' requires operands of the same type: " + operand2 + "<>" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	//if neither operand is in A register then
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}

	string labelN = getLabel(); //L0
	//je                 .L4          ; h = f then jump to set eax to TRUE
	if (contentsOfAReg == operand2)
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "jne", labelN, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	}
	else {
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
		emit("", "jne", labelN, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	}
	//  mov     eax,[FALSE]             ; else set eax to FALSE
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	string labelN_plus_one = getLabel();
	//jmp     .L5                     ; unconditionally jump
	emit("", "jmp", labelN_plus_one, "; unconditionally jump");
	emit(labelN + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	//insert TRUE in symbol table with value -1 and external name true
	symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	//emit code to label the next instruction with the second acquired label L(n+1)
	emit(labelN_plus_one + ":");
	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

void Compiler::emitLessThanCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);
	if (whichType(operand1) != whichType(operand2)) {
		processError("Semantic Fail: binary '<' requires operands of the same type: " + operand2 + "<" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	//if neither operand is in A register then
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}

	string labelN = getLabel(); //L0
	//je                 .L4          ; h = f then jump to set eax to TRUE
	if (contentsOfAReg == operand2)
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "jl", labelN, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}
	else {
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
		emit("", "jl", labelN, "; if " + operand1 + " < " + operand2 + " then jump to set eax to TRUE");
	}
	//  mov     eax,[FALSE]             ; else set eax to FALSE
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	string labelN_plus_one = getLabel();
	//jmp     .L5                     ; unconditionally jump
	emit("", "jmp", labelN_plus_one, "; unconditionally jump");
	emit(labelN + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	//insert TRUE in symbol table with value -1 and external name true
	symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	//emit code to label the next instruction with the second acquired label L(n+1)
	emit(labelN_plus_one + ":");
	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

//open 119
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);
	if (whichType(operand1) != whichType(operand2)) {
		processError("Semantic Fail: binary '<=' requires operands of the same type: " + operand2 + "<=" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	//if neither operand is in A register then
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	//emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	string labelN = getLabel(); //L0
	//je                 .L4          ; h = f then jump to set eax to TRUE
	if (contentsOfAReg == operand2)
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "jle", labelN, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}
	else {
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
		emit("", "jle", labelN, "; if " + operand1 + " <= " + operand2 + " then jump to set eax to TRUE");
	}
	//  mov     eax,[FALSE]             ; else set eax to FALSE
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	string labelN_plus_one = getLabel();
	//jmp     .L5                     ; unconditionally jump
	emit("", "jmp", labelN_plus_one, "; unconditionally jump");
	emit(labelN + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	//insert TRUE in symbol table with value -1 and external name true
	symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	//emit code to label the next instruction with the second acquired label L(n+1)
	emit(labelN_plus_one + ":");
	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

void Compiler::emitGreaterThanCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);
	if (whichType(operand1) != whichType(operand2)) {
		processError("Semantic Fail: binary '>' requires operands of the same type: " + operand2 + ">" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	//if neither operand is in A register then
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	//emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	string labelN = getLabel(); //L0
	//je                 .L4          ; h = f then jump to set eax to TRUE
	if (contentsOfAReg == operand2)
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "jg", labelN, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	}
	else {
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
		emit("", "jg", labelN, "; if " + operand1 + " > " + operand2 + " then jump to set eax to TRUE");
	}
	//  mov     eax,[FALSE]             ; else set eax to FALSE
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	string labelN_plus_one = getLabel();
	//jmp     .L5                     ; unconditionally jump
	emit("", "jmp", labelN_plus_one, "; unconditionally jump");
	emit(labelN + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	//insert TRUE in symbol table with value -1 and external name true
	symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	//emit code to label the next instruction with the second acquired label L(n+1)
	emit(labelN_plus_one + ":");
	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) {
	if (symbolTable.find(operand1) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand1);

	if (symbolTable.find(operand2) == symbolTable.end())
		processError("Semantic Fail: reference to undefined variable: " + operand2);
	if (whichType(operand1) != whichType(operand2)) {
		processError("Semantic Fail:binary '>=' requires operands of the same type: " + operand2 + ">=" + operand1);
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) == 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	if (contentsOfAReg != "" && contentsOfAReg.at(0) != 'T' && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = "";
	}
	//if neither operand is in A register then
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	//emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	string labelN = getLabel(); //L0
	//je                 .L4          ; h = f then jump to set eax to TRUE
	if (contentsOfAReg == operand2)
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "jge", labelN, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	}
	else {
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
		emit("", "jge", labelN, "; if " + operand1 + " >= " + operand2 + " then jump to set eax to TRUE");
	}
	//  mov     eax,[FALSE]             ; else set eax to FALSE
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	string labelN_plus_one = getLabel();
	//jmp     .L5                     ; unconditionally jump
	emit("", "jmp", labelN_plus_one, "; unconditionally jump");
	emit(labelN + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	//insert TRUE in symbol table with value -1 and external name true
	symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	//emit code to label the next instruction with the second acquired label L(n+1)
	emit(labelN_plus_one + ":");
	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1)) {
		freeTemp();
	}
	if (isTemporary(operand2)) {
		freeTemp();
	}
	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	//push name of result onto operandStk;
	operandStk.push(contentsOfAReg);
}

string Compiler::getLabel() {
	static int i = 0;
	string newLabel = ".L" + to_string(i);
	i++;
	return newLabel;
}

void Compiler::emitReadCode(string operand, string) {
	vector<string> listOperand;
	//while (name is broken from list (operand) and put in name != "")
	string currentToken = "";
	for (auto x : operand) {
		if (x != ',') {
			currentToken += x;
		}
		else {
			if (currentToken.length() > 15) {
				listOperand.push_back(currentToken.substr(0, 15));
			}
			else listOperand.push_back(currentToken);
			currentToken = "";
		}
	}
	listOperand.push_back(currentToken.substr(0, 15));
	for (auto name : listOperand) {
		// find return end if can not find 
		//if name is not in symbol table
		if (symbolTable.find(name) == symbolTable.end()) {
			processError("Semantic Fail:reference to undefined variable '" + name + "'");
		}
		//if data type of name is not INTEGER
		if (symbolTable.at(name).getDataType() != INTEGER) {
			processError("Semantic Fail:can't read variables of this type: "+ name );
		}
		//if storage mode of name is not VARIABLE
		if (symbolTable.at(name).getMode() != VARIABLE) {
			processError("Semantic Fail:attempting to read to a read-only location: "+ name);

		}
		emit("", "call", "ReadInt", "; read int; value placed in eax");
		emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
		contentsOfAReg = name;
	}
}

void Compiler::emitWriteCode(string operand, string) {

	static bool definedStorage = false;
	vector<string> listOperand;
	//while (name is broken from list (operand) and put in name != "")
	string currentToken = "";
	for (auto x : operand) {
		if (x != ',') {
			currentToken += x;
		}
		else {
			if (currentToken.length() > 15) {
				listOperand.push_back(currentToken.substr(0, 15));
			}
			else listOperand.push_back(currentToken);
			currentToken = "";
		}
	}
	listOperand.push_back(currentToken.substr(0, 15));
	for (auto name : listOperand) {
		if (symbolTable.find(name) == symbolTable.end()) {
			processError("Semantic Fail:reference to undefined variable '" + name + "'");
		}
		if (contentsOfAReg != name) {
			//emit the code to load name in A register
			emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
			//set the contentsOfAReg = name
			contentsOfAReg = name;
		}
		if (whichType(name) == INTEGER)
		{
			//call    WriteInt                ; write int in eax to standard out
			emit("", "call", "WriteInt", "; write int in eax to standard out");
		}
		else {//// data type is BOOLEAN
			// emit code to compare A register to 0
			emit("", "cmp", "eax,0", "; compare to 0");
			// acquire a new label Ln
			string labelN = getLabel();
			// emit code to jump if equal to the acquired label Ln
			emit("", "je", labelN, "; jump if equal to print FALSE");
			// emit code to load address of TRUE literal in D register
			emit("", "mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
			// acquire a second label L(n + 1)
			string labelN_plus_one = getLabel();
			// emit code to unconditionally jump to label L(n + 1)
			emit("", "jmp", labelN_plus_one, "; unconditionally jump to " + labelN_plus_one);
			// emit code to label the next line with the first acquired label Ln
			emit(labelN + ":");
			// emit code to load address of FALSE literal in D register
			emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
			// emit code to label the next line with the second acquired label L(n + 1)
			emit(labelN_plus_one + ":");
			// emit code to call the Irvine WriteString function
			emit("", "call", "WriteString", "; write string to standard out");
			//if static variable definedStorage is false
			if (definedStorage == false) {
				definedStorage = true;
				objectFile << endl;
				emit("SECTION", ".data");
				// emit code to create label TRUELIT, instruction db, operands 'TRUE',0
				emit("TRUELIT", "db", "\'TRUE\',0", "; literal string TRUE");
				// emit code to create label FALSELIT, instruction db, operands 'FALSE',0
				emit("FALSLIT", "db", "\'FALSE\',0", "; literal string FALSE");
				// output an endl to objectFile
				objectFile << endl;
				// emit code to resume .text SECTION
				emit("SECTION", ".text");
			}
		}
		emit("", "call", "Crlf ", "; write \\r\\n to standard out");
	}

}

bool Compiler::isTemporary(string s) const {
	if (!s.empty()) {
		if (s.at(0) == 'T' &&  s != "TRUE") {
			return true;
		}
	}
	return false;
} // determines if s represents a temporary //Haft DONE
