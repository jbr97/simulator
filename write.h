/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#ifndef __WRITE_H__
#define __WRITE_H__

#include "global_variables.h"

void iwrite()
{
	if (IW.cont_reg) 
		reg[IW.rd] = IW.alu_ans;
	
	if (IW.cont_PC)
		PC = IW.new_PC;
}

#endif
