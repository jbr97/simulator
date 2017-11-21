/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t mulhu(uint64_t ra, uint64_t rb)
{
	uint64_t al = (uint32_t) ra;
	uint64_t ah = ra >> 32;
	uint64_t bl = (uint32_t) rb;
	uint64_t bh = rb >> 32;
	
	uint64_t res = ((ah * bl) & 0xFFFFFFFF) +
			((al * bh) & 0xFFFFFFFF) + 
			((al * bl) >> 32);
	res >>= 32;
	res += ah * bh;
	res += ((ah * bl) >> 32) + ((al * bh) >> 32);
	return res;
}

int64_t mulh(int64_t ra, int64_t rb)
{
	bool s_flag = (ra < 0) != (rb < 0);
	uint64_t ura = ra < 0 ? -ra : ra;
	uint64_t urb = rb < 0 ? -rb : rb;
	uint64_t resh = mulhu(ura, urb);
	uint64_t resl = ura * urb;
	return s_flag ? (~resh) + (resl == 0) : resh;
}

uint64_t divu(uint64_t ra, uint64_t rb, bool isrem)
{
	if (rb == 0) return isrem ? ra : UINT64_MAX;
	return isrem ? ra % rb : ra / rb;
}

int64_t div(int64_t ra, int64_t rb, bool isrem)
{
	if (rb == 0) return isrem ? ra : UINT64_MAX;
	if (ra == INT64_MIN && rb == -1)
		return isrem ? 0 : ra;
	return isrem ? ra % rb : ra / rb;
}

uint32_t divuw(uint32_t ra, uint32_t rb, bool isrem)
{
	if (rb == 0) return isrem ? ra : UINT32_MAX;
	return isrem ? ra % rb : ra / rb;
}

int32_t divw(int32_t ra, int32_t rb, bool isrem)
{
	if (rb == 0) return isrem ? ra : UINT32_MAX;
	if (ra == INT32_MIN && rb == -1)
		return isrem ? 0 : ra;
	return isrem ? ra % rb : ra / rb;
}
