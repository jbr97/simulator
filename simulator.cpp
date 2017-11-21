/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#include "simulator.h"

// important symbols
extern uint64_t sym_PC;
extern uint64_t sym_GP;
// important sections
extern Elf64_Shdr shdr_text;

// file pointer
static void *fp;

int main(int argc, char* argv[])
{
	if (argc != 2) {
		puts("Invalid argument!");
		return 0;
	}
	
	fp = read_elf((const char *) argv[1]);
	printf("RISCV simulator start:\n");
	printf("\tfilename: %s\n", argv[1]);

	Init();

	simulate();

	return 0;
}

void Init()
{
	// init PC
	PC = sym_PC;
	
	// init regs
	reg[0] = 0;
	reg[1] = PC_END;
	_stack = malloc(MAX_STACK + 10);
	
	//init SP
	reg[2] = (uint64_t) _stack;
	reg[2] = reg[2] + MAX_STACK;

	//init GP
	reg[3] = sym_GP;
	__global_pointer = malloc((MAX_GP + 10) * 2);
	__global_pointer = (void *) ((unsigned long) __global_pointer + MAX_GP + 10);
	
	//init OFFSET
	PC_offset = shdr_text.sh_offset - shdr_text.sh_addr;
}

void sim_display()
{
	uint32_t instr;

	if (PC == PC_END)
		instr = 0xFFFFFFFF;
	else
		instr = *((uint32_t *) ((uint64_t) fp + PC + PC_offset));

	printf("PC = 0x%016lx: 0x%08x\n", PC, instr);
	for (int i = 0; i < 32; ++i) {
		printf("0x%lx ", reg[i]);
		if((i+1) % 8 == 0) printf("\n");
	}

	if (PC == PC_END)
		printf("Note: Func(main) has already finished.\n");
}

int sim_step(int count)
{
	while (count--) {
		// $0 is always zero
		reg[0] = 0;
		if(PC == PC_END) {
			// reach RET instr, stall IF & ID
			IF.cont_stall = 1;
			ID.cont_stall = 1;

			// ret in IE stage, flow one step
			flowing((void *) ((uint64_t) fp + PC + PC_offset), PC, reg);

			// ret in IM stage, flow one step
			flowing((void *) ((uint64_t) fp + PC + PC_offset), PC, reg);

			return 0;
		}
		flowing((void *) ((uint64_t) fp + PC + PC_offset), PC, reg);
	}
	return 1;
}

void sim_run()
{
	while (sim_step(1));

	//exit_flag = 0;
	printf("Program exits without error.\n");
	printf("Total_cycles is :       %9ld\n", total_cycles);
	printf("Total_instructions is : %9ld\n", total_instrs);
	printf("CPI : %.16f\n", (double) total_cycles / total_instrs);
}

void sim_showmem()
{
	uint64_t _addr;
	uint32_t _count = 0;

	scanf("%lx", &_addr);
	char _ch = getchar();
	if (_ch == ':')
		scanf("%d", &_count);
	if (_count == 0) _count = 4;

	for (int i = 0; i < _count; ++i) {
		uint32_t *np = (uint32_t *) mem_search(_addr);
		
		if (np == NULL) {
			printf("Invalid address (0x%lx) !\n", _addr);
			break;
		}

		printf("0x%016lx: 0x%08x\n", _addr, *(np));
		_addr += 4;
	}
}

void simulate()
{
	char cmd[80];
	char ch;
	int count;

	while (exit_flag == 0) {
		printf(">>");
		scanf("%s", cmd);
		switch (cmd[0]) {
			case 'r': case 'R':
				sim_run();
				break;
			case 'n': case 'N':
				count = 0;
				ch = getchar();
				if (ch == ' ')
					scanf("%d", &count);
				else
					count = 1;
				sim_step(count);
				sim_display();
				break;
			case 'd': case 'D':
				sim_display();
				break;
			case 'q': case 'Q':
				exit_flag = 1;
				break;
			case 'm': case 'M':
				sim_showmem();
				break;
			default:
				puts("Invalid command!");
		}
	}
}
