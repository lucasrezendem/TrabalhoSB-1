triangulo equ 1
TESTAMACRO EQU 1
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
		
SECTION TEXT		
		INPUT		B		
		INPUT		H		
		LOAD		B		
		MULT		H		
		IF 	TRIANGULO	
		DIV		DOIS		
		STORE		R		
		OUTPUT	R		
		STOP		
		
SECTION DATA		
	B:		SPACE		
	H:		SPACE		
	R:		SPACE		
	DOIS:	CONST		2
