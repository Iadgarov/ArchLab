/*
 * SP ASM: Simple Processor assembler
 *
 * usage: asm
 */
#include <stdio.h>
#include <stdlib.h>

#define ADD 0
#define SUB 1
#define LSF 2
#define RSF 3
#define AND 4
#define OR  5
#define XOR 6
#define LHI 7
#define LD 8
#define ST 9
#define JLT 16
#define JLE 17
#define JEQ 18
#define JNE 19
#define JIN 20
#define HLT 24

#define MEM_SIZE_BITS	(16)
#define MEM_SIZE	(1 << MEM_SIZE_BITS)
#define MEM_MASK	(MEM_SIZE - 1)
unsigned int mem[MEM_SIZE];

int pc = 0;

static void asm_cmd(int opcode, int dst, int src0, int src1, int immediate)
{
	int inst;

	inst = ((opcode & 0x1f) << 25) | ((dst & 7) << 22) | ((src0 & 7) << 19) | ((src1 & 7) << 16) | (immediate & 0xffff);
	mem[pc++] = inst;
}

static void assemble_program(char *program_name)
{
	FILE *fp;
	int addr, i, last_addr;

	for (addr = 0; addr < MEM_SIZE; addr++)
		mem[addr] = 0;

	pc = 0;

	/*
	 * Program starts here
	 */
	/*asm_cmd(ADD, 2, 1, 0, 15); // 0: R2 = 15
	asm_cmd(ADD, 5, 1, 0, 20); // 1: R5 = 20
	asm_cmd(ADD, 3, 1, 0, 0); // 2: 
	asm_cmd(LD,  4, 0, 2, 0); // 3:
	asm_cmd(ADD, 3, 3, 4, 0); // 4:
	asm_cmd(ADD, 2, 2, 1, 1); // 5:
	asm_cmd(JLT, 0, 2, 5, 3); // 6:
	asm_cmd(ST,  0, 3, 1, 15); // 7:
	asm_cmd(HLT, 0, 0, 0, 0); // 8:

	for (i = 0; i < 5; i++)
		mem[15+i] = i;

	last_addr = 20;*/


	// load input
	asm_cmd(ADD, 2, 0, 1, 1000); // put immediate into r2 (mem address)
	asm_cmd(ADD, 3, 0, 1, 1001); // # mem addres into r3
	asm_cmd(LD, 2, 0, 2 ,0); // # load mem[1000] into r2
	asm_cmd(LD, 3, 0, 3, 0); // # load mem[1001] into r3

	// # check the operands' sign
	// # we multiply the absolute values and add sign (r6) at the end
	// # sign = 0 if positive or 1 if negative
	asm_cmd(ADD, 6, 0, 0, 0); // # start with positive
	asm_cmd(JLE, 0, 0, 2, 8); // # jump to line 8 if R2 >= 0
	asm_cmd(XOR, 6, 6, 1, 1); // # flip sign (now negative)
	asm_cmd(SUB, 2, 0, 2, 0); // # r2 = -r2 (absolute value)

	asm_cmd(JLE, 0, 0, 3, 11); // # jump to line 11 if R3 >= 0
	asm_cmd(XOR, 6, 6, 1, 1); // # flip sign
	asm_cmd(SUB, 3, 0, 3, 0); // # r3 = -r3 (absolute value)

	// # make sure r2 <= r3 and switch them if necessary
	asm_cmd(JLE, 0, 2, 3, 15); // # jump to line 15 if R2 <= R3
	asm_cmd(ADD, 4, 2, 0, 0); // # temporarily store r2 in r4
	asm_cmd(ADD, 2, 3, 0, 0); // # move r3 to r2
	asm_cmd(ADD, 3, 4, 0, 0); // # put r4 (r2) in r3

	//# main loop - result (and temp values) stored in r4
	//# idea is to add r3 according to r2's binary representation
	//# only takes about log(r2) additions
	asm_cmd(ADD, 4, 0, 0, 0); // # r4 = 0
	asm_cmd(ADD, 5, 3, 0, 0); // # start with r5 = r3 x 2^0
	asm_cmd(JEQ, 0, 2, 0, 24); //  # exit loop when r2 = 0
	asm_cmd(AND, 7, 2, 1, 1); // # r7 = LSB(r2)
	asm_cmd(JEQ, 0, 7, 0, 21); // # skip addition if r7 = 0
	asm_cmd(ADD, 4, 4, 5, 0); // # r4 += r5
	asm_cmd(LSF, 5, 5, 1, 1); // # r5 = r5 x 2
	asm_cmd(RSF, 2, 2, 1, 1); // # r2 = r2 / 2
	asm_cmd(JIN, 0, 1, 0, 17); // # jump back to start of loop

	// # negate result if necessary
	asm_cmd(JEQ, 0, 6, 0, 26); // # jump to line 26 if positive
	asm_cmd(SUB, 4, 0, 4, 0); // # r4 = -r4

	// # write result to memory
	asm_cmd(ADD, 7, 0, 1, 1002); // # put mem address into r7
	asm_cmd(ST, 0, 4, 7, 0); // # store result
	asm_cmd(HLT, 0, 0, 0, 0);


	mem[1000] = 250;
	mem[1001] = -312;

	last_addr = 1002;



	/*asm_cmd(ADD, 2, 0, 0, 0); // # r2 = 0
	asm_cmd(ADD, 5, 0, 1, 2000); // # r5 = 2000

	//# initialize pointers to memory locations and save them in memory (weird, huh?)
	//# ptr to table[r2][r3] in address 2100
	//# ptr to table[r3][r2] in address 2101

	//# nested main loops
	//# only calculate half of the table because it's symmtrical
	asm_cmd(ADD, 5, 5, 2, 0); //# r5 = r5 + r2 (start of outer loop)
	asm_cmd(ADD, 2, 2, 1, 1); // # r2++
	asm_cmd(ADD, 6, 5, 0 ,0); // # r6 = r5 (first pointing to same location on the diagonal)
	asm_cmd(ADD, 3 , 0 , 2, 0); //# r3 = r2 (start of inner loop)
	asm_cmd(JIN, 0, 1, 0, 13); // # jump to mult method, address saved in r7
	asm_cmd(ADD ,3 ,3 ,1, 1); //# r3++
	asm_cmd(ADD, 7 ,0 ,1 ,10); // # constant 10 in R7
	asm_cmd(JLE, 0, 3, 7, 6); // # if r3 <= 10 go to inner loop
	asm_cmd(ADD, 7 ,0 ,1 ,10); // # constant 10 in R7
	asm_cmd(JEQ, 0, 2, 7, 32); // # if r2 = 10, we're done - exit both loops
	asm_cmd(JIN, 0, 1, 0, 2);// # otherwise jump to outer loop

	//# multiply method (like Q3, slightly tweaked)
	//asm_cmd(ADD, 7, 7, 1, 1); // # r7++ (return address is src address + 1)
	//asm_cmd(ST, 0, 7, 1, 2102); // # store r7 into mem[2102]
	asm_cmd(ST, 0, 5, 1, 2100);// # store updated pointer
	asm_cmd(ST, 0, 6, 1, 2101);// # store updated pointer
	asm_cmd(ADD, 6, 2, 0, 0); // # copy r2 into r6
	asm_cmd(ADD, 4, 0, 0, 0); // # r4 = 0
	asm_cmd(ADD, 5, 3, 0, 0); // # start with r5 = r3 x 2^0
	asm_cmd(JEQ, 0, 6, 0, 25); // # exit loop when r2 = 0
	asm_cmd(AND, 7, 6, 1, 1); // # r7 = LSB(r6)
	asm_cmd(JEQ, 0, 7 ,0 ,22); // # skip addition if r7 = 0
	asm_cmd(ADD, 4, 4, 5, 0); // # r4 += r5
	asm_cmd(LSF, 5, 5, 1, 1); // # r5 = r5 x 2
	asm_cmd(RSF, 6, 6, 1, 1); // # r2 = r2 / 2
	asm_cmd(JIN, 0, 1, 0, 18); // # jump back to start of mult loop

	//# calculations done, write result to memory and return to main loop
	asm_cmd(LD, 5, 0, 1, 2100); // # load pointer to table[r2][r3]
	asm_cmd(LD, 6, 0, 1, 2101); // # load pointer to table[r3][r2]
	asm_cmd(ST,0, 4 ,5, 0); //# store result into mem[r5]
	asm_cmd(ST, 0, 4, 6, 0); // # store result into mem[r6]
	asm_cmd(ADD, 5, 5, 1, 1); // # r5++ (update pointer)
	asm_cmd(ADD, 6, 6, 1, 10); // # r6 += 10 (update pointer)
	//asm_cmd(LD, 7, 0, 1, 2102); // # load return address into r7
	asm_cmd(JIN, 0, 1, 0, 7); // # back to main

	asm_cmd(HLT, 0 ,0 ,0 ,0);

	last_addr = 2099;*/

	fp = fopen(program_name, "w");
	if (fp == NULL) {
		printf("couldn't open file %s\n", program_name);
		exit(1);
	}
	addr = 0;
	while (addr < last_addr) {
		fprintf(fp, "%08x\n", mem[addr]);
		addr++;
	}
}


int main(int argc, char *argv[])
{
	printf("SP assembler\n");
	if (argc != 2)
		printf("usage: asm program_name\n");
	assemble_program(argv[1]);
	return 0;
}
