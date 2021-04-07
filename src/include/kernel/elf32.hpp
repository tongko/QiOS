#pragma once

#include <kernel/elfcommon.hpp>
#include <kernel/types.hpp>

namespace qkrnl {

/*
 * ELF definitions common to all 32-bit architectures.
 */

typedef uint32_t Elf32Addr;
typedef uint16_t Elf32Half;
typedef uint32_t Elf32Off;
typedef int32_t	 Elf32SWord;
typedef uint32_t Elf32Word;
typedef uint64_t Elf32LWord;

typedef Elf32Word Elf32Hashelt;

/* Non-standard class-dependent datatype used for abstraction. */
typedef Elf32Word  Elf32Size;
typedef Elf32SWord Elf32SSize;

/*
 * ELF header.
 */

typedef struct {
	byte_t	  e_ident[EI_NIDENT]; /* File identification. */
	Elf32Half e_type;			  /* File type. */
	Elf32Half e_machine;		  /* Machine architecture. */
	Elf32Word e_version;		  /* ELF format version. */
	Elf32Addr e_entry;			  /* Entry point. */
	Elf32Off  e_phoff;			  /* Program header file offset. */
	Elf32Off  e_shoff;			  /* Section header file offset. */
	Elf32Word e_flags;			  /* Architecture-specific flags. */
	Elf32Half e_ehsize;			  /* Size of ELF header in bytes. */
	Elf32Half e_phentsize;		  /* Size of program header entry. */
	Elf32Half e_phnum;			  /* Number of program header entries. */
	Elf32Half e_shentsize;		  /* Size of section header entry. */
	Elf32Half e_shnum;			  /* Number of section header entries. */
	Elf32Half e_shstrndx;		  /* Section name strings section. */
} Elf32Ehdr;

/*
 * Section header.
 */

typedef struct {
	Elf32Word sh_name;		/* Section name (index into the
					   section header string table). */
	Elf32Word sh_type;		/* Section type. */
	Elf32Word sh_flags;		/* Section flags. */
	Elf32Addr sh_addr;		/* Address in memory image. */
	Elf32Off  sh_offset;	/* Offset in file. */
	Elf32Word sh_size;		/* Size in bytes. */
	Elf32Word sh_link;		/* Index of a related section. */
	Elf32Word sh_info;		/* Depends on section type. */
	Elf32Word sh_addralign; /* Alignment in bytes. */
	Elf32Word sh_entsize;	/* Size of each entry in section. */
} Elf32Shdr;

/*
 * Program header.
 */

typedef struct {
	Elf32Word p_type;	/* Entry type. */
	Elf32Off  p_offset; /* File offset of contents. */
	Elf32Addr p_vaddr;	/* Virtual address in memory image. */
	Elf32Addr p_paddr;	/* Physical address (not used). */
	Elf32Word p_filesz; /* Size of contents in file. */
	Elf32Word p_memsz;	/* Size of contents in memory. */
	Elf32Word p_flags;	/* Access permission flags. */
	Elf32Word p_align;	/* Alignment in memory and file. */
} Elf32Phdr;

/*
 * Dynamic structure.  The ".dynamic" section contains an array of them.
 */

typedef struct {
	Elf32SWord d_tag; /* Entry type. */
	union {
		Elf32Word d_val; /* Integer value. */
		Elf32Addr d_ptr; /* Address value. */
	} d_un;
} Elf32Dyn;

/*
 * Relocation entries.
 */

/* Relocations that don't need an addend field. */
typedef struct {
	Elf32Addr r_offset; /* Location to be relocated. */
	Elf32Word r_info;	/* Relocation type and symbol index. */
} Elf32Rel;

/* Relocations that need an addend field. */
typedef struct {
	Elf32Addr  r_offset; /* Location to be relocated. */
	Elf32Word  r_info;	 /* Relocation type and symbol index. */
	Elf32SWord r_addend; /* Addend. */
} Elf32Rela;

/* Macros for accessing the fields of r_info. */
#define ELF32_R_SYM(info)  ((info) >> 8)
#define ELF32_R_TYPE(info) ((unsigned char) (info))

/* Macro for constructing r_info from field values. */
#define ELF32_R_INFO(sym, type) (((sym) << 8) + (unsigned char) (type))

/*
 *	Note entry header
 */
typedef Elf_Note Elf32Nhdr;

/*
 *	Move entry
 */
typedef struct {
	Elf32LWord m_value;	  /* symbol value */
	Elf32Word  m_info;	  /* size + index */
	Elf32Word  m_poffset; /* symbol offset */
	Elf32Half  m_repeat;  /* repeat count */
	Elf32Half  m_stride;  /* stride info */
} Elf32Move;

/*
 *	The macros compose and decompose values for Move.r_info
 *
 *	sym = ELF32_M_SYM(M.m_info)
 *	size = ELF32_M_SIZE(M.m_info)
 *	M.m_info = ELF32_M_INFO(sym, size)
 */
#define ELF32_M_SYM(info)		((info) >> 8)
#define ELF32_M_SIZE(info)		((unsigned char) (info))
#define ELF32_M_INFO(sym, size) (((sym) << 8) + (unsigned char) (size))

/*
 *	Hardware/Software capabilities entry
 */
typedef struct {
	Elf32Word c_tag; /* how to interpret value */
	union {
		Elf32Word c_val;
		Elf32Addr c_ptr;
	} c_un;
} Elf32Cap;

/*
 * Symbol table entries.
 */

typedef struct {
	Elf32Word st_name;	/* String table index of name. */
	Elf32Addr st_value; /* Symbol value. */
	Elf32Word st_size;	/* Size of associated object. */
	byte_t	  st_info;	/* Type and binding information. */
	byte_t	  st_other; /* Reserved (not used). */
	Elf32Half st_shndx; /* Section index of symbol. */
} Elf32Sym;

/* Macros for accessing the fields of st_info. */
#define ELF32_ST_BIND(info) ((info) >> 4)
#define ELF32_ST_TYPE(info) ((info) &0xf)

/* Macro for constructing st_info from field values. */
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type) &0xf))

/* Macro for accessing the fields of st_other. */
#define ELF32_ST_VISIBILITY(oth) ((oth) &0x3)

/* Structures used by Sun & GNU symbol versioning. */
typedef struct {
	Elf32Half vd_version;
	Elf32Half vd_flags;
	Elf32Half vd_ndx;
	Elf32Half vd_cnt;
	Elf32Word vd_hash;
	Elf32Word vd_aux;
	Elf32Word vd_next;
} Elf32Verdef;

typedef struct {
	Elf32Word vda_name;
	Elf32Word vda_next;
} Elf32Verdaux;

typedef struct {
	Elf32Half vn_version;
	Elf32Half vn_cnt;
	Elf32Word vn_file;
	Elf32Word vn_aux;
	Elf32Word vn_next;
} Elf32Verneed;

typedef struct {
	Elf32Word vna_hash;
	Elf32Half vna_flags;
	Elf32Half vna_other;
	Elf32Word vna_name;
	Elf32Word vna_next;
} Elf32Vernaux;

typedef Elf32Half Elf32Versym;

typedef struct {
	Elf32Half si_boundto; /* direct bindings - symbol bound to */
	Elf32Half si_flags;	  /* per symbol flags */
} Elf32Syminfo;

}	 // namespace qkrnl