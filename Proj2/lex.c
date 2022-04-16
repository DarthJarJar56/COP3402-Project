#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

// The declare the list that we're returning to the main function
lexeme *list;
int lex_index;
int list_index;

int reservedcheck(char *buffer);
void printlexerror(int type);
void printtokens();


lexeme *lexanalyzer(char *input, int printFlag)
{
	// Allocate memory for the list of lexemes
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;
	list_index = 0;
	
	// Token and loop variables
	int i = 0;
	lexeme currToken;

	// Counters for errors and a variable for reserved word type
	int idenCounter = 0;
	int numCounter = 0;
	int reserved = 0;

	// Create a buffer for storing input
	char *buffer = malloc(sizeof(char) * 12);

	// Loop over the entire input
	while(input[i] != '\0')
	{
		// Put the current input into the buffer
		buffer[lex_index] = input[i];
		lex_index++;
		// If we have a comment, ignore it
		if (buffer[0] == '/')
		{
			if (input[i+1] == '/')
			{
				while((!iscntrl(input[i])) && input[i] != '\0')
				{
					i++;
				}

				continue;
			}
		}

		// Create a new token on space or newline
		if (isspace(input[i]) || iscntrl(input[i]))
		{
			
			lexeme temp;
			currToken = temp;
			i++;
			idenCounter = 0;
			numCounter = 0;
			lex_index = 0;
			buffer = malloc(sizeof(char) * 12);
			continue;
		}

		// If the current input is a letter, keep going until either an error occurs
		// or you hit something that isn't a letter or number
		if (isalpha(input[i]))
		{
			idenCounter++;
			i++;

			while (input[i] != '\0' && idenCounter <= 11 && (isalpha(input[i]) || isdigit(input[i])))
			{
				idenCounter++;
				buffer[lex_index] = input[i];
				i++;
				lex_index++;
			}

			// Error if indetifier is too large
			if (idenCounter > 11)
			{
				printlexerror(3);
				return NULL;
			}
			// Add end of line character to use strcmp in reserved check
			buffer[lex_index] = '\0';
			lex_index++;
			// Check if the indentifier is a reserved word
			reserved = reservedcheck(buffer);

			if (reserved != 0)
			{
				currToken.type = reserved;
			}
			else
			{
				currToken.type = identsym;
			}
			// Update current token, add to list
			strcpy(currToken.name, buffer);
			list[list_index] = currToken;
			list_index++;
			continue;

			
		}

		// If current input is a number, keep going until you hit an error or symbol/space
		else if (isdigit(input[i]))
		{
			numCounter++;
			i++;

			while (isdigit(input[i]) && numCounter <= 5)
			{
				numCounter++;
				buffer[lex_index++] = input[i++];
			}
			
			// Throw appropriate errors
			if (numCounter > 5)
			{
				printlexerror(2);
				return NULL;
			}

			if (isalpha(input[i]))
			{
				printlexerror(1);
				return NULL;
			}

			// Convert string to int, update token, and add it to the list
			currToken.value = atoi(buffer);
			currToken.type = numbersym;
			list[list_index] = currToken;
			list_index++;
			continue;
		}

		else
		{
			// If the current input isn't a letter, digit, or whitespace, it's a symbol
			// Create new token, reset buffer
			lexeme temp;
			currToken = temp;
			buffer = malloc(sizeof(char) * 12);
			idenCounter = 0;
			numCounter = 0;
			lex_index = 0;

			buffer[lex_index] = input[i];

			// Update the current token accordingly
			switch(input[i])
			{
				case '.': 
					strcpy(currToken.name, buffer);
					currToken.type = periodsym;
					break;
				case '[':
					strcpy(currToken.name, buffer);
					currToken.type = lbracketsym;
					break;
				case ']':
					strcpy(currToken.name, buffer);
					currToken.type = rbracketsym;
					break;
				case ',':
					strcpy(currToken.name, buffer);
					currToken.type = commasym;
					break;
				case ';':
					strcpy(currToken.name, buffer);
					currToken.type = semicolonsym;
					break;
				case ':':
					if (input[i+1] == '=')
					{
						strcpy(currToken.name, ":=");
						i++;
						currToken.type = assignsym;
					}
					else
					{
						strcpy(currToken.name, buffer);
						currToken.type = colonsym;
					}
					break;
				case '?':
					strcpy(currToken.name, buffer);
					currToken.type = questionsym;
					break;
				case '(':
					strcpy(currToken.name, buffer);
					currToken.type = lparenthesissym;
					break;
				case ')':
					strcpy(currToken.name, buffer);
					currToken.type = rparenthesissym;
					break;
				case '=' :
					if (input[i + 1] == '=')
					{
						strcpy(currToken.name, "==");
						currToken.type = eqlsym;
						i++;
					}
					break;
				case '<':
					switch (input[i+1])
					{
						case '>':
							strcpy(currToken.name, "<>");
							currToken.type = neqsym;
							i++;
							break;
						case '=':
							strcpy(currToken.name, "<=");
							currToken.type = leqsym;
							i++;
							break;
						default:
							strcpy(currToken.name, buffer);
							currToken.type = lsssym;
							break;
					}
					break;
				case '>':
					if (input[i+1] == '=')
					{
						strcpy(currToken.name, ">=");
						currToken.type = geqsym;
						i++;
					}
					else
					{
						strcpy(currToken.name, buffer);
						currToken.type = gtrsym;
					}
					break;
				case '+':
					strcpy(currToken.name, buffer);
					currToken.type = addsym;
					break;
				case '-':
					strcpy(currToken.name, buffer);
					currToken.type = subsym;
					break;
				case '*':
					strcpy(currToken.name, buffer);
					currToken.type = multsym;
					break;
				case '/':
					strcpy(currToken.name, buffer);
					currToken.type = divsym;
					break;
				case '%':
					strcpy(currToken.name, buffer);
					currToken.type = modsym;
					break;
				default:
					printlexerror(4);
					return NULL;
			}

		}
		// Add token to the list
		list[list_index++] = currToken;

		i++;
	}

	if (printFlag)
			printtokens();

	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < list_index; i++)
	{
		switch (list[i].type)
		{
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case lbracketsym:
				printf("%11s\t%d", "[", lbracketsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case rbracketsym:
				printf("%11s\t%d", "]", rbracketsym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case questionsym:
				printf("%11s\t%d", "?", questionsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case colonsym:
				printf("%11s\t%d", ":", colonsym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case lparenthesissym:
				printf("%11s\t%d", "(", lparenthesissym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case rparenthesissym:
				printf("%11s\t%d", ")", rparenthesissym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			default:
				printf("%11s\t%s", "err", "err");
				break;
		}
		printf("\n");
	}
	printf("\n");
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Number Length\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Identifier Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}

int reservedcheck(char *buffer)
{
	// Check for reserved words, return their enum if there's a match
	if (strcmp(buffer, "var") == 0)
	{
		return varsym;
	}

	if (strcmp(buffer, "procedure") == 0)
	{
		return procsym;
	}

	if (strcmp(buffer, "call") == 0)
	{
		return callsym;
	}

	if (strcmp(buffer, "begin") == 0)
	{
		return beginsym;
	}

	if (strcmp(buffer, "end") == 0)
	{
		return endsym;
	}

	if (strcmp(buffer, "if") == 0)
	{
		return ifsym;
	}

	if (strcmp(buffer, "do") == 0)
	{
		return dosym;
	}

	if (strcmp(buffer, "while") == 0)
	{
		return whilesym;
	}

	if (strcmp(buffer, "read") == 0)
	{
		return readsym;
	}

	if (strcmp(buffer, "write") == 0)
	{
		return writesym;
	}

	return 0;


}