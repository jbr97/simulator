/***********
Author: joishbader
Date:   2017/11/21
Title:  COD_lab2.2
***********/

#include"Read_Elf.h"

static FILE *elf;
static FILE *file;

// Program headers
static unsigned int padr;
static unsigned int psize;
static unsigned int pnum;

// Section Headers
static unsigned int sadr;
static unsigned int ssize;
static unsigned int snum;

// Symbol table
static unsigned int symnum;
static unsigned int symadr;
static unsigned int symsize;

// Section header string table index 
static unsigned int sindex;

// Symbol string table
static unsigned int stradr;
static unsigned int strsize;

// import sections
void *sec_text;
Elf64_Shdr shdr_text;
void *sec_bss;
Elf64_Shdr shdr_bss;
void *sec_data;
Elf64_Shdr shdr_data;
void *sec_sdata;
Elf64_Shdr shdr_sdata;

// important symbols
uint64_t sym_PC;
uint64_t sym_GP;

void *read_elf(const char *file_name)
{
	// open elf_info file
	elf = fopen("elf_log.txt", "w");

	// try to open elf file
	file = fopen(file_name, "rb");
	if (file == NULL) {
		fprintf(elf, "Can not open ELF file %s.\n", file_name);
		fclose(elf);
		exit(1);
	}

	// read elf file header
	fprintf(elf,"ELF Header:\n");
	read_elf_header();

	// read elf file sections
	fprintf(elf,"\n\nSection Headers:\n");
	read_elf_sections();

	// read elf file program hdr
	fprintf(elf,"\n\nProgram Headers:\n");
	read_phdr();

	// read elf file symbol table
	fprintf(elf,"\n\nSymbol table:\n");
	read_symtable();

	// load file into memory
	fseek(file, 0, SEEK_END);
	uint64_t file_size = (uint64_t) ftell(file);
	fseek(file, 0, 0);
	void *fp = malloc(file_size);
	fread(fp, 1, file_size, file);

	// close elf file
	fclose(elf);
	
	return fp;
}

void read_elf_header()
{
	Elf64_Ehdr elf64_hdr;
	// file should be relocated
	fread(&elf64_hdr, 1, sizeof(elf64_hdr), file);
	
	/* assign global elf variables */
	// program headers
	padr = elf64_hdr.e_phoff;
	psize = elf64_hdr.e_phentsize;
	pnum = elf64_hdr.e_phnum;
	// section headers
	sadr = elf64_hdr.e_shoff;
	ssize = elf64_hdr.e_shentsize;
	snum = elf64_hdr.e_shnum;
	sindex = elf64_hdr.e_shstrndx;

	// print elf_header_info
	fprintf(elf, " magic number:  ");
	for (int i = 0; i < 16; ++i)
		fprintf(elf, " %02x", elf64_hdr.e_ident[i]);
	fprintf(elf, "\n");
	fprintf(elf, " Class:\t\t\t\t\tELF64\n");	
	fprintf(elf, " Data:\t\t\t\t\tlittle-endian\n");		
	fprintf(elf, " Version:\t\t\t\t%d (current)\n", 1);
	fprintf(elf, " OS/ABI:\t\t\t\tSystem V ABI\n");
	fprintf(elf, " ABI Version:\t\t\t\t%d\n", 0);
	fprintf(elf, " Type:\t\t\t\t\t0x%x\n", (unsigned int) elf64_hdr.e_type);
	fprintf(elf, " Machine:\t\t\t\t0x%x\n", (unsigned int) elf64_hdr.e_machine);
	fprintf(elf, " Version:\t\t\t\t0x%x\n", (unsigned int) elf64_hdr.e_version);
	fprintf(elf, " Entry point address:\t\t\t0x%lx\n", (unsigned long) elf64_hdr.e_entry);
	fprintf(elf, " Start of program headers:\t\t%lu (bytes into  file)\n", (unsigned long) elf64_hdr.e_phoff);
	fprintf(elf, " Start of section headers:\t\t%lu (bytes into  file)\n", (unsigned long) elf64_hdr.e_shoff);
	fprintf(elf, " Flags:\t\t\t\t\t0x%x\n", (unsigned int) elf64_hdr.e_flags);
	fprintf(elf, " Size of this header:\t\t\t%u (Bytes)\n", (unsigned int) elf64_hdr.e_ehsize);
	fprintf(elf, " Size of program headers:\t\t%u (Bytes)\n", (unsigned int) elf64_hdr.e_phentsize);
	fprintf(elf, " Number of program headers:\t\t%u\n", (unsigned int) elf64_hdr.e_phnum);
	fprintf(elf, " Size of section headers:\t\t%u (Bytes)\n", (unsigned int) elf64_hdr.e_shentsize);
	fprintf(elf, " Number of section headers:\t\t%u\n", (unsigned int) elf64_hdr.e_shnum);
	fprintf(elf, " Section header string table index:\t%u\n", (unsigned int) elf64_hdr.e_shstrndx);
}

void read_elf_sections()
{
	Elf64_Shdr elf64_shdr, strhdr;
	unsigned char *strtable;

	// find section header string table
	fseek(file, sadr + sindex * ssize, 0);
	fread(&strhdr, 1, sizeof(strhdr), file);

	fseek(file, strhdr.sh_offset, 0);
	strtable = (unsigned char *) malloc(sizeof(unsigned char) * strhdr.sh_size);
	fread(strtable, 1, strhdr.sh_size, file);

	for(int c = 0; c < snum; c++) {
		fprintf(elf, " [%3d]\n", c);
		
		// file should be relocated
		fseek(file, sadr + c * ssize, 0);
		fread(&elf64_shdr, 1, sizeof(elf64_shdr), file);
		
		unsigned char *str_sh_name = strtable + (unsigned int) elf64_shdr.sh_name;

		if (strcmp((const char *) str_sh_name, ".symtab") == 0) {
			symadr = elf64_shdr.sh_offset;
			symsize = elf64_shdr.sh_size;
			symnum = symsize / elf64_shdr.sh_entsize;
		}
		
		if (strcmp((const char *) str_sh_name, ".strtab") == 0)  {
			stradr = elf64_shdr.sh_offset;
			strsize = elf64_shdr.sh_size;
		}

		if (strcmp((const char *) str_sh_name, ".text") == 0)  {
			shdr_text = elf64_shdr;
			sec_text = malloc(elf64_shdr.sh_size);
			fseek(file, shdr_text.sh_offset, 0);
			fread(sec_text, 1, shdr_text.sh_size, file);
		}
		if (strcmp((const char *) str_sh_name, ".bss") == 0)  {
			shdr_bss = elf64_shdr;
			sec_bss = malloc(elf64_shdr.sh_size);
			fseek(file, shdr_bss.sh_offset, 0);
			fread(sec_bss, 1, shdr_bss.sh_size, file);
		}
		if (strcmp((const char *) str_sh_name, ".data") == 0)  {
			shdr_data = elf64_shdr;
			sec_data = malloc(shdr_data.sh_size);
			fseek(file, shdr_data.sh_offset, 0);
			fread(sec_data, 1, shdr_data.sh_size, file);
		}
		if (strcmp((const char *) str_sh_name, ".sdata") == 0)  {
			shdr_sdata = elf64_shdr;
			sec_sdata = malloc(shdr_sdata.sh_size);
			fseek(file, shdr_sdata.sh_offset, 0);
			fread(sec_sdata, 1, shdr_sdata.sh_size, file);
		}

		fprintf(elf, " Name: %20s  ", str_sh_name);
		fprintf(elf, " Type: %x", (unsigned int) elf64_shdr.sh_type);
		fprintf(elf, " Address: %x", (unsigned int) elf64_shdr.sh_addr);
		fprintf(elf, " Offest:  %lx\n", (unsigned long) elf64_shdr.sh_offset);
		fprintf(elf, " Size:  %lx", (unsigned long) elf64_shdr.sh_size);
		fprintf(elf, " Entsize:  %lx", (unsigned long) elf64_shdr.sh_entsize);
		fprintf(elf, " Flags:   %lx", (unsigned long) elf64_shdr.sh_flags);
		fprintf(elf, " Link:  %x", (unsigned int) elf64_shdr.sh_link);
		fprintf(elf, " Info:  %x", (unsigned int) elf64_shdr.sh_info);
		fprintf(elf, " Align: %lu\n", (unsigned long) elf64_shdr.sh_addralign);
 	}

	free(strtable);
}

void read_phdr()
{
	Elf64_Phdr elf64_phdr;

	fseek(file, padr, 0);
	for(int c = 0; c < pnum; c++) {
		fprintf(elf," [%3d]\n",c);
			
		//file should be relocated
		fread(&elf64_phdr, 1, sizeof(elf64_phdr), file);

		fprintf(elf, " Type:   0x%x\n", (unsigned int) elf64_phdr.p_type);
		fprintf(elf, " Flags:  0x%x\n", (unsigned int) elf64_phdr.p_flags);
		fprintf(elf, "Offset:\t\t0x%016lx\n", (unsigned long) elf64_phdr.p_offset);
		fprintf(elf, "VirtAddr:\t0x%016lx\n", (unsigned long) elf64_phdr.p_vaddr);
		fprintf(elf, "PhysAddr:\t0x%016lx\n", (unsigned long) elf64_phdr.p_paddr);
		fprintf(elf, "FileSiz:\t0x%016lx\n", (unsigned long) elf64_phdr.p_filesz);
		fprintf(elf, "MemSiz:\t\t0x%016lx\n", (unsigned long) elf64_phdr.p_memsz);
		fprintf(elf, "Align:\t\t%lx\n", (unsigned long) elf64_phdr.p_align);
	}
}


void read_symtable()
{
	Elf64_Sym elf64_sym;
	unsigned char *strtable;
	
	fseek(file, stradr, 0);
	strtable = (unsigned char *) malloc(sizeof(unsigned char) * strsize);
	fread(strtable, 1, strsize, file);

	fseek(file, symadr, 0);
	for(int c = 0; c < symnum; c++) {
		fprintf(elf, " [%3d]   ", c);
		
		//file should be relocated
		fread(&elf64_sym, 1, sizeof(elf64_sym), file);
		
		unsigned char *str_st_name = strtable + (unsigned int) elf64_sym.st_name;

		if (strcmp((const char *) str_st_name, "main") == 0) 
			sym_PC = elf64_sym.st_value;

		if (strcmp((const char *) str_st_name, "__global_pointer$") == 0) 
			sym_GP = elf64_sym.st_value;
		if (strcmp((const char *) str_st_name, "_gp") == 0) 
			sym_GP = elf64_sym.st_value;

		fprintf(elf, " Name: %40s   ", str_st_name);
		fprintf(elf, " Bind: %x", (unsigned int) elf64_sym.st_info >> 4);
		fprintf(elf, " Type: %x", (unsigned int) elf64_sym.st_info & 0xf);
		fprintf(elf, " NDX:  %u", (unsigned int) elf64_sym.st_shndx);
		fprintf(elf, " Size: %lu", (unsigned long) elf64_sym.st_size);
		fprintf(elf, " Value: %016lx\n", (unsigned long) elf64_sym.st_value);
	}

	free(strtable);
}

