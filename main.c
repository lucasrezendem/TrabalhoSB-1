#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	if(argc > 4){
		printf("ERRO: ARGUMENTOS EM EXCESSO! DIGA SOMENTE O TIPO DE OPERACAO, O ARQUIVO DE ENTRADA E O ARQUIVO DE SAIDA.\n");
		return -1;
	} 
	else if(argc < 4){
		printf("ERRO: FALTAM ARGUMENTOS! DIGA O TIPO DE OPERACAO, O ARQUIVO DE ENTRADA E O ARQUIVO DE SAIDA.\n");
		return -1;
	} 

	/*printf("%s\n", argv[0]);
	printf("%s\n", argv[1]);
	printf("%s\n", argv[2]);
	printf("%s\n", argv[3]);*/

	if(!strcmp(argv[1], "-p")){
		printf("---EXECUTA PREPROCESSAMENTO---\n");
	} 
	else if(!strcmp(argv[1], "-m")){
		printf("---EXECUTA EXPANSAO DE MACROS---\n");
	}
	else if(!strcmp(argv[1], "-o")){
		printf("---EXECUTA MONTAGEM COMPLETA---\n");
	}
	else{
		printf("ERRO: TIPO DE OPERACAO INVALIDO!\n");
		return -1;
	}

	return 0;
}