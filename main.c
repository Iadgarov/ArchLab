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

int mem[MEM_SIZE] = {0};
int reg[8] = {0};
int PC = 0;
int counter = 0;
FILE *input, *output;
char *filename;

typedef struct Instruction {
	int opcode;
	int dst, src0, src1;
	int immediate;
} Instruction;


Instruction parseInst() {
	int inst = mem[PC];

	Instruction iinst;

	iinst.immediate = 0xffff & inst;

	inst >>= 16;
	iinst.src1 = inst & 0x7;

	inst >>= 3;
	iinst.src0 = inst & 0x7;

	inst >>= 3;
	iinst.dst = inst & 0x7;

	inst >>= 3;
	iinst.opcode = inst & 0x1f;

	return iinst;
}

void execute(Instruction inst) {
	int op1, op2;
	op1 = (inst.src0 == 1 ? inst.immediate : reg[inst.src0]);
	op2 = (inst.src1 == 1 ? inst.immediate : reg[inst.src1]);

	switch (inst.opcode) {

	case ADD:
		reg[inst.dst] = op1 + op2;
		break;

	case SUB:
		reg[inst.dst] = op1 - op2;
		break;

	case LSF:
		reg[inst.dst] = op1 << op2;
		break;

	case RSF:
		reg[inst.dst] = op1 >> op2;
		break;

	case AND:
		reg[inst.dst] = op1 & op2;
		break;

	case OR:
		reg[inst.dst] = op1 | op2;
		break;

	case XOR:
		reg[inst.dst] = op1 ^ op2;
		break;

	case LHI:
		int x = reg[inst.dst] & 0xffff;
		reg[inst.dst] = (inst.immediate << 16) | x;
		break;

	case LD:
		reg[inst.dst] = mem[reg[op2] & 0xffff];
		break;

	case ST:
		mem[reg[op2] & 0xffff] = reg[op1];
		break;

	case JLT:
		if (op1 < op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		break;

	case JLE:
		if (op1 <= op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		break;

	case JEQ:
		if (op1 == op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		break;

	case JNE:
		if (op1 != op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		break;

	case JIN:
		reg[7] = (PC == 0 ? 0xffff : PC - 1);
		PC = reg[op1];
		break;

	}
}

void writeTrace1(FILE *output, Instruction *inst) {
	fprintf(output, "--- instruction %d (%04x) @ PC %d (%04x) -----------------------------------------------------------\n \
	pc = %04x, inst = %08x, opcode = %d (%s), dst = %d, src0 = %d, src1 = %d, immediate = %16x\n \
	r[0] = %08x r[1] = %08x r[2] = %08x r[3] = %08x\n \
	r[4] = %08x r[5] = %08x r[6] = %08x r[7] = %08x\n\n",
		counter, counter,
		PC, PC, PC,
		mem[PC],
		inst->opcode, inst->dst, inst->src0, inst->src1, inst->immediate,
		reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
}

void run() {
	Instruction inst;

	output = fopen("trace.txt", "w");

	while (1) {
		inst = parseInst();
		if (inst.opcode == HLT) {
			break;
		}

		if (PC == 0xffff) {
			PC = 0;
		}
		else {
			PC++;
		}

		writeTrace1(output, &inst);

		execute(inst);

		fprintf(output, ">>>> EXEC: R[%d] = %d ADD 0 <<<<\n\n");

		counter++;

	}

	fclose(output);

}

void readMem() {
	input = fopen(filename, "r");

	int addr = 0;
	while (fgets(line, 9, input) != NULL) {
		mem[addr] = strtol(line, NULL, 16);
		addr++;
	}

	fclose(input);
}




