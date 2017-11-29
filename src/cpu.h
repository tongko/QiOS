#ifndef __CPU_H_
#define __CPU_H_

/*
	CR0 — Contains system control flags that control operating mode and states of the processor. 
	When loading a control register, reserved bits should always be set to the values previously read.
*/
enum cr0 {
	PE = 0x00000001,  //	(00) Protection Enable
	MP = 0x00000002,  //	(01) Monitor Coprocessor
	EM = 0x00000004,  //	(02) Emulation
	TS = 0x00000008,  //	(03) Task Switch
	ET = 0x00000010,  //	(04) Extension Type
	NE = 0x00000020,  //	(05) Numeric Error
	WP = 0x00010000,  //	(16) Write Protect
	AM = 0x00040000,  //	(18) Alignment Mask
	NW = 0x20000000,  //	(29) Not Write-through
	CD = 0x40000000,  //	(30) Cache Disable
	PG = 0x80000000   //	(31) Paging
};

enum cr3 {
	PWT = 0x00000008,  //	(03) Page-level Write-Through
	PCD = 0x00000010   //	(04) Page-level Cache Disable
};

enum cr4 {
	VME = 0x00000001,
	PVI = 0x00000002,
	TSD = 0x00000004,
	DE = 0x00000008,
	PSE = 0x00000010,
	PAE = 0x00000020,
	MCE = 0x00000040,
	PGE = 0x00000080,
	PCE = 0x00000100,
	OSFXSR = 0X00000200,
	OSXMMEXCPT = 0x00000400,
	UMIP = 0x00000800,
	VMXE = 0x00002000,
	SMXE = 0x00004000,
	FSGSBASE = 0x00010000,
	PCIDE = 0x00020000,
	OSXSAVE = 0x00040000,
	SMEP = 0x00100000,
	SMAP = 0x00200000,
	PKE = 0x00400000
};

#endif  //	__CPU_H