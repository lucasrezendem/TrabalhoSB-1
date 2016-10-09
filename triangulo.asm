triangulo: equ 1
TESTAMACRO: EQU

swap2: macro &1, &2, &3
	mult &2, &2
	store &3, &3 ;testando comentario
	input &2, &1
	end macro

swap: macro &a, &b, &t
	copy &a, &t
	copy &b, &a
	swap2 teste1, teste2, teste3 ;teasfas
	copy &t, &b
	end macro

Section text
	IF aESTAMACRO
	swap testamacro, triangulo, b
	inpt	b
	input	h
	lod	b
	mult	h
	IF TRIANGULO
	div	dois
	store	r
	output	r
	stop

section data
	b:	space
	h:	space
	r:	space
	dois:	const	2
