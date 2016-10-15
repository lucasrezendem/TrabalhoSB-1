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
		int tam = strlen(programa[i]);
		programa[i][tam-1]='\0';
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
	if(feof(fp)) return;
	fseek(fp, 1, SEEK_CUR); /*faz o fp sair do \n e ja apontar pra proxima linha*/

	aux = strtok(linha, " ,\t");
	strcpy(tokens[0], aux);
	for(i=1; i<5; i++){
		aux = strtok(NULL, " ,\t");
		if(aux == NULL) break; /*se a linha ja tiver acabado, antes de completar os 5 elementos, acaba com o loop*/
		strcpy(tokens[i], aux);
	}
}

/*Recebe um token que tem a possibilidade de ter um '+', e o separa entre o nome do simbolo e o offset */
char* separaTokenDoOffset(char *token, int *offset){
	char *aux, *nome_simbolo;
	
	nome_simbolo = strtok(token, "+");
	aux = strtok(NULL, "+");
	if(aux) *offset = atoi(aux);
	return nome_simbolo;
}


void segundaPassagem(FILE *fp, FILE *fpfinal){
	int i, j; /*i eh o maior indice do vetor de tokens da linha dada*/
	char *rotulo;
  	char tokens[10][50], numLinha[10];
	int expParams, instPos = 0, offset = 0, offset2 = 0, operando = 0; /*o operando so eh usado no space e no const*/
	ListSimbolo Lsimb1, Lsimb2 = NULL;

	i = separaTokens(fp, tokens);
  	if(i > -1){ /*Esse if evita bugs no fim da leitura do arquivo*/

	  	/*remove o '(' do indicador de linha*/
    	getNumLinha(numLinha, tokens[i]);

    	/*Verifica se a instrucao eh o primeiro ou segundo token (ou seja, o indice 0 ou 1 do vetor de tokens)*/
    	instPos = getInstPos(tokens, numLinha);

    	/*Calcula a quantidade de operandos*/
    	expParams = i - instPos - 1;

   		if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0) { /*verifica se eh uma instrucao ou uma diretiva*/
    		instrucao = procuraInstrucao(tokens[instPos], expParams); /*verifica se a instrucao eh valida*/
    		if (instrucao < 0 ){ /*se o numero de operandos for invalido...*/
      			erroCompilacao = 1;
      			switch (instrucao) {
        			case EXCESSO_OPERANDOS:
          				printf("Instrucao com excesso de operandos (linha %s)\n", numLinha);
          				break;
       				case FALTA_OPERANDOS:
          				printf("Instrucao com operandos insuficientes (linha %s)\n", numLinha);
          				break;
        			default:
          				printf("ERRO DE EXECUCAO\n");
          				break;
      			}
    		} else { /*ja que o numero de operandos eh valido, resta agora saber se os tipos deles sao validos, para cada instrucao. Se for, gera codigo objeto*/
      			if((instrucao >= 0 && instrucao <= 3) || instrucao == 9 || instrucao == 12){ /*ADD, SUB, MULT, DIV, LOAD, OUTPUT*/
      				rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
      				Lsimb1 = procuraSimbolo(rotulo);
      				if(Lsimb1 != NULL){
						if(Lsimb1->simbolo.tipo == VARIAVEL){
							fprintf(fpfinal, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao + offset);
						} 
						else if(Lsimb1->simbolo.tipo == CONSTANTE){
							if(instrucao == 3 && Lsimb1->simbolo.valor == 0){ /*verifica se o operando eh uma constante com valor zero*/
								erroCompilacao = 1;
								printf("Erro! Tentativa de divisao por zero! (linha %s)\n", numLinha);
							}
							fprintf(fpfinal, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao);
						}
						else{
			      			erroCompilacao = 1;
			      			printf("Instrucao com operandos de tipos invalidos (linha %s)\n", numLinha);
						}
					}
					else{
						erroCompilacao = 1;
			      		printf("Erro! Simbolo nao definido! (linha %s)\n", numLinha);
					}
      			} 
      			else if(instrucao >= 4 && instrucao <= 7){ /*JMP, JMPN, JMPP, JMPZ*/
      				Lsimb1 = procuraSimbolo(tokens[instPos+1]);
					if(Lsimb1 != NULL){
						if(Lsimb1->simbolo.tipo == LABEL){
							fprintf(fpfinal, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao);
						} 
						else{
			      			erroCompilacao = 1;
			      			printf("Instrucao com operandos de tipos invalidos (linha %s)\n", numLinha);
						}
					}
					else{
						erroCompilacao = 1;
			      		printf("Erro! Simbolo nao definido! (linha %s)\n", numLinha);
					}
      			} 
      			else if(instrucao == 8){ /*COPY*/
      				rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
      				Lsimb1 = procuraSimbolo(rotulo);
      				rotulo = separaTokenDoOffset(tokens[instPos+2], &offset2);
      				Lsimb2 = procuraSimbolo(rotulo);
      				if(Lsimb1 != NULL && Lsimb2 != NULL){
						if((Lsimb1->simbolo.tipo == VARIAVEL || Lsimb1->simbolo.tipo == CONSTANTE) && Lsimb2->simbolo.tipo == VARIAVEL){
							fprintf(fpfinal, "%s %d %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao + offset, Lsimb2->simbolo.posicao + offset2);
						} 
						else{
			      			erroCompilacao = 1;
			      			printf("Instrucao com operandos de tipos invalidos (linha %s)\n", numLinha);
						}
					}
					else{
						erroCompilacao = 1;
			      		printf("Erro! Simbolo nao definido! (linha %s)\n", numLinha);
					}
      			} 
      			else if(instrucao == 10 || instrucao == 11){ /*STORE, INPUT*/
      				rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
      				Lsimb1 = procuraSimbolo(rotulo);
      				if(Lsimb1 != NULL){
						if(Lsimb1->simbolo.tipo == VARIAVEL){
							fprintf(fpfinal, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao + offset);
						} 
						else{
			      			erroCompilacao = 1;
			      			printf("Instrucao com operandos de tipos invalidos (linha %s)\n", numLinha);
						}
					}
					else{
						erroCompilacao = 1;
			      		printf("Erro! Simbolo nao definido! (linha %s)\n", numLinha);
					}
      			} 
      			else if(instrucao == 13){ /*STOP*/
      				fprintf(fpfinal, "%s ", instrucoes[instrucao].opcode);
      			}
    		}
  		} else if (strcasecmp(tokens[0], "SECTION") != 0 && i != 0) {
    		diretiva = procuraDiretiva(tokens[instPos], expParams);
    		/*TODO: verificar os tipos dos operandos e, se der tudo certo, traduzir*/
    		switch (diretiva) {
    			case 2: /*SPACE sem argumentos*/
    				fprintf(fpfinal, "0 ");
    				break;
    			case 3: /*SPACE com argumentos*/
    				if((tokens[instPos+1][0]=='0' && (tokens[instPos+1][1]=='x' || tokens[instPos+1][1]=='X')) || 
						(tokens[instPos+1][1]=='0' && (tokens[instPos+1][2]=='x' || tokens[instPos+1][2]=='X'))) /*Se os primeiros caracteres forem 0x ou 0X ou -0x ou -0X, entao le como hexa*/
						sscanf(tokens[instPos+1], "%x", &operando);
					else sscanf(tokens[instPos+1], "%d", &operando); /*senao, le como decimal mesmo*/
					if(operando > 0){
						for(j=0; j<operando; j++) fprintf(fpfinal, "0 ");
					} else {
						erroCompilacao = 1;
			      		printf("Operando invalido para essa diretiva (linha %s)\n", numLinha);
					}
    				break;
    			case 4: /*CONST*/
    				if((tokens[instPos+1][0]=='0' && (tokens[instPos+1][1]=='x' || tokens[instPos+1][1]=='X')) || 
						(tokens[instPos+1][1]=='0' && (tokens[instPos+1][2]=='x' || tokens[instPos+1][2]=='X'))) /*Se os primeiros caracteres forem 0x ou 0X ou -0x ou -0X, entao le como hexa*/
						sscanf(tokens[instPos+1], "%x", &operando);
					else sscanf(tokens[instPos+1], "%d", &operando); /*senao, le como decimal mesmo*/
					fprintf(fpfinal, "%d ", operando);
    				break;
    		}
  		}

    	
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


	if(!strcmp(argv[1], "-p")){
		printf("---EXECUTA PREPROCESSAMENTO---\n");
		limpa_linhas(argv[3]);
	} 
	else if(!strcmp(argv[1], "-m")){
		printf("---EXECUTA EXPANSAO DE MACROS---\n");
		limpa_linhas(argv[3]);
	}
	else if(!strcmp(argv[1], "-o")){
		printf("---EXECUTA MONTAGEM COMPLETA---\n");
	}
	else{
		printf("ERRO: TIPO DE OPERACAO INVALIDO!\n");
		return -1;
	}

	limpa_linhas(argv[2]);

	return 0;
}
