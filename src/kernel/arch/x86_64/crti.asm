[BITS 64]

SECTION		.init

[GLOBAL		_init]
_init:
		push		rbp
		mov			QWORD rbp, rsp
		;	gcc will nicely put the contents of crtbegin.o's .init section here

SECTION	.fini

[GLOBAL		_fini]
_fini:
		push		rbp
		mov			QWORD rbp, rsp
		;	gcc will nicely put the contents of crtbegin.o's .init section here
