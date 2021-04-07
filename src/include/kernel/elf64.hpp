#pragma once

#include <kernel/elfcommon.hpp>
#include <kernel/types.hpp>

namespace qkrnl {

//	Ident header

#define EI_MAG0		  0		// File identification
#define EI_MAG1		  1		// File identification
#define EI_MAG2		  2		// File identification
#define EI_MAG3		  3		// File identification
#define EI_CLASS	  4		// File class
#define EI_DATA		  5		// Data encoding
#define EI_VERSION	  6		// File version
#define EI_OSABI	  7		// Operating system/ABI identification
#define EI_ABIVERSION 8		// ABI version
#define EI_PAD		  9		// Start of padding bytes
#define EI_NIDENT	  16	// Size of e_ident[]

#define ELFMAG0 0x7F	// e_ident[EI_MAG0]
#define ELFMAG1 'E'		// e_ident[EI_MAG1]
#define ELFMAG2 'L'		// e_ident[EI_MAG2]
#define ELFMAG3 'F'		// e_ident[EI_MAG3]

#define ELFCLASSNONE 0	  // Invalid class
#define ELFCLASS32	 1	  // 32-bit objects
#define ELFCLASS64	 2	  // 64-bit objects

#define ELFDATANONE 0	 // Invalid data encoding
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

typedef uint64_t Elf64Addr;
typedef uint16_t Elf64Half;
typedef uint64_t Elf64Off;
typedef int32_t	 Elf64SWord;
typedef int64_t	 Elf64SXWord;
typedef uint32_t Elf64Word;
typedef uint64_t Elf64LWord;
typedef uint64_t Elf64XWord;

//	Inconsistent among 64 bit architectures, so a machine dependent typedef is
// required.
typedef Elf64Word Elf64Hashelt;

//	Data type used for abstraction
typedef Elf64XWord	Elf64Size;
typedef Elf64SXWord Elf64SSize;

//	ELF header.
typedef struct {
	byte_t	  e_ident[EI_NIDENT];	 //	File identification.
	Elf64Half e_type;				 //	File type.
	Elf64Half e_machine;			 //	Machine architecture.
	Elf64Word e_version;			 //	ELF format version.
	Elf64Addr e_entry;				 //	Entry point.
	Elf64Off  e_phoff;				 //	Program header file offset.
	Elf64Off  e_shoff;				 //	Section header file offset.
	Elf64Word e_flags;				 //	Architecture-specific flags.
	Elf64Half e_ehsize;				 //	Size of ELF header in bytes.
	Elf64Half e_phentsize;			 //	Size of program header entry.
	Elf64Half e_phnum;				 //	Number of program header entries.
	Elf64Half e_shentsize;			 //	Size of section header entry.
	Elf64Half e_shnum;				 //	Number of section header entries.
	Elf64Half e_shstrndx;			 //	Section name strings section.
} Elf64Ehdr;

//	Section header.
typedef struct {
	Elf64Word sh_name;			/* Section name (index into the
						   section header string table). */
	Elf64Word  sh_type;			//	Section type.
	Elf64XWord sh_flags;		//	Section flags.
	Elf64Addr  sh_addr;			//	Address in memory image.
	Elf64Off   sh_offset;		//	Offset in file.
	Elf64XWord sh_size;			//	Size in bytes.
	Elf64Word  sh_link;			//	Index of a related section.
	Elf64Word  sh_info;			//	Depends on section type.
	Elf64XWord sh_addralign;	//	Alignment in bytes.
	Elf64XWord sh_entsize;		//	Size of each entry in section.
} Elf64Shdr;

//	Program header.
typedef struct {
	Elf64Word  p_type;		//	Entry type.
	Elf64Word  p_flags;		//	Access permission flags.
	Elf64Off   p_offset;	//	File offset of contents.
	Elf64Addr  p_vaddr;		//	Virtual address in memory image.
	Elf64Addr  p_paddr;		//	Physical address (not used).
	Elf64XWord p_filesz;	//	Size of contents in file.
	Elf64XWord p_memsz;		//	Size of contents in memory.
	Elf64XWord p_align;		//	Alignment in memory and file.
} Elf64Phdr;

//	Dynamic structure.  The ".dynamic" section contains an array of them.
typedef struct {
	Elf64SXWord d_tag;	  //	Entry type.
	union {
		Elf64XWord d_val;	 //	Integer value.
		Elf64Addr  d_ptr;	 //	Address value.
	} d_un;
} Elf64Dyn;

//	Relocation entries.
//	Relocations that don't need an addend field.
typedef struct {
	Elf64Addr  r_offset;	//	Location to be relocated.
	Elf64XWord r_info;		//	Relocation type and symbol index.
} Elf64Rel;

//	Relocations that need an addend field.
typedef struct {
	Elf64Addr	r_offset;	 //	Location to be relocated.
	Elf64XWord	r_info;		 //	Relocation type and symbol index.
	Elf64SXWord r_addend;	 //	Addend.
} Elf64Rela;

//	Macros for accessing the fields of r_info.
#define ELF64_R_SYM(info)  ((info) >> 32)
#define ELF64_R_TYPE(info) ((info) &0xffffffffL)

//	Macro for constructing r_info from field values.
#define ELF64_R_INFO(sym, type) (((sym) << 32) + ((type) &0xffffffffL))

#define ELF64_R_TYPE_DATA(info) (((Elf64XWord)(info) << 32) >> 40)
#define ELF64_R_TYPE_ID(info)	(((Elf64XWord)(info) << 56) >> 56)
#define ELF64_R_TYPE_INFO(data, type) \
	(((Elf64XWord)(data) << 8) + (Elf64XWord)(type))

//	Move entry
typedef struct {
	Elf64LWord m_value;		 //	symbol value
	Elf64XWord m_info;		 //	size + index
	Elf64XWord m_poffset;	 //	symbol offset
	Elf64Half  m_repeat;	 //	repeat count
	Elf64Half  m_stride;	 //	stride info
} Elf64Move;

#define ELF64_M_SYM(info)		((info) >> 8)
#define ELF64_M_SIZE(info)		((unsigned char) (info))
#define ELF64_M_INFO(sym, size) (((sym) << 8) + (unsigned char) (size))

//	Hardware/Software capabilities entry
typedef struct {
	Elf64XWord c_tag;	 //	how to interpret value
	union {
		Elf64XWord c_val;
		Elf64Addr  c_ptr;
	} c_un;
} Elf64Cap;

//	Symbol table entries.
typedef struct {
	Elf64Word  st_name;		//	String table index of name.
	byte_t	   st_info;		//	Type and binding information.
	byte_t	   st_other;	//	Reserved (not used).
	Elf64Half  st_shndx;	//	Section index of symbol.
	Elf64Addr  st_value;	//	Symbol value.
	Elf64XWord st_size;		//	Size of associated object.
} Elf64Sym;

//	Macros for accessing the fields of st_info.
#define ELF64_ST_BIND(info) ((info) >> 4)
#define ELF64_ST_TYPE(info) ((info) &0xf)

//	Macro for constructing st_info from field values.
#define ELF64_ST_INFO(bind, type) (((bind) << 4) + ((type) &0xf))

//	Macro for accessing the fields of st_other.
#define ELF64_ST_VISIBILITY(oth) ((oth) &0x3)

//	Structures used by Sun & GNU-style symbol versioning.
typedef struct {
	Elf64Half vd_version;
	Elf64Half vd_flags;
	Elf64Half vd_ndx;
	Elf64Half vd_cnt;
	Elf64Word vd_hash;
	Elf64Word vd_aux;
	Elf64Word vd_next;
} Elf64Verdef;

typedef struct {
	Elf64Word vda_name;
	Elf64Word vda_next;
} Elf64Verdaux;

typedef struct {
	Elf64Half vn_version;
	Elf64Half vn_cnt;
	Elf64Word vn_file;
	Elf64Word vn_aux;
	Elf64Word vn_next;
} Elf64Verneed;

typedef struct {
	Elf64Word vna_hash;
	Elf64Half vna_flags;
	Elf64Half vna_other;
	Elf64Word vna_name;
	Elf64Word vna_next;
} Elf64Vernaux;

typedef Elf64Half Elf64Versym;

typedef struct {
	Elf64Half si_boundto;	 //	direct bindings - symbol bound to
	Elf64Half si_flags;		 //	per symbol flags
} Elf64Syminfo;

}	 // namespace qkrnl