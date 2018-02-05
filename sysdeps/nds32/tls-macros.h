#define TLS_LE(x) 						\
({	int *__l;						\
	asm(							\
		"sethi   %0, hi20("#x"@TPOFF)\n\t"		\
		"ori     %0, %0, lo12("#x"@TPOFF)\n\t"		\
		"add	%0, %0, $r25\n\t"			\
	:"=r"(__l));						\
	__l;})

#ifdef PIC
# define TLS_IE(x)						\
({	int *__l;						\
	asm(							\
		".relax_hint begin\n\t"				\
		"sethi	%0, hi20("#x"@GOTTPOFF)\n\t"		\
		".relax_hint\n\t"				\
		"ori	%0, %0, lo12("#x"@GOTTPOFF)\n\t"	\
		".relax_hint end\n\t"				\
		"lw	%0, [%0 + $gp]\n\t"			\
		"add	%0, %0, $r25\n\t"			\
		:"=r"(__l));					\
	__l;})
#else
# define TLS_IE(x) 						\
({	int *__l;						\
	asm(							\
		".relax_hint begin\n\t"				\
		"sethi 	%0, hi20("#x"@GOTTPOFF)\n\t"		\
		".relax_hint end\n\t"				\
		"lwi 	%0, [%0 + lo12("#x"@GOTTPOFF)]\n\t"	\
		"add	%0, %0, $r25\n\t"			\
	:"=r"(__l));						\
	__l;})
#endif

#define TLS_LD(x) TLS_GD(x)

#define TLS_GD(x)						\
({	int *__l;	                                        \
	asm(							\
		"smw.adm $r1,[$sp],$r5,#0\n\t"			\
		"smw.adm $r16,[$sp],$r24,#0\n\t"		\
		".relax_hint begin\n\t"			\
	        "sethi	$r0, hi20("#x"@TLSDESC)\n\t"		\
		".relax_hint\n\t"				\
		"ori	$r0, $r0, lo12("#x"@TLSDESC)\n\t"	\
		".relax_hint\n\t"				\
		"lw 	$r15, [$r0 + $gp]\n\t"			\
		".relax_hint\n\t"				\
		"add 	$r0, $r0, $gp\n\t"			\
		".relax_hint end\n\t"			\
	 	"jral 	$r15\n\t"	        		\
		"lmw.bim $r16,[$sp],$r24,#0\n\t"                \
		"lmw.bim $r1,[$sp],$r5,#0\n\t"                  \
		"move	%0, $r0\n\t"				\
		:"=r"(__l)					\
		:						\
		:"$r0","$r15");					\
	__l;})
