/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#ifndef __DECODE_H__
#define __DECODE_H__

#include "global_variables.h"

#define OPCODE_MASK 	0x7F
#define FUNC3_MASK	0x00007000
#define FUNC7_MASK	0xFE000000
#define RD_MASK		0x00000F80
#define RS1_MASK	0x000F8000
#define RS2_MASK	0x01F00000
#define JALR_MASK	(~(0x1))

#define FUNC3_OFF	12
#define FUNC7_OFF	25
#define RD_OFF		7
#define RS1_OFF		15
#define RS2_OFF		20

#define DEINSTR(x,m,o)	(((x)&(m))>>(o))

void idecode()
{
	uint32_t instr = ID.instr;

	ID.opcode = instr & OPCODE_MASK;
	ID.rd = DEINSTR(instr, RD_MASK, RD_OFF);
	ID.rs1 = DEINSTR(instr, RS1_MASK, RS1_OFF);
	ID.rs2 = DEINSTR(instr, RS2_MASK, RS2_OFF);
	ID.func3 = DEINSTR(instr, FUNC3_MASK, FUNC3_OFF);
	ID.func7 = DEINSTR(instr, FUNC7_MASK, FUNC7_OFF);
}

#endif
