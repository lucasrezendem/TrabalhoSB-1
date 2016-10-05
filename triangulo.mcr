section data
	b:	space
	h:	space
	r:	space
	dois:	const	2
section text
	copy  &a,  &t
	copy  &b,  &a
	copy  &t,  &b
	end  macro
swap testamacro, triangulo, b
	input	b
	input	h
	load	b
	mult	h
	div	dois
	store	r
	output	r
	stop
