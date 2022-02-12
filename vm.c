/*
	You can use these two print statements for the errors:
		printf("Virtual Machine Error: Stack Overflow Error\n");
		printf("Virtual Machine Error: Out of Bounds Access Error\n");
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define REG_FILE_SIZE 10
#define MAX_STACK_LENGTH 100

void print_execution(int line, char *opname, instruction IR, int PC, int BP, int SP, int *stack, int *RF)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t\t", line, opname, IR.r, IR.l, IR.m, PC, SP, BP);
	
	// print register file
	for (i = 0; i < REG_FILE_SIZE; i++)
		printf("%d ", RF[i]);
	printf("\n");
	
	// print stack
	printf("stack:\t");
	for (i = MAX_STACK_LENGTH - 1; i >= SP; i--)
		printf("%d ", stack[i]);
	printf("\n");
}

int base(int L, int BP, int *stack)
{
	int ctr = L;
	int rtn = BP;
	while (ctr > 0)
	{
		rtn = stack[rtn];
		ctr--;
	}
	return rtn;
}

void execute_program(instruction *code, int printFlag)
{
	// setting initial values
	int BP = MAX_STACK_LENGTH - 1;
	int SP = BP + 1;
	int PC = 0;
	instruction IR;
	int *stack = calloc(MAX_STACK_LENGTH, sizeof(int));
	int *RF = calloc(REG_FILE_SIZE, sizeof(int)); // register file

	char *opname;
	int halt = 0;

	// keep this
	if (printFlag)
	{
		printf("\t\t\t\t\tPC\tSP\tBP\n");
		printf("Initial values:\t\t\t\t%d\t%d\t%d\n", PC, SP, BP);
	}

	// iffy about the condition of this while loop
	while (!halt)
	{
		// fetch cycle
		IR = code[PC];
		PC++;

		// execute cycle
		switch (IR.opcode)
		{
			case 1:
				opname = "LIT";
				SP++;
				stack[SP] = IR.m;
				break;
			case 2:
				opname = "RET";
				SP = BP + 1;
				BP = stack[SP+2]; // dynamic link?
				PC = stack[SP+3]; // return address?
				break;
			case 3:
				opname = "LOD";
				int index = base(IR.l, BP, stack) - RF[IR.m];
				if ((index < 0) || (index >= MAX_STACK_LENGTH))
				{
					printf("Virtual Machine Error: Out of Bounds Access Error\n");
					halt = 1;
				}

				break;
			case 4:
				opname = "STO";
				break;
			case 5:
				opname = "CAL";
				break;
			case 6:
				opname = "INC";
				break;
			case 7:
				opname = "JMP";
				break;
			case 8:
				opname = "JPC";
				break;
			case 9:
				opname = "WRT";
				break;
			case 10:
				opname = "RED";
				break;
			case 11:
				opname = "HAL";
				break;
			case 12:
				opname = "NEG";
				break;
			case 13:
				opname = "ADD";
				break;
			case 14:
				opname = "SUB";
				break;
			case 15:
				opname = "MUL";
				break;
			case 16:
				opname = "DIV";
				break;
			case 17:
				opname = "MOD";
				break;
			case 18:
				opname = "EQL";
				break;
			case 19:
				opname = "NEQ";
				break;
			case 20:
				opname = "LSS";
				break;
			case 21:
				opname = "LEQ";
				break;
			case 22:
				opname = "GTR";
				break;
			case 23:
				opname = "GEQ";
				break;
		}
	}

	free(stack);
	free(RF);
	free(opname);
}
