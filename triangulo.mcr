section data
	b:	space
	h:	space
	r:	space
	dois:	const	2
section text
	copy  testamacro,  b
	copy  triangulo,  testamacro
	mult  teste2,  teste2
	store  teste3,  teste3
	input  teste2,  teste1
	copy  b,  triangulo
	input	b
	input	h
	load	b
	mult	h
	div	dois
	store	r
	output	r
	stop
