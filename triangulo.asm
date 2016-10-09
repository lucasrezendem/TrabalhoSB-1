triangulo equ 1	(1
EQU	(2
	(3
swap2: macro &1, &2, &3	(4
	mult &2, &2	(5
	store &3, &3 	(6
	input &2, &1	(7
	end macro	(8
	(9
swap: macro &a, &b, &t	(10
	copy &a, &t	(11
	copy &b, &a	(12
	swap2 teste1, teste2, teste3 	(13
	copy &t, &b	(14
	end macro	(15
	(16
SECTION TEXT	(17
		INPUT		B	(18
		INPUT		H	(19
		LOAD		B	(20
		MULT		H	(21
		IF 	(22
		DIV		DOIS	(23
		STORE		R	(24
		OUTPUT	R	(25
		STOP	(26
	(27
SECTION DATA	(28
	B:		SPACE	(29
	H:		SPACE	(30
	R:		SPACE	(31
	DOIS:	CONST		2	(32
