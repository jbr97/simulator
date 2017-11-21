/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#ifndef __FETCH_H__
#define __FETCH_H__

#include "global_variables.h"


void ifetch(void *ins_addr)
{
	uint32_t instr = * ((uint32_t *) ins_addr);
	
	// init struct 
	IF.init();
	// fetch instr
	IF.instr = instr;
	IF.PC = PC;
	// renew PC
	PC = PC + 4;

	total_instrs++;
}

#endif
