/*
 * Driver: elfloader.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 28.05.2014
 * Description: 
 * TODO
 */



#ifndef ELFLOADER_H_
#define ELFLOADER_H_

#include <inttypes.h>
#include "../mmu/sc_mmu.h"

typedef uint16_t elf_half_t;
typedef uint32_t elf_addr_t;
typedef uint32_t elf_off_t;
typedef int32_t elf_sword_t;
typedef uint32_t elf_word_t;

#define ELF_MAG0 0x7F
#define ELF_MAG1 0x45
#define ELF_MAG2 0x4C
#define ELF_MAG3 0x46

#define ELF_MAGIC ((ELF_MAG0) | (ELF_MAG1 << 8) | (ELF_MAG2 << 16) | (ELF_MAG3 << 24))

#define ELF_CLASS_NONE 0x00
#define ELF_CLASS_32 0x01	//32bit file
#define ELF_CLASS_64 0x02	//64bit file

#define ELF_DATA_NONE 0x00
#define ELF_DATA_2LSB 0x01
#define ELF_DATA_2MSB 0x02

#define ELF_ET_NONE 0x0000         	// no type
#define ELF_ET_REL 0x0001           // relocatable
#define ELF_ET_EXEC 0x0002          // executeable
#define ELF_ET_DYN 0x0003           // Shared-Object-File
#define ELF_ET_CORE 0x0004          // Corefile
#define ELF_ET_LOPROC 0xFF00        // Processor-specific
#define ELF_ET_HIPROC 0x00FF        // Processor-specific

#define ELF_EM_NONE 0x0000          // no type
#define ELF_EM_M32 0x0001           // AT&T WE 32100
#define ELF_EM_SPARC 0x0002         // SPARC
#define ELF_EM_386 0x0003           // Intel 80386
#define ELF_EM_68K 0x0004           // Motorola 68000
#define ELF_EM_88K 0x0005           // Motorola 88000
#define ELF_EM_860 0x0007           // Intel 80860
#define ELF_EM_MIPS 0x0008          // MIPS RS3000

#define ELF_EV_NONE 0x00           	// invalid version
#define ELF_EV_CURRENT 0x01        	// current version



typedef struct
{
    uint32_t magic;			// ELF-Magic Number
    uint8_t class;			// 32 or 64 bit?
    uint8_t data;			// Little or Big Endian?
    uint8_t version;		// same as ELF_HEADER.e_version
    uint32_t pad;			// reserved (zero)
    uint32_t pad2;			// reserved (zero)
    uint8_t nident;
}  elf_ident_header_t;

typedef struct
{
    elf_ident_header_t ident;	// IDENT-HEADER (see above)
    elf_half_t type;			// type of the ELF-file (relocatable, executeable, shared-object...)
    elf_half_t machine;			// processor-type
    elf_word_t version;			// ELF-version
    elf_addr_t entry;			// virtual address of the entrypoint
    elf_off_t phoff;			// offset of the program-header. zero if no program-header exists
    elf_off_t shoff;			// offset of the section-header. zero if no section-header exists
    elf_word_t flags;			// processor-specific flags
    elf_half_t ehsize;			// size of the ELF-header
    elf_half_t phentsize;		// size of one program-header entry
    elf_half_t phnum;			// number of entries in the program-header. zero if no program-header exists
    elf_half_t shentsize;		// size of one section-header entry
    elf_half_t shnum;			// number of entries in the section-header. zero if no section-header exists
    elf_half_t shstrndx;		// tells us which entry of the section-header is linked to the String-Table
} elf_header_t;

typedef struct
{
    elf_word_t type;     // type of the segment (see constants above)
    elf_off_t offset;    // offset of the segment (in the file)
    elf_addr_t vaddr;    // virtual address to which we should copy the segment
    elf_addr_t paddr;    // physical address
    elf_word_t filesz;   // size of the segment in the file
    elf_word_t memsz;    // size of the segment in memory
    elf_word_t flags;    // flags (combination of constants above)
    elf_word_t align;    // alignment. if zero or one, then no alignment is needed, otherwise the alignment has to be a power of two
}  elf_program_header_entry_t;

#define ELF_PT_NULL 0x00          	// invalid segment
#define ELF_PT_LOAD 0x01          	// loadable segment
#define ELF_PT_DYNAMIC 0x02       	// dynamic segment
#define ELF_PT_INTERP 0x03        	// position of a zero-terminated string, which tells the interpreter
#define ELF_PT_NOTE 0x04          	// universal segment
#define ELF_PT_SHLIB 0x05         	// shared lib
#define ELF_PT_PHDIR 0x06         	// tells position and size of the program-header
#define ELF_PT_LOPROC 0x70000000  	// reserved
#define ELF_PT_HIPROC 0x7FFFFFFF  	// reserved

#define ELF_PF_X 0x01             	// executeable segment
#define ELF_PF_W 0x02             	// writeable segment
#define ELF_PF_R 0x04             	// readable segment






#endif /* ELFLOADER_H_ */
