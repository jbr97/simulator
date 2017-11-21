RISCV simulator (with pipeline)
===
Author: Jiang Borui
Date: 2017/11/21

How to compiler and run
===

```
$ cd /DIR/TO/THE/SIMULATOR/
$ make
$ ./simulator /DIR/TO/THE/ELFFILE
```

Then you can see `>> ` on the screen.
1. R mode: type `r` to run until the program terminates.
	`>> r`
2. N mode: type `n (NUM)` to run by $NUM step. The default $NUM is 1.
	`>> n`
	`>> n 20`
3. M mode: type `m ADDR:(NUM)` to view the memory. The default $NUM is 4.
	`>> m 0x1897`
	`>> m 0x1897:10`
4. D mode: type `d` to view the registers including PC and instruction-codes.
	`>> d`
5. Q mode: type `q` to exit.
	`>> q`

File composition
===
* Read_Elf.h
	* Read_Elf.h
* Read_Elf.cpp
	* Read_Elf.cpp
* global_variables.h
	* define global variables and data structures
* fetch.h
	* define the fetch logic
* decode.h
	* define the decoder logic
* execute.h
	* define the register-reading, executing and forwarding logic
* memory.h
	* define the memory read and write logic
* write.h
	* define the write back logic
* simulator.h
	* functions and dependence which simulator.cpp needs
* simulator.cpp
	* main simulator file
