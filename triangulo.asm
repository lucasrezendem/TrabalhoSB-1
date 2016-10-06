swap2: macro &1, &2, &3
	mult &2, &2
	store &3, &3
	input &2, &1
	end macro

swap: macro &a, &b, &t
	copy &a, &t
	copy &b, &a
	swap2 teste1, teste2, teste3
	copy &t, &b
	end macro
section data
	b:	space
	h:	space
	r:	space
	dois:	const	2
	triangulo: equ 1
	TESTAMACRO: EQU 1
Section text
	IF TESTAMACRO
	swap testamacro, triangulo, b
	input	b
	input	h
	load	b
	mult	h
	IF TRIANGULO
	div	dois
	store	r
	output	r
	stop
