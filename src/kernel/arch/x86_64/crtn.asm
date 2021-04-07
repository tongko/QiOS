[BITS	64]

SECTION		.init
		; gcc will nicely put the contents of crtend.o's .init section here. */
		pop		rbp
		ret

SECTION		.fini
		; gcc will nicely put the contents of crtend.o's .init section here. */
		pop		rbp
		ret
