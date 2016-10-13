#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void limpa_linhas(char *nome_arquivo){
	FILE *fp;
	int i, j;
	char programa[216][159]; /*216 eh o numero maximo de palavras nesse assembly; cada linha tem no maximo 159 caracteres (50+50+50+6+3)*/
	char aux;
	fp = fopen(nome_arquivo, "r");
	if(!fp){
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		return;
	}

	for(i=0; i<216 && !feof(fp); i++){

		fscanf(fp, "%[^\n(]", programa[i]); /*pega uma linha toda ate achar um \n ou um (*/
		do{
			if(feof(fp)) break;
			aux = fgetc(fp); 
		} while(aux != '\n'); /*esse loop procura por um \n*/
	}

	fclose(fp);
	fp = fopen(nome_arquivo, "w"); /*agora que eu ja tenho o programa todo num vetor, posso criar um novo arquivo com o mesmo nome, porem limpo*/
	if(!fp){
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		return;
	}

	for(j=0; j<i; j++){
		fprintf(fp, "%s\n", programa[j]);
	}

	fclose(fp);
}

void separa_tokens(FILE *fp, char tokens[5][50]){ /*ASSIM QUE POSSIVEL, RETIRE TODOS OS PRINTFS DESSA FUNCAO!!*/
	int i;
	char linha[160], *aux; /*cada linha tem NO MAXIMO cerca de 160 caracteres*/

	fscanf(fp, "%[^\n]", linha); /*pega a linha a qual o fp estava apontando*/
	printf("%ld\n", ftell(fp));
	if(feof(fp)) {
		printf("ARQUIVO ENCERRADO!\n");
		return;
	}
	fseek(fp, 1, SEEK_CUR); /*faz o fp sair do \n e ja apontar pra proxima linha*/

	aux = strtok(linha, " ,\t");
	printf("token 0: %s\n", aux);
	strcpy(tokens[0], aux);
	for(i=1; i<5; i++){
		aux = strtok(NULL, " ,\t");
		if(aux == NULL) break; /*se a linha ja tiver acabado, antes de completar os 5 elementos, acaba com o loop*/
		printf("token %d: %s\n", i, aux);
		strcpy(tokens[i], aux);
	}
}

int main(int argc, char *argv[]){
	if(argc > 4){
		printf("ERRO: ARGUMENTOS EM EXCESSO! DIGA SOMENTE O TIPO DE OPERACAO, O ARQUIVO DE ENTRADA E O ARQUIVO DE SAIDA.\n");
		return -1;
	} 
	else if(argc < 4){
		printf("ERRO: FALTAM ARGUMENTOS! DIGA O TIPO DE OPERACAO, O ARQUIVO DE ENTRADA E O ARQUIVO DE SAIDA.\n");
		return -1;
	} 



	char tokens[5][50];
	FILE *fp;
	fp = fopen("triangulo.mcr", "r");
	if(!fp){
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		return -1;
	}



	if(!strcmp(argv[1], "-p")){
		printf("---EXECUTA PREPROCESSAMENTO---\n");
		limpa_linhas(argv[2]);
		while(!feof(fp)) separa_tokens(fp, tokens); /*CHAMADA SOMENTE PRA TESTE! ASSIM QUE PUDER, RETIRE ESSA LINHA*/
	} 
	else if(!strcmp(argv[1], "-m")){
		printf("---EXECUTA EXPANSAO DE MACROS---\n");
		limpa_linhas(argv[2]);
	}
	else if(!strcmp(argv[1], "-o")){
		printf("---EXECUTA MONTAGEM COMPLETA---\n");
	}
	else{
		printf("ERRO: TIPO DE OPERACAO INVALIDO!\n");
		return -1;
	}

	fclose(fp);

	return 0;
}