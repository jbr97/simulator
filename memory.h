/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "global_variables.h"

#define PC_END 		0x0
#define MAX_STACK	100000
#define MAX_GP		(reg[3] < 100000 ? reg[3] : 100000)

// important sections
extern void *sec_text;
extern Elf64_Shdr shdr_text;
extern void *sec_bss;
extern Elf64_Shdr shdr_bss;
extern void *sec_data;
extern Elf64_Shdr shdr_data;
extern void *sec_sdata;
extern Elf64_Shdr shdr_sdata;

// stack 
void *_stack;
void *__global_pointer;

static bool addr_internal(uint64_t x, uint64_t s, uint64_t l)
{
	return x >= s && x < s + l;
}

void *mem_search(uint64_t addr)
{
    if (addr_internal(addr, shdr_bss.sh_addr, shdr_bss.sh_size))
        return (void *) ((unsigned long) sec_bss + addr - (unsigned long) shdr_bss.sh_addr);
    if (addr_internal(addr, shdr_text.sh_addr, shdr_text.sh_size))
        return (void *) ((unsigned long) sec_text + addr - (unsigned long) shdr_text.sh_addr);
    if (addr_internal(addr, shdr_data.sh_addr, shdr_data.sh_size))
        return (void *) ((unsigned long) sec_data + addr - (unsigned long) shdr_data.sh_addr);
    if (addr_internal(addr, shdr_sdata.sh_addr, shdr_sdata.sh_size))
        return (void *) ((unsigned long) sec_sdata + addr - (unsigned long) shdr_sdata.sh_addr);
    if (addr_internal(addr, (unsigned long) _stack, MAX_STACK))
        return (void *) (addr);

    if (addr_internal(addr, reg[3] - MAX_GP, reg[3] + MAX_GP))
	return (void *) ((unsigned long) __global_pointer + (unsigned long) (addr - reg[3]));
    return 0;
}

void imemory()
{
	// invalid read & write at the same time
	if (IM.cont_readmem && IM.cont_writemem) {
		puts("FUNC (imemory): Invalid memory read&write controller!");
		exit(1);
	}
	
	// control: read_memory
	if (IM.cont_readmem) {
		uint64_t temp = 0;
		if (IM.mem_width == 8)
			temp = *((int8_t *) mem_search(IM.entry));
		else if (IM.mem_width == 16)
			temp = *((int16_t *) mem_search(IM.entry));
		else if (IM.mem_width == 32)
			temp = *((int32_t *) mem_search(IM.entry));
		else if (IM.mem_width == 64)
			temp = *((int64_t *) mem_search(IM.entry));
		else {
			puts("FUNC (imemory): Invalid memory width!");
			exit(1);
		}
		IM.alu_ans = temp;
	}
	
	// control: write_memory
	if (IM.cont_writemem) {
		if (IM.mem_width == 8) {
			uint8_t *mp = (uint8_t *) mem_search(IM.entry);
			*mp = (uint8_t) (IM.alu_ans & 0xFF);
		}
		else if (IM.mem_width == 16) {
			uint16_t *mp = (uint16_t *) mem_search(IM.entry);
			*mp = (uint16_t) (IM.alu_ans & 0xFFFF);
		}
		else if (IM.mem_width == 32) {
			uint32_t *mp = (uint32_t *) mem_search(IM.entry);
			*mp = (uint32_t) (IM.alu_ans & 0xFFFFFFFF);
		}
		else if (IM.mem_width == 64) {
			uint64_t *mp = (uint64_t *) mem_search(IM.entry);
			*mp = (uint64_t) IM.alu_ans;
		}
		else {
			puts("FUNC (imemory): Invalid memory width!");
			exit(1);
		}	
	}
}

#endif
