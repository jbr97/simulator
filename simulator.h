/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "write.h"

uint64_t flowing(void *ins_addr, uint64_t PC, uint64_t *reg)
{	
	total_cycles = total_cycles + 1;

	// pipeline flowing
	if (!IW.cont_stall)
		IW = IM;
	if (!IM.cont_stall)
		IM = IE;
	if (!IE.cont_stall)
		IE = ID;
	if (!ID.cont_stall)
		ID = IF;

	// fetch
	if (!IF.cont_stall)
		ifetch(ins_addr);

	// decode
	if (ID.PC != PC_END)
		idecode();

	// memory
	if (IM.PC != PC_END)
		imemory();

	// write back
	if (IW.PC != PC_END)
		iwrite();

	// execute is the latest because of forwarding
	if (IE.PC != PC_END)
		iexecute();
}

uint64_t flowing_nopipe(void *ins_addr, uint64_t PC, uint64_t *reg)
{	
	// flowing no pipe
	IF.init();
	ID.init();
	IE.init();
	IM.init();
	IW.init();

	uint64_t total_cycles_old = total_cycles;

	ifetch(ins_addr);
	ID = IF;
	idecode();
	IE = ID;
	iexecute();
	IM = IE;
	imemory();
	IW = IM;
	iwrite();

	// this instr is not MUL or DIV
	if (total_cycles - total_cycles_old == 1)
		total_cycles += 4;
}

void Init();
void sim_display();
int sim_step(int count);
void sim_run();
void sim_showmem();
void simulate();
