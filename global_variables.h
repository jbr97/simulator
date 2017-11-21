/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#ifndef __GLOBALVARIABLE_H__
#define __GLOBALVARIABLE_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "Read_Elf.h"

#define REG_NULL UINT_MAX

struct FORWARD {

// for bubble & stall cont
	bool cont_stall;

// fetch
	uint32_t instr;
	uint64_t PC;

// decode
	uint32_t opcode;
	uint32_t func7;
	uint32_t func3;
	uint32_t rs1;
	uint32_t rs2;
	uint32_t rd;

// execute
	uint64_t alu_ans;

// memory
	bool cont_readmem;
	bool cont_writemem;
	uint32_t mem_width;
	uint64_t imm;
	uint64_t entry;

// write back
	bool cont_reg;
	bool cont_PC;
	uint64_t new_PC; 
	
// initialize when ifetch
	void init()
	{
		instr = ~0x0;
		PC = 0x0;
		
		opcode = 0x0;
		func7 = 0x0;
		func3 = 0x0;
		rs1 = 0x0;
		rs2 = 0x0;
		rd = 0x0;
		
		alu_ans = 0x0;
		
		cont_readmem = false;
		cont_writemem = false;
		mem_width = 0;
		imm = 0x0;
		entry = 0x0;
		
		cont_reg = false;
		cont_PC = false;
		new_PC = 0x0;
	}
};

FORWARD IF, ID, IE, IM, IW; // all info of instrs

uint64_t reg[32];   // register file
uint64_t PC;        // PC pointer
uint64_t PC_offset;    // code offset

bool exit_flag = 0; // cmd exit flag

uint64_t total_cycles = 0; // total running cycles
uint64_t total_instrs = 0; // total instructions

#endif
