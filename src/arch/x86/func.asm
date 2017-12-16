;-- QiOS------------------------------------------------------------------------
;
;    Copyright (C) 2017, 2018 Liew Tze Wei
;
; 	This file is part of the QiOS kernel, and is made available under the      
;   terms of The Unlicense (That means just do whatever you want with the code  
;   base).                                                                      
;-------------------------------------------------------------------------------
;
;   load.asm
;	Entry point of QiOS kernel. GRUB will load this to designated location
;   which is 0x00100000 (1 MiB) base on the linker script link.ld.
;	Loading kernel to 1 MiB is considered safe since EBDA will never exceed this
;	barrier
;
;-------------------------------------------------------------------------------
[bits 32]

SECTION .text

GLOBAL _get_cpu_string
		align	8
_get_cpu_string:
		enter	0, 0
		push	dword [ebp+8]			; store address of buffer

		mov		eax, 0
		cpuid
		pop		eax
		mov		dword [eax], ebx
		mov		dword [eax+4], edx
		mov		dword [eax+8], ecx

		leave
		ret
.end:

GLOBAL _gdt_flush
		align	8
_gdt_flush:
		enter	0, 0

		mov		ecx, [ebp+8]
		lgdt	[ecx]
		mov		ecx, [ebp+12]    ; 0x10 is the offset in the GDT to our data segment, 3rd entry (0x00, 0x08, 0x10, 0x18, ... + 8 byte) 
		mov		ds, cx
		mov		es, cx
		mov		fs, cx
		mov		gs, cx
		mov		ss, cx
		jmp		0x08:.flush2    ; 0x08 is the offset to our code segment: Far jump! 

		align	8
.flush2: 
	    leave 
    	ret            ; Function end
.end:

GLOBAL _tss_flush
		align	8
_tss_flush:
		mov		ax, 0x2B		; Load the index of our TSS structure - The index is
								; 0x28, as it is the 5th selector and each is 8 bytes
								; long, but we set the bottom two bits (making 0x2B)
								; so that it has an RPL of 3, not zero.
		ltr		ax				; Load 0x2B into the task state register.
		ret
.end:	; _tss_flush End of function

GLOBAL _probe_cpuid
		align	8
_probe_cpuid:
		enter	0, 0
		push	ebx
		push	ecx
		push	edx

		; probing CPUID support
		pushfd                               ;Save EFLAGS
		pushfd                               ;Store EFLAGS again for invert test
		xor		dword [esp],0x00200000       ;Invert the ID bit in stored EFLAGS
		popfd                                ;Load stored EFLAGS (with ID bit inverted)
		pushfd                               ;Store EFLAGS again (ID bit may or may not be inverted)
		pop		ecx                          ;eax = modified EFLAGS (ID bit may or may not be inverted)
		xor		ecx,[esp]                    ;eax = whichever bits were changed
		popfd                                ;Restore original EFLAGS
		and		ecx,0x00200000               ;ecx = zero if ID bit can't be changed, else non-zero

		pop		edx
		pop		ecx
		pop		ebx
		leave
		ret
.end:

;*****************************************************************
;	Interrupt Handler
;*****************************************************************

global _isr_0;  0: Divide By Zero Exception
_isr_0:
		cli
		push	dword 0
		push	dword 0
		jmp		fault_common_stub

global _isr_1;  1: Debug Exception
_isr_1:
		cli
		push	dword 0
		push	dword 1
		jmp		fault_common_stub

global _isr_2;  2: Non Maskable Interrupt Exception
_isr_2:
		cli
		push	dword 0
		push	dword 2
		jmp		fault_common_stub

global _isr_3;  3: Int 3 Exception
_isr_3:
		cli
		push	dword 0
		push	dword 3
		jmp		fault_common_stub

global _isr_4;  4: INTO Exception
_isr_4:
		cli
		push	dword 0
		push	dword 4
		jmp		fault_common_stub

global _isr_5;  5: Out of Bounds Exception
_isr_5:
		cli
		push	dword 0
		push	dword 5
		jmp		fault_common_stub

global _isr_6;  6: Invalid Opcode Exception
_isr_6:
		cli
		push	dword 0
		push	dword 6
		jmp		fault_common_stub

global _isr_7;  7: Coprocessor Not Available Exception
_isr_7:
		cli
		push	dword 0
		push	dword 7
		jmp		fault_common_stub

global _isr_8; Double Fault Exception (With Error Code!)
_isr_8:
		cli
		push	dword 8				; Note that we DON'T push a value on the stack in this one!
									; It pushes one already! Use this type of stub for exceptions
									; that pop error codes!
		jmp		fault_common_stub

global _isr_9;  9: Coprocessor Segment Overrun Exception
_isr_9:
		cli
		push	dword 0
		push	dword 9
		jmp		fault_common_stub

global _isr_10; 10: Bad TSS Exception (With Error Code!)
_isr_10:
		cli
		push	dword 10
		jmp		fault_common_stub

global _isr_11; 11: Segment Not Present Exception (With Error Code!)
_isr_11:
		cli
		push	dword 11
		jmp		fault_common_stub

global _isr_12; 12: Stack Fault Exception (With Error Code!)
_isr_12:
		cli
		push	dword 12
		jmp		fault_common_stub

global _isr_13; 13: General Protection Fault Exception (With Error Code!)
_isr_13:
		cli
		push	dword 13
		jmp		fault_common_stub

global _isr_14; 14: Page Fault Exception (With Error Code!)
_isr_14:
		cli
  ; don't push an error code, the cpu will do that
		push	dword 14
		jmp		fault_common_stub

global _isr_15; 15: Reserved Exception
_isr_15:
		cli
		push	dword 0
		push	dword 15
		jmp		fault_common_stub

global _isr_16; 16: Floating Point Exception
_isr_16:
		cli
		push	dword 0
		push	dword 16
		jmp		fault_common_stub

global _isr_17; 17: Alignment Check Exception
_isr_17:
		cli
		push	dword 0
		push	dword 17
		jmp		fault_common_stub

global _isr_18; 18: Machine Check Exception
_isr_18:
		cli
		push	dword 0
		push	dword 18
		jmp		fault_common_stub

; Interrup Service Routine 19-31 are reserved by Intel Processor.
global _isr_19
_isr_19:
		cli
		push	dword 0
		push	dword 19
		jmp		fault_common_stub

global _isr_20
_isr_20:
		cli
		push	dword 0
		push	dword 20
		jmp		fault_common_stub

global _isr_21
_isr_21:
		cli
		push	dword 0
		push	dword 21
		jmp		fault_common_stub

global _isr_22
_isr_22:
		cli
		push	dword 0
		push	dword 22
		jmp		fault_common_stub

global _isr_23
_isr_23:
		cli
		push	dword 0
		push	dword 23
		jmp		fault_common_stub

global _isr_24
_isr_24:
		cli
		push	dword 0
		push	dword 24
		jmp		fault_common_stub

global _isr_25
_isr_25:
		cli
		push	dword 0
		push	dword 25
		jmp		fault_common_stub

global _isr_26
_isr_26:
		cli
		push	dword 0
		push	dword 26
		jmp		fault_common_stub

global _isr_27
_isr_27:
		cli
		push	dword 0
		push	dword 27
		jmp		fault_common_stub

global _isr_28
_isr_28:
		cli
		push	dword 0
		push	dword 28
		jmp		fault_common_stub

global _isr_29
_isr_29:
		cli
		push	dword 0
		push	dword 29
		jmp		fault_common_stub

global _isr_30
_isr_30:
		cli
		push	dword 0
		push	dword 30
		jmp		fault_common_stub

global _isr_31
_isr_31:
		cli
		push	dword 0
		push	dword 31
		jmp		fault_common_stub
; End Intel Processor Reserved.

; Interrupt Service Routines 32-47 are IRQs
global _isr_32 ; IRQ0
_isr_32:
		cli
		push	dword 0
		push	dword 32
		jmp		irq_common_handler

global _isr_33 ; IRQ1
_isr_33:
		cli
		push	dword 0
		push	dword 33
		jmp		irq_common_handler

global _isr_34 ; IRQ2
_isr_34:
		cli
		push	dword 0
		push	dword 34
		jmp		irq_common_handler

global _isr_35 ; IRQ3
_isr_35:
		cli
		push	dword 0
		push	dword 35
		jmp		irq_common_handler

global _isr_36 ; IRQ4
_isr_36:
		cli
		push	dword 0
		push	dword 36
		jmp		irq_common_handler

global _isr_37 ; IRQ5
_isr_37:
		cli
		push	dword 0
		push	dword 37
		jmp		irq_common_handler

global _isr_38 ; IRQ6
_isr_38:
		cli
		push	dword 0
		push	dword 38
		jmp		irq_common_handler

global _isr_39 ; IRQ7
_isr_39:
		cli
		push	dword 0
		push	dword 39
		jmp		irq_common_handler

global _isr_40 ; IRQ8
_isr_40:
		cli
		push	dword 0
		push	dword 40
		jmp		irq_common_handler

global _isr_41 ; IRQ9
_isr_41:
		cli
		push	dword 0
		push	dword 41
		jmp		irq_common_handler

global _isr_42 ; IRQ10
_isr_42:
		cli
		push	dword 0
		push	dword 42
		jmp		irq_common_handler

global _isr_43 ; IRQ11
_isr_43:
		cli
		push	dword 0
		push	dword 43
		jmp		irq_common_handler

global _isr_44 ; IRQ12
_isr_44:
		cli
		push	dword 0
		push	dword 44
		jmp		irq_common_handler

global _isr_45 ; IRQ13
_isr_45:
		cli
		push	dword 0
		push	dword 45
		jmp		irq_common_handler

global _isr_46 ; IRQ14
_isr_46:
		cli
		push	dword 0
		push	dword 46
		jmp		irq_common_handler

global _isr_47 ; IRQ15
_isr_47:
		cli
		push	dword 0
		push	dword 47
		jmp		irq_common_handler

global _isr_48 ; IRQ15
_isr_48:
		cli
		push	dword 0
		push	dword 48
		jmp		irq_common_handler

; The rest are software interrupt.
global _isr_49
_isr_49:
		cli
		push	dword 0
		push	dword 49
		jmp		isr_common_handler

global _isr_50
_isr_50:
		cli
		push	dword 0
		push	dword 50
		jmp		isr_common_handler

global _isr_51
_isr_51:
		cli
		push	dword 0
		push	dword 51
		jmp		isr_common_handler

global _isr_52
_isr_52:
		cli
		push	dword 0
		push	dword 52
		jmp		isr_common_handler

global _isr_53
_isr_53:
		cli
		push	dword 0
		push	dword 53
		jmp		isr_common_handler

global _isr_54
_isr_54:
		cli
		push	dword 0
		push	dword 54
		jmp		isr_common_handler

global _isr_55
_isr_55:
		cli
		push	dword 0
		push	dword 55
		jmp		isr_common_handler

global _isr_56
_isr_56:
		cli
		push	dword 0
		push	dword 56
		jmp		isr_common_handler

global _isr_57
_isr_57:
		cli
		push	dword 0
		push	dword 57
		jmp		isr_common_handler

global _isr_58
_isr_58:
		cli
		push	dword 0
		push	dword 58
		jmp		isr_common_handler

global _isr_59
_isr_59:
		cli
		push	dword 0
		push	dword 59
		jmp		isr_common_handler

global _isr_60
_isr_60:
		cli
		push	dword 0
		push	dword 60
		jmp		isr_common_handler

global _isr_61
_isr_61:
		cli
		push	dword 0
		push	dword 61
		jmp		isr_common_handler

global _isr_62
_isr_62:
		cli
		push	dword 0
		push	dword 62
		jmp		isr_common_handler

global _isr_63
_isr_63:
		cli
		push	dword 0
		push	dword 63
		jmp		isr_common_handler

global _isr_64
_isr_64:
		cli
		push	dword 0
		push	dword 64
		jmp		isr_common_handler

global _isr_65
_isr_65:
		cli
		push	dword 0
		push	dword 65
		jmp		isr_common_handler

global _isr_66
_isr_66:
		cli
		push	dword 0
		push	dword 66
		jmp		isr_common_handler

global _isr_67
_isr_67:
		cli
		push	dword 0
		push	dword 67
		jmp		isr_common_handler

global _isr_68
_isr_68:
		cli
		push	dword 0
		push	dword 68
		jmp		isr_common_handler

global _isr_69
_isr_69:
		cli
		push	dword 0
		push	dword 69
		jmp		isr_common_handler

global _isr_70
_isr_70:
		cli
		push	dword 0
		push	dword 70
		jmp		isr_common_handler

global _isr_71
_isr_71:
		cli
		push	dword 0
		push	dword 71
		jmp		isr_common_handler

global _isr_72
_isr_72:
		cli
		push	dword 0
		push	dword 72
		jmp		isr_common_handler

global _isr_73
_isr_73:
		cli
		push	dword 0
		push	dword 73
		jmp		isr_common_handler

global _isr_74
_isr_74:
		cli
		push	dword 0
		push	dword 74
		jmp		isr_common_handler

global _isr_75
_isr_75:
		cli
		push	dword 0
		push	dword 75
		jmp		isr_common_handler

global _isr_76
_isr_76:
		cli
		push	dword 0
		push	dword 76
		jmp		isr_common_handler

global _isr_77
_isr_77:
		cli
		push	dword 0
		push	dword 77
		jmp		isr_common_handler

global _isr_78
_isr_78:
		cli
		push	dword 0
		push	dword 78
		jmp		isr_common_handler

global _isr_79
_isr_79:
		cli
		push	dword 0
		push	dword 79
		jmp		isr_common_handler

global _isr_80
_isr_80:
		cli
		push	dword 0
		push	dword 80
		jmp		isr_common_handler

global _isr_81
_isr_81:
		cli
		push	dword 0
		push	dword 81
		jmp		isr_common_handler

global _isr_82
_isr_82:
		cli
		push	dword 0
		push	dword 82
		jmp		isr_common_handler

global _isr_83
_isr_83:
		cli
		push	dword 0
		push	dword 83
		jmp		isr_common_handler

global _isr_84
_isr_84:
		cli
		push	dword 0
		push	dword 84
		jmp		isr_common_handler

global _isr_85
_isr_85:
		cli
		push	dword 0
		push	dword 85
		jmp		isr_common_handler

global _isr_86
_isr_86:
		cli
		push	dword 0
		push	dword 86
		jmp		isr_common_handler

global _isr_87
_isr_87:
		cli
		push	dword 0
		push	dword 87
		jmp		isr_common_handler

global _isr_88
_isr_88:
		cli
		push	dword 0
		push	dword 88
		jmp		isr_common_handler

global _isr_89
_isr_89:
		cli
		push	dword 0
		push	dword 89
		jmp		isr_common_handler

global _isr_90
_isr_90:
		cli
		push	dword 0
		push	dword 90
		jmp		isr_common_handler

global _isr_91
_isr_91:
		cli
		push	dword 0
		push	dword 91
		jmp		isr_common_handler

global _isr_92
_isr_92:
		cli
		push	dword 0
		push	dword 92
		jmp		isr_common_handler

global _isr_93
_isr_93:
		cli
		push	dword 0
		push	dword 93
		jmp		isr_common_handler

global _isr_94
_isr_94:
		cli
		push	dword 0
		push	dword 94
		jmp		isr_common_handler

global _isr_95
_isr_95:
		cli
		push	dword 0
		push	dword 95
		jmp		isr_common_handler

global _isr_96
_isr_96:
		cli
		push	dword 0
		push	dword 96
		jmp		isr_common_handler

global _isr_97
_isr_97:
		cli
		push	dword 0
		push	dword 97
		jmp		isr_common_handler

global _isr_98
_isr_98:
		cli
		push	dword 0
		push	dword 98
		jmp		isr_common_handler

global _isr_99
_isr_99:
		cli
		push	dword 0
		push	dword 99
		jmp		isr_common_handler

global _isr_100
_isr_100:
		cli
		push	dword 0
		push	dword 100
		jmp		isr_common_handler

global _isr_101
_isr_101:
		cli
		push	dword 0
		push	dword 101
		jmp		isr_common_handler

global _isr_102
_isr_102:
		cli
		push	dword 0
		push	dword 102
		jmp		isr_common_handler

global _isr_103
_isr_103:
		cli
		push	dword 0
		push	dword 103
		jmp		isr_common_handler

global _isr_104
_isr_104:
		cli
		push	dword 0
		push	dword 104
		jmp		isr_common_handler

global _isr_105
_isr_105:
		cli
		push	dword 0
		push	dword 105
		jmp		isr_common_handler

global _isr_106
_isr_106:
		cli
		push	dword 0
		push	dword 106
		jmp		isr_common_handler

global _isr_107
_isr_107:
		cli
		push	dword 0
		push	dword 107
		jmp		isr_common_handler

global _isr_108
_isr_108:
		cli
		push	dword 0
		push	dword 108
		jmp		isr_common_handler

global _isr_109
_isr_109:
		cli
		push	dword 0
		push	dword 109
		jmp		isr_common_handler

global _isr_110
_isr_110:
		cli
		push	dword 0
		push	dword 110
		jmp		isr_common_handler

global _isr_111
_isr_111:
		cli
		push	dword 0
		push	dword 111
		jmp		isr_common_handler

global _isr_112
_isr_112:
		cli
		push	dword 0
		push	dword 112
		jmp		isr_common_handler

global _isr_113
_isr_113:
		cli
		push	dword 0
		push	dword 113
		jmp		isr_common_handler

global _isr_114
_isr_114:
		cli
		push	dword 0
		push	dword 114
		jmp		isr_common_handler

global _isr_115
_isr_115:
		cli
		push	dword 0
		push	dword 115
		jmp		isr_common_handler

global _isr_116
_isr_116:
		cli
		push	dword 0
		push	dword 116
		jmp		isr_common_handler

global _isr_117
_isr_117:
		cli
		push	dword 0
		push	dword 117
		jmp		isr_common_handler

global _isr_118
_isr_118:
		cli
		push	dword 0
		push	dword 118
		jmp		isr_common_handler

global _isr_119
_isr_119:
		cli
		push	dword 0
		push	dword 119
		jmp		isr_common_handler

global _isr_120
_isr_120:
		cli
		push	dword 0
		push	dword 120
		jmp		isr_common_handler

global _isr_121
_isr_121:
		cli
		push	dword 0
		push	dword 121
		jmp		isr_common_handler

global _isr_122
_isr_122:
		cli
		push	dword 0
		push	dword 122
		jmp		isr_common_handler

global _isr_123
_isr_123:
		cli
		push	dword 0
		push	dword 123
		jmp		isr_common_handler

global _isr_124
_isr_124:
		cli
		push	dword 0
		push	dword 124
		jmp		isr_common_handler

global _isr_125
_isr_125:
		cli
		push	dword 0
		push	dword 125
		jmp		isr_common_handler

global _isr_126
_isr_126:
		cli
		push	dword 0
		push	dword 126
		jmp		isr_common_handler

global _isr_127
_isr_127:
		cli
		push	dword 0
		push	dword 127
		jmp		isr_common_handler

global _isr_128
_isr_128:
		cli
		push	dword 0
		push	dword 128
		jmp		interrupt_handler_with_return_value

global _isr_129
_isr_129:
		cli
		push	dword 0
		push	dword 129
		jmp		isr_common_handler

global _isr_130
_isr_130:
		cli
		push	dword 0
		push	dword 130
		jmp		isr_common_handler

global _isr_131
_isr_131:
		cli
		push	dword 0
		push	dword 131
		jmp		isr_common_handler

global _isr_132
_isr_132:
		cli
		push	dword 0
		push	dword 132
		jmp		isr_common_handler

global _isr_133
_isr_133:
		cli
		push	dword 0
		push	dword 133
		jmp		isr_common_handler

global _isr_134
_isr_134:
		cli
		push	dword 0
		push	dword 134
		jmp		isr_common_handler

global _isr_135
_isr_135:
		cli
		push	dword 0
		push	dword 135
		jmp		isr_common_handler

global _isr_136
_isr_136:
		cli
		push	dword 0
		push	dword 136
		jmp		isr_common_handler

global _isr_137
_isr_137:
		cli
		push	dword 0
		push	dword 137
		jmp		isr_common_handler

global _isr_138
_isr_138:
		cli
		push	dword 0
		push	dword 138
		jmp		isr_common_handler

global _isr_139
_isr_139:
		cli
		push	dword 0
		push	dword 139
		jmp		isr_common_handler

global _isr_140
_isr_140:
		cli
		push	dword 0
		push	dword 140
		jmp		isr_common_handler

global _isr_141
_isr_141:
		cli
		push	dword 0
		push	dword 141
		jmp		isr_common_handler

global _isr_142
_isr_142:
		cli
		push	dword 0
		push	dword 142
		jmp		isr_common_handler

global _isr_143
_isr_143:
		cli
		push	dword 0
		push	dword 143
		jmp		isr_common_handler

global _isr_144
_isr_144:
		cli
		push	dword 0
		push	dword 144
		jmp		isr_common_handler

global _isr_145
_isr_145:
		cli
		push	dword 0
		push	dword 145
		jmp		isr_common_handler

global _isr_146
_isr_146:
		cli
		push	dword 0
		push	dword 146
		jmp		isr_common_handler

global _isr_147
_isr_147:
		cli
		push	dword 0
		push	dword 147
		jmp		isr_common_handler

global _isr_148
_isr_148:
		cli
		push	dword 0
		push	dword 148
		jmp		isr_common_handler

global _isr_149
_isr_149:
		cli
		push	dword 0
		push	dword 149
		jmp		isr_common_handler

global _isr_150
_isr_150:
		cli
		push	dword 0
		push	dword 150
		jmp		isr_common_handler

global _isr_151
_isr_151:
		cli
		push	dword 0
		push	dword 151
		jmp		isr_common_handler

global _isr_152
_isr_152:
		cli
		push	dword 0
		push	dword 152
		jmp		isr_common_handler

global _isr_153
_isr_153:
		cli
		push	dword 0
		push	dword 153
		jmp		isr_common_handler

global _isr_154
_isr_154:
		cli
		push	dword 0
		push	dword 154
		jmp		isr_common_handler

global _isr_155
_isr_155:
		cli
		push	dword 0
		push	dword 155
		jmp		isr_common_handler

global _isr_156
_isr_156:
		cli
		push	dword 0
		push	dword 156
		jmp		isr_common_handler

global _isr_157
_isr_157:
		cli
		push	dword 0
		push	dword 157
		jmp		isr_common_handler

global _isr_158
_isr_158:
		cli
		push	dword 0
		push	dword 158
		jmp		isr_common_handler

global _isr_159
_isr_159:
		cli
		push	dword 0
		push	dword 159
		jmp		isr_common_handler

global _isr_160
_isr_160:
		cli
		push	dword 0
		push	dword 160
		jmp		isr_common_handler

global _isr_161
_isr_161:
		cli
		push	dword 0
		push	dword 161
		jmp		isr_common_handler

global _isr_162
_isr_162:
		cli
		push	dword 0
		push	dword 162
		jmp		isr_common_handler

global _isr_163
_isr_163:
		cli
		push	dword 0
		push	dword 163
		jmp		isr_common_handler

global _isr_164
_isr_164:
		cli
		push	dword 0
		push	dword 164
		jmp		isr_common_handler

global _isr_165
_isr_165:
		cli
		push	dword 0
		push	dword 165
		jmp		isr_common_handler

global _isr_166
_isr_166:
		cli
		push	dword 0
		push	dword 166
		jmp		isr_common_handler

global _isr_167
_isr_167:
		cli
		push	dword 0
		push	dword 167
		jmp		isr_common_handler

global _isr_168
_isr_168:
		cli
		push	dword 0
		push	dword 168
		jmp		isr_common_handler

global _isr_169
_isr_169:
		cli
		push	dword 0
		push	dword 169
		jmp		isr_common_handler

global _isr_170
_isr_170:
		cli
		push	dword 0
		push	dword 170
		jmp		isr_common_handler

global _isr_171
_isr_171:
		cli
		push	dword 0
		push	dword 171
		jmp		isr_common_handler

global _isr_172
_isr_172:
		cli
		push	dword 0
		push	dword 172
		jmp		isr_common_handler

global _isr_173
_isr_173:
		cli
		push	dword 0
		push	dword 173
		jmp		isr_common_handler

global _isr_174
_isr_174:
		cli
		push	dword 0
		push	dword 174
		jmp		isr_common_handler

global _isr_175
_isr_175:
		cli
		push	dword 0
		push	dword 175
		jmp		isr_common_handler

global _isr_176
_isr_176:
		cli
		push	dword 0
		push	dword 176
		jmp		isr_common_handler

global _isr_177
_isr_177:
		cli
		push	dword 0
		push	dword 177
		jmp		isr_common_handler

global _isr_178
_isr_178:
		cli
		push	dword 0
		push	dword 178
		jmp		isr_common_handler

global _isr_179
_isr_179:
		cli
		push	dword 0
		push	dword 179
		jmp		isr_common_handler

global _isr_180
_isr_180:
		cli
		push	dword 0
		push	dword 180
		jmp		isr_common_handler

global _isr_181
_isr_181:
		cli
		push	dword 0
		push	dword 181
		jmp		isr_common_handler

global _isr_182
_isr_182:
		cli
		push	dword 0
		push	dword 182
		jmp		isr_common_handler

global _isr_183
_isr_183:
		cli
		push	dword 0
		push	dword 183
		jmp		isr_common_handler

global _isr_184
_isr_184:
		cli
		push	dword 0
		push	dword 184
		jmp		isr_common_handler

global _isr_185
_isr_185:
		cli
		push	dword 0
		push	dword 185
		jmp		isr_common_handler

global _isr_186
_isr_186:
		cli
		push	dword 0
		push	dword 186
		jmp		isr_common_handler

global _isr_187
_isr_187:
		cli
		push	dword 0
		push	dword 187
		jmp		isr_common_handler

global _isr_188
_isr_188:
		cli
		push	dword 0
		push	dword 188
		jmp		isr_common_handler

global _isr_189
_isr_189:
		cli
		push	dword 0
		push	dword 189
		jmp		isr_common_handler

global _isr_190
_isr_190:
		cli
		push	dword 0
		push	dword 190
		jmp		isr_common_handler

global _isr_191
_isr_191:
		cli
		push	dword 0
		push	dword 191
		jmp		isr_common_handler

global _isr_192
_isr_192:
		cli
		push	dword 0
		push	dword 192
		jmp		isr_common_handler

global _isr_193
_isr_193:
		cli
		push	dword 0
		push	dword 193
		jmp		isr_common_handler

global _isr_194
_isr_194:
		cli
		push	dword 0
		push	dword 194
		jmp		isr_common_handler

global _isr_195
_isr_195:
		cli
		push	dword 0
		push	dword 195
		jmp		isr_common_handler

global _isr_196
_isr_196:
		cli
		push	dword 0
		push	dword 196
		jmp		isr_common_handler

global _isr_197
_isr_197:
		cli
		push	dword 0
		push	dword 197
		jmp		isr_common_handler

global _isr_198
_isr_198:
		cli
		push	dword 0
		push	dword 198
		jmp		isr_common_handler

global _isr_199
_isr_199:
		cli
		push	dword 0
		push	dword 199
		jmp		isr_common_handler

global _isr_200
_isr_200:
		cli
		push	dword 0
		push	dword 200
		jmp		isr_common_handler

global _isr_201
_isr_201:
		cli
		push	dword 0
		push	dword 201
		jmp		isr_common_handler

global _isr_202
_isr_202:
		cli
		push	dword 0
		push	dword 202
		jmp		isr_common_handler

global _isr_203
_isr_203:
		cli
		push	dword 0
		push	dword 203
		jmp		isr_common_handler

global _isr_204
_isr_204:
		cli
		push	dword 0
		push	dword 204
		jmp		isr_common_handler

global _isr_205
_isr_205:
		cli
		push	dword 0
		push	dword 205
		jmp		isr_common_handler

global _isr_206
_isr_206:
		cli
		push	dword 0
		push	dword 206
		jmp		isr_common_handler

global _isr_207
_isr_207:
		cli
		push	dword 0
		push	dword 207
		jmp		isr_common_handler

global _isr_208
_isr_208:
		cli
		push	dword 0
		push	dword 208
		jmp		isr_common_handler

global _isr_209
_isr_209:
		cli
		push	dword 0
		push	dword 209
		jmp		isr_common_handler

global _isr_210
_isr_210:
		cli
		push	dword 0
		push	dword 210
		jmp		isr_common_handler

global _isr_211
_isr_211:
		cli
		push	dword 0
		push	dword 211
		jmp		isr_common_handler

global _isr_212
_isr_212:
		cli
		push	dword 0
		push	dword 212
		jmp		isr_common_handler

global _isr_213
_isr_213:
		cli
		push	dword 0
		push	dword 213
		jmp		isr_common_handler

global _isr_214
_isr_214:
		cli
		push	dword 0
		push	dword 214
		jmp		isr_common_handler

global _isr_215
_isr_215:
		cli
		push	dword 0
		push	dword 215
		jmp		isr_common_handler

global _isr_216
_isr_216:
		cli
		push	dword 0
		push	dword 216
		jmp		isr_common_handler

global _isr_217
_isr_217:
		cli
		push	dword 0
		push	dword 217
		jmp		isr_common_handler

global _isr_218
_isr_218:
		cli
		push	dword 0
		push	dword 218
		jmp		isr_common_handler

global _isr_219
_isr_219:
		cli
		push	dword 0
		push	dword 219
		jmp		isr_common_handler

global _isr_220
_isr_220:
		cli
		push	dword 0
		push	dword 220
		jmp		isr_common_handler

global _isr_221
_isr_221:
		cli
		push	dword 0
		push	dword 221
		jmp		isr_common_handler

global _isr_222
_isr_222:
		cli
		push	dword 0
		push	dword 222
		jmp		isr_common_handler

global _isr_223
_isr_223:
		cli
		push	dword 0
		push	dword 223
		jmp		isr_common_handler

global _isr_224
_isr_224:
		cli
		push	dword 0
		push	dword 224
		jmp		isr_common_handler

global _isr_225
_isr_225:
		cli
		push	dword 0
		push	dword 225
		jmp		isr_common_handler

global _isr_226
_isr_226:
		cli
		push	dword 0
		push	dword 226
		jmp		isr_common_handler

global _isr_227
_isr_227:
		cli
		push	dword 0
		push	dword 227
		jmp		isr_common_handler

global _isr_228
_isr_228:
		cli
		push	dword 0
		push	dword 228
		jmp		isr_common_handler

global _isr_229
_isr_229:
		cli
		push	dword 0
		push	dword 229
		jmp		isr_common_handler

global _isr_230
_isr_230:
		cli
		push	dword 0
		push	dword 230
		jmp		isr_common_handler

global _isr_231
_isr_231:
		cli
		push	dword 0
		push	dword 231
		jmp		isr_common_handler

global _isr_232
_isr_232:
		cli
		push	dword 0
		push	dword 232
		jmp		isr_common_handler

global _isr_233
_isr_233:
		cli
		push	dword 0
		push	dword 233
		jmp		isr_common_handler

global _isr_234
_isr_234:
		cli
		push	dword 0
		push	dword 234
		jmp		isr_common_handler

global _isr_235
_isr_235:
		cli
		push	dword 0
		push	dword 235
		jmp		isr_common_handler

global _isr_236
_isr_236:
		cli
		push	dword 0
		push	dword 236
		jmp		isr_common_handler

global _isr_237
_isr_237:
		cli
		push	dword 0
		push	dword 237
		jmp		isr_common_handler

global _isr_238
_isr_238:
		cli
		push	dword 0
		push	dword 238
		jmp		isr_common_handler

global _isr_239
_isr_239:
		cli
		push	dword 0
		push	dword 239
		jmp		isr_common_handler

global _isr_240
_isr_240:
		cli
		push	dword 0
		push	dword 240
		jmp		isr_common_handler

global _isr_241
_isr_241:
		cli
		push	dword 0
		push	dword 241
		jmp		isr_common_handler

global _isr_242
_isr_242:
		cli
		push	dword 0
		push	dword 242
		jmp		isr_common_handler

global _isr_243
_isr_243:
		cli
		push	dword 0
		push	dword 243
		jmp		isr_common_handler

global _isr_244
_isr_244:
		cli
		push	dword 0
		push	dword 244
		jmp		isr_common_handler

global _isr_245
_isr_245:
		cli
		push	dword 0
		push	dword 245
		jmp		isr_common_handler

global _isr_246
_isr_246:
		cli
		push	dword 0
		push	dword 246
		jmp		isr_common_handler

global _isr_247
_isr_247:
		cli
		push	dword 0
		push	dword 247
		jmp		isr_common_handler

global _isr_248
_isr_248:
		cli
		push	dword 0
		push	dword 248
		jmp		isr_common_handler

global _isr_249
_isr_249:
		cli
		push	dword 0
		push	dword 249
		jmp		isr_common_handler

global _isr_250
_isr_250:
		cli
		push	dword 0
		push	dword 250
		jmp		isr_common_handler

global _isr_251
_isr_251:
		cli
		push	dword 0
		push	dword 251
		jmp		isr_common_handler

global _isr_252
_isr_252:
		cli
		push	dword 0
		push	dword 252
		jmp		isr_common_handler

global _isr_253
_isr_253:
		cli
		push	dword 0
		push	dword 253
		jmp		isr_common_handler

global _isr_254
_isr_254:
		cli
		push	dword 0
		push	dword 254
		jmp		isr_common_handler

global _isr_255
_isr_255:
		cli
		push	dword 0
		push	dword 255
		jmp		isr_common_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
fault_common_stub:               ; the common parts of the generic interrupt handler
		; save the registers
		pushad

		mov		eax, cr2
		push	eax

		; call the C function
		; extern	fault_handler ; the C interrupt handler
		; call	fault_handler

		; restore the registers
		add		esp, 4 ; cr2
		popad

		; pop error_code and interrupt_number off the stack
		add		esp, 8

  		; return to the code that got interrupted
		iret
.end:

interrupt_handler_with_return_value:
		pushad

		mov		eax, cr2
		push	eax

		; call the C function
		; extern	interrupt_handler
		; call	interrupt_handler

		; restore the registers
		add		esp, 4 ; cr2
		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		; don't pop eax. It contains the return value

		; pop eax, error_code and interrupt_number off the stack
		add		esp, 12

		; return to the code that got interrupted
		iret
.end:

irq_common_handler:
		; save the registers
		pushad

		mov		eax, cr2
		push	eax

		; call the C function
		; extern	irq_handler ; the C interrupt handler
		; call	irq_handler

		; restore the registers
		add		esp, 4 ; cr2
		popad

		; pop error_code and interrupt_number off the stack
		add		esp, 8

  		; return to the code that got interrupted
		iret
.end:

isr_common_handler:
