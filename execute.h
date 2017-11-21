/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#ifndef __EXECUTE_H__
#define __EXECUTE_H__

#include "myalu.h"
#include "global_variables.h"

#define _INT_EXT32(x) 	((int64_t)(int32_t)(x))
#define _UINT_EXT32(x) 	((uint64_t)(uint32_t)(x))
#define _INT_EXT64(x) 	((int64_t)(x))
#define	_UINT_EXT64(x)	((uint64_t)(x))
#define _IMM_EXT(x,y) 	((((int64_t)(x))<<(64-(y)))>>(64-(y)))

static uint64_t renew_cycles;

static void err_instr(uint32_t instr, const char *opt = NULL)
{
	if (opt != NULL)
		printf("Error instruction type: %s\n", opt);
	printf("Error! Instruction is not defined.\n");
	printf("instruction address: 0x%016lx\n", IE.PC);
	printf("instruction code: 0x%08x\n", instr);
	exit(1);
}

void exe_R(uint32_t instr)
{
	uint32_t opcode = IE.opcode;
	uint32_t rd = IE.rd;
	uint32_t rs1 = IE.rs1;
	uint32_t rs2 = IE.rs2;
	uint32_t func3 = IE.func3;
	uint32_t func7 = IE.func7;
	uint32_t reg_rs1 = reg[rs1];
	uint32_t reg_rs2 = reg[rs2];

	// if forwarding
	if (IM.rd == IE.rs1 && IM.cont_reg && IM.rd != 0)
		reg_rs1 = IM.alu_ans;
	if (IM.rd == IE.rs2 && IM.cont_reg && IM.rd != 0)
		reg_rs2 = IM.alu_ans;

	if (opcode == 0x33) {
		if (func3 == 0x0) {
			if (func7 == 0x00) { // add
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 + reg_rs2;
			}
			else if (func7 == 0x01) { // mul
				renew_cycles = 2;
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 * reg_rs2;
			}
			else if (func7 == 0x20) { // sub
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 - reg_rs2;
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x1) {
			if (func7 == 0x00) { // sll
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 << reg_rs2;
			}
			else if (func7 == 0x01) { // mulh
				renew_cycles = 2;
				IE.cont_reg = 1;
				IE.alu_ans = mulh(reg_rs1, reg_rs2);
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x2) {
			if (func7 == 0x00) { // slt
				IE.cont_reg = 1;
				IE.alu_ans = ((int64_t) reg_rs1 < (int64_t) reg_rs2) ? 1 : 0;
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x4) {
			if (func7 == 0x00) { // xor
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 ^ reg_rs2;
			}
			else if (func7 == 0x01) { // div
				// TODO cannot in the pipeline
				renew_cycles = 40;
				IE.cont_reg = 1;
				IE.alu_ans = div(reg_rs1, reg_rs2, 0);
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x5) {
			if (func7 == 0x00) { // srl
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 >> reg_rs2;
			}
			else if (func7 == 0x20) { // sra
				IE.cont_reg = 1;
				IE.alu_ans = (uint64_t) (((int64_t) reg_rs1) >> reg_rs2);
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x6) {
			if (func7 == 0x00) { // or
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 | reg_rs2;
			}
			else if (func7 == 0x00) { // rem
				// TODO if last command is DIV, renew_cycles = 0
				renew_cycles = 40;
				IE.cont_reg = 1;
				IE.alu_ans = div(reg_rs1, reg_rs2, 1);
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x7) {
			if (func7 == 0x00) { // and
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 & reg_rs2;
			}
			else
				err_instr(instr, "R");
		}
		else
			err_instr(instr, "R");
	}
	else if (opcode == 0x3B) {
		if (func3 == 0x0) {
			if (func7 == 0x00) { // addw
				IE.cont_reg = 1;
				IE.alu_ans = (int32_t) reg_rs1 + reg_rs2;
			}
			else if (func7 == 0x01) { // mulw
				IE.cont_reg = 1;
				IE.alu_ans = (int32_t) reg_rs1 * reg_rs2;
			}
			else if (func7 == 0x20) { // subw
				IE.cont_reg = 1;
				IE.alu_ans = (int32_t) reg_rs1 - reg_rs2;
			}
			else
				err_instr(instr, "R");
		}
		else if (func3 == 0x4) {
			if (func7 == 0x01) { // divw
				renew_cycles = 40;
				IE.cont_reg = 1;
				IE.alu_ans = divw(reg_rs1, reg_rs2, 0);
			}
			else
				err_instr(instr, "R");
		}
		else
			err_instr(instr, "R");
	}
	else
		err_instr(instr, "R");
}

void exe_I(uint32_t instr)
{
	uint32_t opcode = IE.opcode;
	uint32_t rd = IE.rd;
	uint32_t rs1 = IE.rs1;
	uint32_t func3 = IE.func3;
	uint64_t imm = (instr & 0xFFF00000) >> 20;
	uint32_t reg_rs1 = reg[rs1];

	// if forwarding
	if (IM.rd == IE.rs1 && IM.cont_reg && IM.rd != 0)
		reg_rs1 = IM.alu_ans;

	if (opcode == 0x03) {
		if (func3 == 0x0) { // lb
			IE.cont_reg = 1;
			IE.cont_readmem = 1;
			IE.mem_width = 8;
			IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		}
		else if (func3 == 0x1) { // lh
			IE.cont_reg = 1;
			IE.cont_readmem = 1;
			IE.mem_width = 16;
			IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		}
		else if (func3 == 0x2) { // lw
			IE.cont_reg = 1;
			IE.cont_readmem = 1;
			IE.mem_width = 32;
			IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		}
		else if (func3 == 0x3) { // ld
			IE.cont_reg = 1;
			IE.cont_readmem = 1;
			IE.mem_width = 64;
			IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		}
		else
			err_instr(instr, "I");
	}
	else if (opcode == 0x13) {
		if (func3 == 0x0) { // addi
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 + _IMM_EXT(imm, 12);
		}
		else if (func3 == 0x1) { // slli
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 << (imm & 0x3F);
		}
		else if (func3 == 0x2) { // slti
			IE.cont_reg = 1;
			IE.alu_ans = ((int64_t) reg_rs1 < (int64_t) _IMM_EXT(imm, 12)) ? 1 : 0;
		}
		else if (func3 == 0x4) { // xori
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 ^ _IMM_EXT(imm, 12);
		}
		else if (func3 == 0x5) { // srli & srai
			uint64_t funcimm = (imm & 0xFFF) >> 5;
			if (funcimm == 0x00) { // srli
				IE.cont_reg = 1;
				IE.alu_ans = reg_rs1 >> (imm & 0x3F);
			}
			else if (funcimm == 0x20) { // srai
				IE.cont_reg = 1;
				IE.alu_ans = (int64_t) reg_rs1 >> (imm & 0x3F);
			}
			else
				err_instr(instr, "I");
		}
		else if (func3 == 0x6) { // ori
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 | _IMM_EXT(imm, 12);
		}
		else if (func3 == 0x7) { // andi
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 & _IMM_EXT(imm, 12);
		}
		else
			err_instr(instr, "I");
	}
	else if (opcode == 0x1B) {
		uint64_t func7 = IE.func7;

		if (func3 == 0x0) { // addiw
			IE.cont_reg = 1;
			IE.alu_ans = (int32_t) (((int32_t) reg_rs1 + _IMM_EXT(imm, 12)) & 0xFFFFFFFF);
		}
		else if (func7 == 0x00 && func3 == 0x1) { // slliw
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 << (imm & 0xF);
		}
		else if (func7 == 0x00 && func3 == 0x5) { // srliw
			IE.cont_reg = 1;
			IE.alu_ans = reg_rs1 >> (imm & 0xF);
		}
		else if (func7 == 0x20 && func3 == 0x5) { // sraiw
			IE.cont_reg = 1;
			IE.alu_ans = (int64_t) reg_rs1 >> (imm & 0xF);
		}
		else 
			err_instr(instr, "I");
	}
	else if (opcode == 0x67) {
		if (func3 == 0x0) { // jalr
			// PC forwarding
			IE.cont_reg = 1;
			IE.alu_ans = IE.PC + 4;
			PC = (reg_rs1 + _IMM_EXT(imm, 12)) & (~0x1);
			// bubble ID & IF
			ID.init();
			IF.init();
			total_instrs -= 2;
		}
		else
			err_instr(instr, "I");
	}
	else if (opcode == 0x73) {
		if (func3 == 0x0) { // ecall

		}
		else
			err_instr(instr, "I");
	}
	else
	    	err_instr(instr, "I");
}

void exe_S(uint32_t instr)
{
	uint32_t opcode = IE.opcode;
	uint32_t rs1 = IE.rs1;
	uint32_t rs2 = IE.rs2;
	uint32_t func3 = IE.func3;
	uint64_t imm =  ((instr & 0xFE000000) >> 20) +
			((instr & 0x00000F80) >> 7);
	uint32_t reg_rs1 = reg[rs1];
	uint32_t reg_rs2 = reg[rs2];
	
	// if forwarding
	if (IM.rd == IE.rs1 && IM.cont_reg && IM.rd != 0)
		reg_rs1 = IM.alu_ans;
	if (IM.rd == IE.rs2 && IM.cont_reg && IM.rd != 0)
		reg_rs2 = IM.alu_ans;

	if (func3 == 0x0) { // sb
		IE.cont_writemem = 1;
		IE.mem_width = 8;
		IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		IE.alu_ans = reg_rs2;
	}
	else if (func3 == 0x1) { //sh
		IE.cont_writemem = 1;
		IE.mem_width = 16;
		IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		IE.alu_ans = reg_rs2;
	}
	else if (func3 == 0x2) { //sw
		IE.cont_writemem = 1;
		IE.mem_width = 32;
		IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		IE.alu_ans = reg_rs2;
	}
	else if (func3 == 0x3) { //sd
		IE.cont_writemem = 1;
		IE.mem_width = 64;
		IE.entry = reg_rs1 + _IMM_EXT(imm, 12);
		IE.alu_ans = reg_rs2;
	}
	else
		err_instr(instr, "S");
}

void exe_SB(uint32_t instr)
{
	uint32_t opcode = IE.opcode;
	uint32_t rs1 = IE.rs1;
	uint32_t rs2 = IE.rs2;
	uint32_t func3 = IE.func3;
	uint64_t imm =  ((instr & 0x7E000000) >> 20) +
			((instr & 0x80000000) >> 19) +
			((instr & 0x00000F00) >> 7) +
			((instr & 0x00000080) << 4);
	uint32_t reg_rs1 = reg[rs1];
	uint32_t reg_rs2 = reg[rs2];
	
	// if forwarding
	if (IM.rd == IE.rs1 && IM.cont_reg && IM.rd != 0)
		reg_rs1 = IM.alu_ans;
	if (IM.rd == IE.rs2 && IM.cont_reg && IM.rd != 0)
		reg_rs2 = IM.alu_ans;

	// PC forwarding
	if (func3 == 0x0) { // beq
		if (reg_rs1 == reg_rs2) {
			PC = IE.PC + _IMM_EXT(imm, 13);
			// bubble ID & IF
			ID.init();
			IF.init();
			total_instrs -= 2;
		}
	}
	else if (func3 == 0x1) { // bne
		if (reg_rs1 != reg_rs2) {
			PC = IE.PC + _IMM_EXT(imm, 13);
			// bubble ID & IF
			ID.init();
			IF.init();
			total_instrs -= 2;
		}
	}
	else if (func3 == 0x4) { // blt
		if ((int64_t) reg_rs1 < (int64_t) reg_rs2) {
			PC = IE.PC + _IMM_EXT(imm, 13);
			// bubble ID & IF
			ID.init();
			IF.init();
			total_instrs -= 2;
		}
	}
	else if (func3 == 0x5) { // bge
		if ((int64_t) reg_rs1 >= (int64_t) reg_rs2) {
			PC = IE.PC + _IMM_EXT(imm, 13);
			// bubble ID & IF
			ID.init();
			IF.init();
			total_instrs -= 2;
		}
	}
	else 
		err_instr(instr, "SB");
}

void exe_U(uint32_t instr)
{
	uint32_t opcode = IE.opcode;
	uint32_t rd = IE.rd;
	uint64_t imm = instr & 0xFFFFF000;

	if (opcode == 0x17) { // auipc
		IE.cont_reg = 1;
		IE.alu_ans = IE.PC + _IMM_EXT(imm, 32);
	}
	else if (opcode == 0x37) { // lui
		IE.cont_reg = 1;
		IE.alu_ans = _IMM_EXT(imm, 32);
	}
	else
		err_instr(instr, "U");
}

uint64_t exe_UJ(uint32_t instr)
{
	uint32_t opcode = IE.opcode;
	uint32_t rd = IE.rd;
	uint64_t imm =  ((instr & 0x7FE00000) >> 20) +
			((instr & 0x80000000) >> 11) +
			((instr & 0x00100000) >> 9) + 
			(instr & 0x000FF000);

	if (opcode == 0x6F) { //jal
		// PC forwrding
		IE.cont_reg = 1;
		IE.alu_ans = IE.PC + 4;
		PC = IE.PC + _IMM_EXT(imm, 21);
		// bubble ID & IF
		ID.init();
		IF.init();
		total_instrs -= 2;
	}
	else
		err_instr(instr, "UJ");
}

void iexecute()
{
	uint32_t instr = IE.instr;
	uint32_t opcode = IE.opcode;
	renew_cycles = 1;

	if (opcode == 0x33 || opcode == 0x3B)
		exe_R(instr);
	else if (opcode == 0x23)
		exe_S(instr);
	else if (opcode == 0x63)
		exe_SB(instr);
	else if (opcode == 0x6F)
		exe_UJ(instr);
	else if (opcode == 0x03 || opcode == 0x13 || opcode == 0x1B || opcode == 0x67 || opcode  == 0x73)
		exe_I(instr);
	else if (opcode == 0x17 || opcode == 0x37)
		exe_U(instr);
	else
		err_instr(instr, "execute");
	
	// minus 1 because in FUNC flowing, has been added
	total_cycles = total_cycles + renew_cycles - 1;
}

#endif
