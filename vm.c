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

	char *opname; // the name of the operation we're currently doing
	int halt = 0; // boolean to tell us if we need to halt
	int currLine = 0; // the line we're on
	int index; // index we use for the base() function in LOD and STO

	// keep this
	if (printFlag)
	{
		printf("\t\t\t\t\tPC\tSP\tBP\n");
		printf("Initial values:\t\t\t\t%d\t%d\t%d\n", PC, SP, BP);
	}

	// iffy about the condition of this while loop
	while (!halt)
	{
		currLine = PC;

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
				index = base(IR.l, BP, stack) - RF[IR.m];

				if ((index < 0) || (index >= MAX_STACK_LENGTH))
				{
					printf("Virtual Machine Error: Out of Bounds Access Error\n");
					halt = 1;
				}

				SP = index;
				RF[IR.r] = stack[SP];
				break;
			case 4:
				opname = "STO";
				index = base(IR.l, BP, stack) - RF[IR.m];
				if ((index < 0) || (index >= MAX_STACK_LENGTH))
				{
					printf("Virtual Machine Error: Out of Bounds Access Error\n");
					halt = 1;
				}

				SP = index;
				stack[SP] = RF[IR.r];
				break;
			case 5:
				opname = "CAL";
				stack[SP+1] = base(IR.l, BP, stack);
				stack[SP+2] = BP;
				stack[SP+3] = PC;
				BP = SP + 1;
				PC = IR.m;
				break;
			case 6:
				opname = "INC";
				SP = SP - IR.m;
				if (SP < 0)
				{
					printf("Virtual Machine Error: Stack Overflow Error\n");
					halt = 1;
				}
				break;
			case 7:
				opname = "JMP";
				PC = IR.m;
				break;
			case 8:
				opname = "JPC";
				if (RF[IR.r] == 0)
					PC = IR.m;
				break;
			case 9:
				opname = "WRT";
				printf("Writing value %d\n", RF[IR.r]);
				break;
			case 10:
				opname = "RED";
				printf("Enter the value for R: ");
				scanf("%d", &RF[IR.r]);
				break;
			case 11:
				opname = "HAL";
				halt = 1;
				break;
			case 12:
				opname = "NEG";
				// negate the register R
				RF[IR.r] = -1 * RF[IR.r];
				break;
			case 13:
				opname = "ADD";
				// add the registers L and M and store the result in register R
				RF[IR.r] = RF[IR.l] + RF[IR.m];
				break;
			case 14:
				opname = "SUB";
				// subtract register M from register L (L-M) and store the result in register R
				RF[IR.r] = RF[IR.l] - RF[IR.m];
				break;
			case 15:
				opname = "MUL";
				// multiply registers L and M and store the result in register R
				RF[IR.r] = RF[IR.l] * RF[IR.m];
				break;
			case 16:
				opname = "DIV";
				// divide register L by register M (L/M) and store the result in register R
				RF[IR.r] = RF[IR.l] / RF[IR.m];
				break;
			case 17:
				opname = "MOD";
				// set register R equal to register L modulo register M
				RF[IR.r] = RF[IR.l] % RF[IR.m];
				break;
			case 18:
				opname = "EQL";
				// if register L equals register M, set register R to 1.
				// otherwise, set register R to 0
				RF[IR.r] = (RF[IR.l] == RF[IR.m]);
				break;
			case 19:
				opname = "NEQ";
				// if register L does not equal register M, set register R to 1.
				// otherwise, set register R to 0.
				RF[IR.r] = (RF[IR.l] != RF[IR.m]);
				break;
			case 20:
				opname = "LSS";
				// if register L is less than register M, set register R to 1.
				// otherwise, set register R to 0.
				RF[IR.r] = (RF[IR.l] < RF[IR.m]);
				break;
			case 21:
				opname = "LEQ";
				// if register L is less than or equal to register M, set register R to 1.
				// otherwise, set register R to 0.
				RF[IR.r] = (RF[IR.l] <= RF[IR.m]);
				break;
			case 22:
				opname = "GTR";
				// if register L is greater than register M, set register R to 1.
				// otherwise, set register R to 0.
				RF[IR.r] = (RF[IR.l] > RF[IR.m]);
				break;
			case 23:
				opname = "GEQ";
				// if register L is greater than or equal to register M, set register R to 1.
				// otherwise, set register R to 0.
				RF[IR.r] = (RF[IR.l] >= RF[IR.m]);
				break;
		}

		if (printFlag)
		{
			print_execution(currLine, opname, IR, PC, BP, SP, stack, RF);
		}

	}

	free(stack);
	free(RF);
	free(opname);
}
