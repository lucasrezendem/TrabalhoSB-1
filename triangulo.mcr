section data
	b:	space
	h:	space
	r:	space
	dois:	const	2
section text
	swap testamacro, triangulo, b
	input	b
	input	h
	load	b
	mult	h
	div	dois
	store	r
	output	r
	stop
