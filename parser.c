#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 200
#define MAX_SYMBOL_COUNT 50
#define MAX_REG_COUNT 10

// generated code
instruction *code;
int cIndex;

// symbol table
symbol *table;
int tIndex;

lexeme *tokens; // "list[]" in pseudocode
int lIndex; // "listidx" in pseudocode

int level;
int registerCounter;
int error = 0;

// given functions
void emit(int opname, int reg, int level, int mvalue);
void addToSymbolTable(int k, char n[], int s, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// TODO: our functions
void program();
void block();
int var_declaration();
void proc_declaration();
void statement();
void condition();
void expression();
void term();
void factor();

instruction *parse(lexeme *list, int printTable, int printCode)
{
	// set up program variables
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	cIndex = 0;
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
	tIndex = 0;
	tokens = list;

	program();

	// print off table and code
	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();
	
	// mark the end of the code
	code[cIndex].opcode = -1;
	return code;
}

void emit(int opname, int reg, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].r = reg;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int s, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].size = s;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Symbol names must be identifiers\n");
			break;
		case 3:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 4:
			printf("Parser Error: Array sizes must be given as a single, nonzero number\n");
			break;
		case 5:
			printf("Parser Error: [ must be followed by ]\n");
			break;
		case 6:
			printf("Parser Error: Multiple symbols in var declaration must be separated by commas\n");
			break;
		case 7:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 8:
			printf("Parser Error: Procedure declarations should contain a semicolon before the body of the procedure begins\n");
			break;
		case 9:
			printf("Parser Error: Procedures may not be assigned to, read, or used in arithmetic\n");
			break;
		case 10:
			printf("Parser Error: Undeclared identifier\n");
			break;
		case 11:
			printf("Parser Error: Variables cannot be indexed\n");
			break;
		case 12:
			printf("Parserr Error: Arrays must be indexed\n");
			break;
		case 13:
			printf("Parser Error: Assignment operator missing\n");
			break;
		case 14:
			printf("Parser Error: Register Overflow Error\n");
			break;
		case 15:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 16:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 17:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 18:
			printf("Parser Error: if must be followed by ?\n");
			break;
		case 19:
			printf("Parser Error: do must be followed by while\n");
			break;
		case 20:
			printf("Parser Error: read must be followed by a var or array identifier\n");
			break;
		case 21:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 22:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 23:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 24:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, and variables\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Size | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %4d | %5d | %5d\n", table[i].kind, table[i].name, table[i].size, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tR\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				printf("RET\t");
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("WRT\t");
				break;
			case 10:
				printf("RED\t");
				break;
			case 11:
				printf("HLT\t");
				break;
			case 12:
				printf("NEG\t");
				break;
			case 13:
				printf("ADD\t");
				break;
			case 14:
				printf("SUB\t");
				break;
			case 15:
				printf("MUL\t");
				break;
			case 16:
				printf("DIV\t");
				break;
			case 17:
				printf("MOD\t");
				break;
			case 18:
				printf("EQL\t");
				break;
			case 19:
				printf("NEQ\t");
				break;
			case 20:
				printf("LSS\t");
				break;
			case 21:
				printf("LEQ\t");
				break;
			case 22:
				printf("GTR\t");
				break;
			case 23:
				printf("GEQ\t");
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\t%d\n", code[i].r, code[i].l, code[i].m);
	}
	
	if (table != NULL)
		free(table);
}

// our functions

void program()
{
	// directly from pseudocode
	registerCounter = -1;
	emit (7, 0, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;
	block();

	// check for misc. errors
	if (error != 0)
	{
		printparseerror(error);
		return;
	}

	// check for error 1
	if (tokens[lIndex].type != periodsym)
	{
		error = 1;
		printparseerror(error);
		return;
	}

	emit(11, 0, 0, 0); // 11 == HALT
	code[0].m = table[0].addr;

	for (int i = 0; i < cIndex; i++)
		if (code[i].opcode == 5) // 5 == CAL
			code[i].m = table[code[i].m].addr;
}

void block()
{
	level++;
	int procIndex = tIndex - 1;
	int x = var_declaration();
	proc_declaration();
	table[procIndex].addr = cIndex;

	if (error == 0)
	{
		emit(6, 0, 0, x); // INC == 6
		statement();
		if (error == 0)
		{
			mark();
			level++;
		}
	}

	level--;
}

//        X  X  X  X  X  X
// errors 2, 3, 4, 5, 6, 7
// returns 0 if we encounter an error
// else, returns the number of memory spaces declared
int var_declaration()
{
	int memorysize = 3;
	int arraysize;
	char *symbolname;

	if (tokens[lIndex].type == varsym)
	{
		do
		{
			lIndex++;
			if (tokens[lIndex].type != identsym)
			{
				error = 2;
				return 0;
			}
			
			if (multipledeclarationcheck(tokens[lIndex].name) != -1)
			{
				error = 3;
				return 0;
			}

			strcpy(symbolname, tokens[lIndex].name);
			lIndex++;

			if (tokens[lIndex].type == lbracketsym)
			{
				lIndex++;
				if (tokens[lIndex].type != numbersym || tokens[lIndex].value == 0)
				{
					error = 4;
					return 0;
				}

				arraysize = tokens[lIndex].value;

				lIndex++;

				if (tokens[lIndex].type == multsym || tokens[lIndex].type == divsym || 
					tokens[lIndex].type == modsym || tokens[lIndex].type == addsym ||
					tokens[lIndex].type == subsym)
				{
					error = 4;
					return 0;
				}
				else if (tokens[lIndex].type != rbracketsym)
				{
					error = 5;
					return 0;
				}

				lIndex++;

				addToSymbolTable(2, symbolname, arraysize, level, memorysize, 0);
				memorysize += arraysize;
			}
			else
			{
				addToSymbolTable(1, symbolname, 0, level, memorysize, 0);
			}
		} while (tokens[lIndex].type == commasym);
		
		if (tokens[lIndex].type == identsym)
		{
			error = 6;
			return 0;
		}
		else if (tokens[lIndex].type != semicolonsym)
		{
			error = 7;
			return 0;
		}

		lIndex++;
	}
	else
	{
	return memorysize;
	}

}

//        X  X  X  X
// errors 2, 3, 7, 8
void proc_declaration()
{
	char *symbolname;

	while (tokens[lIndex].type == procsym)
	{
		lIndex++;
		if (tokens[lIndex].type != identsym)
		{
			error = 2;
			return;
		}
		
		if (multipledeclarationcheck(tokens[lIndex].name) != -1)
		{
			error = 3;
			return;
		}

		strcpy(symbolname, tokens[lIndex].name);
		lIndex++;

		if (tokens[lIndex].type != semicolonsym)
		{
			error = 8;
			return;
		}

		lIndex++;
		addToSymbolTable(3, symbolname, 0, level, 0, 0);

		block();

		if (tokens[lIndex].type != semicolonsym)
		{
			error = 7;
			return;
		}	

		lIndex++;

		// emit RET
		emit(2, 0, 0, 0);
	}
}

void statement()
{
	
}

void condition()
{
	expression();
	if (tokens[lIndex].type == eqlsym)
	{
		lIndex++;
		expression();
		emit(18, registerCounter - 1, registerCounter - 1, registerCounter);
		registerCounter--;
	}
	else if (tokens[lIndex].type == neqsym)
	{
		lIndex++;
		expression();
		emit(19, registerCounter - 1, registerCounter - 1, registerCounter);
		registerCounter--;
	}
	else if (tokens[lIndex].type == lsssym)
	{
		lIndex++;
		expression();
		emit(20, registerCounter - 1, registerCounter - 1, registerCounter);
		registerCounter--;
	}
	else if (tokens[lIndex].type == leqsym)
	{
		lIndex++;
		expression();
		emit(21, registerCounter - 1, registerCounter - 1, registerCounter);
		registerCounter--;
	}
	else if (tokens[lIndex].type == gtrsym)
	{
		lIndex++;
		expression();
		emit(22, registerCounter - 1, registerCounter - 1, registerCounter);
		registerCounter--;
	}
	else if (tokens[lIndex].type == geqsym)
	{
		lIndex++;
		expression();
		emit(23, registerCounter - 1, registerCounter - 1, registerCounter);
		registerCounter--;
	}
	else
	{
		error = 21;
		return;
	}
}

void expression()
{
	if (tokens[lIndex].type == subsym)
	{
		lIndex++;
		term();
		emit(12, registerCounter, 0, registerCounter);
		while (tokens[lIndex].type == addsym || tokens[lIndex].type == subsym)
		{
			if (tokens[lIndex].type == addsym)
			{
				lIndex++;
				term();
				emit(13, registerCounter - 1, registerCounter - 1, registerCounter);
				registerCounter--;
			}
			else
			{
				lIndex++;
				term();
				emit(14, registerCounter - 1, registerCounter - 1, registerCounter);
				registerCounter--;
			}
		}
	}
	else
	{
		term();
		while (tokens[lIndex].type == addsym || tokens[lIndex].type == subsym)
		{
			if (tokens[lIndex].type == addsym)
			{
				lIndex++;
				term();
				emit(13, registerCounter - 1, registerCounter - 1, registerCounter);
				registerCounter--;
			}
			else
			{
				lIndex++;
				term();
				emit(14, registerCounter - 1, registerCounter - 1, registerCounter);
				registerCounter--;
			}
		}
		if (tokens[lIndex].type == lparenthesissym || tokens[lIndex].type == identsym || tokens[lIndex].type == numbersym)
		{
			error = 22;
			return;
		}
	}
}

void term()
{
	factor();
	while (tokens[lIndex].type == multsym || tokens[lIndex].type == divsym || tokens[lIndex].type == modsym)
	{
		if (tokens[lIndex].type = multsym)
		{
			lIndex++;
			factor();
			emit(15, registerCounter - 1, registerCounter - 1, registerCounter);
			registerCounter--;
		}
		else if (tokens[lIndex].type == divsym)
		{
			lIndex++;
			factor();
			emit(16, registerCounter - 1, registerCounter - 1, registerCounter);
			registerCounter--;
		}
		else 
		{
			lIndex++;
			factor();
			emit(17, registerCounter - 1, registerCounter - 1, registerCounter);
			registerCounter--;
		}
	}
}

void factor()
{
	char * symbolname;
	int symidx;
	int arrayidxreg;
	int varlocreg;
	
	if (tokens[lIndex].type == identsym)
	{
		strcpy(symbolname, tokens[lIndex].name);
		lIndex++;
		if (tokens[lIndex].type == lbracketsym)
		{
			lIndex++;
			symidx = findsymbol(symbolname, 2);
			if (symidx == -1)
			{
				if (findsymbol(symbolname, 1) != -1)
				{
					error = 11;
					return;
				}
				else if (findsymbol(symbolname, 3) != -1)
				{
					error = 9;
					return;
				}
				else
				{
					error = 10;
					return;
				}
			}
			expression();
			arrayidxreg = registerCounter;
			if (tokens[lIndex].type != rbracketsym)
			{
				error = 5;
				return;
			}
			lIndex++;
			registerCounter++;
			if (registerCounter >= 10)
			{
				error = 14;
				return;
			}
			emit(1, registerCounter, 0, table[symidx].addr);
			emit(13, arrayidxreg, arrayidxreg, registerCounter);
			registerCounter--;
			emit(3, registerCounter, level - table[symidx].level, arrayidxreg);
		}
		else
		{
			symidx = findsymbol(symbolname, 1);
			if (symidx == -1)
			{
				if (findsymbol(symbolname, 2) != -1)
				{
					error = 12;
					return;
				}
				else if (findsymbol(symbolname, 3) != -1)
				{
					error = 9;
					return;
				}
				else
				{
					error = 10;
					return;
				}
			}
			registerCounter++;
			if (registerCounter >= 10)
			{
				error = 14;
				return;
			}
			emit(1, registerCounter, 0, table[symidx].addr);
			varlocreg = registerCounter;
			emit(3, registerCounter, level - table[symidx].level, varlocreg);
		}
	}
	else if (tokens[lIndex].type == numbersym)
	{
		registerCounter++;
		if (registerCounter >= 10)
		{
			error = 14;
			return;
		}
		emit(1, registerCounter, 0, tokens[lIndex].value);
		lIndex++;
	}
	else if (tokens[lIndex].type == lparenthesissym)
	{
		lIndex++;
		expression();
		if (tokens[lIndex].type != rparenthesissym)
		{
			error = 23;
			return;
		}
		lIndex++;
	}
	else
	{
		error = 24;
		return;
	}
}