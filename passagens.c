#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "passagens.h"

#define NUM_INSTRUCOES 14
#define NUM_DIRETIVAS 5
#define NAO_ENCONTRADO -1
#define EXCESSO_OPERANDOS -2
#define FALTA_OPERANDOS -3
#define CONSTANTE 268 /*valores pra nao entrar em conflito com nada*/
#define VARIAVEL 269
#define LABEL 270
#define NUM_TOKENS 10

static const Instrucao instrucoes[NUM_INSTRUCOES] = {
                                        {"ADD", "01", 1},
                                        {"SUB", "02", 1},
                                        {"MULT", "03", 1},
                                        {"DIV", "04", 1},
                                        {"JMP", "05", 1},
                                        {"JMPN", "06", 1},
                                        {"JMPP", "07", 1},
                                        {"JMPZ", "08", 1},
                                        {"COPY", "09", 2},
                                        {"LOAD", "10", 1},
                                        {"STORE", "11", 1},
                                        {"INPUT", "12", 1},
                                        {"OUTPUT", "13", 1},
                                        {"STOP", "14", 0}
                                      };

static const Diretiva diretivas[NUM_DIRETIVAS] = {
                                      {"IF", 1, 0},
                                      {"EQU", 1, 0},
                                      {"SPACE", 0, 1},
                                      {"SPACE", 1, 1},
                                      {"CONST", 1, 1}
                                    };

static ListSimbolo *ls = NULL;
static int contPos = 0;
static int secText = 0;
static int erroCompilacao = 0;/*flag que define se a tabela de simbolos deve ser construida*/

void adicionaSimbolo(Simbolo sim) {
  ListSimbolo *aux = ls;
  if (ls == NULL) {
    ls = malloc(sizeof(ListSimbolo));
    ls->simbolo = sim;
    ls->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListSimbolo));
    aux->prox->simbolo = sim;
    aux->prox->prox = NULL;
  }
}

ListSimbolo *procuraSimbolo(const char *nomeSim) {
  ListSimbolo *aux = ls;
  while(aux != NULL && strcasecmp(nomeSim, aux->simbolo.nome) != 0) {
    aux = aux->prox;
  }
  return aux;
}

void removeSimbolo(Simbolo sim) {
  ListSimbolo *aux = procuraSimbolo(sim.nome);
  ListSimbolo *aux2 = ls;
  if (aux == NULL) return;
  while (aux2->prox != aux) aux2 = aux2->prox;
  aux2->prox = aux->prox;
  free(aux);
}

void esvaziaTabela() {
  ListSimbolo *aux;
  while (ls != NULL) {
    aux = ls;
    ls = ls->prox;
    free(aux);
  }
}

/*imprime a tabela de simbolos*/
void imprimeSimbolos() {
  ListSimbolo *aux = ls;
  if (ls == NULL) printf("Lista Vazia.\n");
  while (aux != NULL) {
    printf("%s: %d -> ", aux->simbolo.nome, aux->simbolo.posicao);
    switch (aux->simbolo.tipo) {
      case CONSTANTE:
        printf("constante -> ");
        break;
      case VARIAVEL:
        printf("variavel -> ");
        break;
      case LABEL:
        printf("label -> ");
        break;
      default:
        printf("ERRO -> ");
        break;
    }
    printf("valor: %d\n", aux->simbolo.valor);
    aux = aux->prox;
  }
}

/*retorna a posicao da instrucao no vetor ou valores de erro*/
int procuraInstrucao(const char *nome, int operandos) {
  int i;
  for (i = 0; i < NUM_INSTRUCOES; i++) {
    if (strcasecmp(nome, instrucoes[i].nome) == 0){
      if (operandos > instrucoes[i].operandos) return EXCESSO_OPERANDOS;
      if (operandos < instrucoes[i].operandos) return FALTA_OPERANDOS;
      return i;
    }
  }
  return NAO_ENCONTRADO;
}

int procuraInstrucaoNom(const char *nome) {
  int i;
  for (i = 0; i < NUM_INSTRUCOES; i++) {
    if (strcasecmp(nome, instrucoes[i].nome) == 0){
      return i;
    }
  }
  return NAO_ENCONTRADO;
}

/*retorna a posicao da diretiva no vetor ou valores de erro conforme o erro*/
int procuraDiretiva(const char *nome, int operandos){
  int retVal = NAO_ENCONTRADO;
  int i;
  for (i = 0; i < NUM_DIRETIVAS; i++) {
    if (strcasecmp(nome, diretivas[i].nome) == 0) {
      if (operandos > diretivas[i].operandos) retVal = EXCESSO_OPERANDOS;
      else if (operandos < diretivas[i].operandos) retVal = FALTA_OPERANDOS;
      else {
        retVal = i;
        break;
        if (strcasecmp(diretivas[i].nome, "SPACE") == 0) break; /*evita que SPACE sem operandos lance erro de falta de operands*/
      }

    }
  }
  return retVal;
}

int separaTokens(FILE *fp, char tokens[10][50]) {
  int i, j;
  char linha[160], *aux; /*cada linha tem NO MAXIMO cerca de 160 caracteres*/
  fscanf(fp, "%[^\n]", linha); /*pega a linha a qual o fp estava apontando*/
  if(feof(fp)) {
  	return -1;
  }
  fseek(fp, 1, SEEK_CUR); /*faz o fp sair do \n e ja apontar pra proxima linha*/
	aux = strtok(linha, " ,\t");
	strcpy(tokens[0], aux);
	for(i=1; i<NUM_TOKENS; i++){
		aux = strtok(NULL, " ,\t");
		if(aux == NULL) break; /*se a linha ja tiver acabado, antes de completar os 5 elementos, acaba com o loop*/
		strcpy(tokens[i], aux);
	}
  i--; /*faz i apontar para ultimo token valido*/
  for(j = 2; j < i; j++) {
    if (strlen(tokens[j]) < 3 && tokens[j][0] == '+') {
      strcat(tokens[j-1], tokens[j]);
    }
    if (tokens[j-2][strlen(tokens[j-2])-1] == '+') {
      strcat(tokens[j-2], tokens[j]);
      i -= 2;
    }
  }
  return i;
}

void getNumLinha(char *dest, const char *token) {
  if (strchr(token, '(')){
    strcpy(dest,token);
    memmove(dest, dest+1, strlen(dest));
  }
}

void validaTokens(int i, char tokens[10][50], const char *numLinha, int instPos) {
  int k, j, errTok = 0;
  for (k = 0; k < i; k++) {
    if (tokens[k] != NULL){
      if (!isalpha(tokens[k][0]) && tokens[k][0] != '_' && (strlen(tokens[k]) == 1 && k < instPos)) {
        printf("Token '%s' invalido (linha %s)\n", tokens[k], numLinha);
        erroCompilacao = 1;
      }
      for (j = 1; j < strlen(tokens[k]); j++) {
        if (!isalnum(tokens[k][j]) && tokens[k][j] != '_') {
          if (!(j == strlen(tokens[k]) - 1 && (tokens[k][j] == ':' && k == 0)) && !(tokens[k][j] == '+' && isdigit(tokens[k][j+1]) && isalpha(tokens[k][j-1]))){
            errTok = 1;
          }
        }
      }
      if (errTok == 1) {
        printf("Token '%s' invalido (inha %s)\n", tokens[k],  numLinha);
        erroCompilacao = 1;
      }
    }
  }
}

void validaSecao(const char tokens[10][50], const char *numLinha) {
  /*verifica se a secao eh valida*/
  if (strcasecmp(tokens[0], "section") == 0 && !(strcasecmp(tokens[1], "data") == 0 || strcasecmp(tokens[1], "text") == 0)){
    erroCompilacao = 1;
    printf("Seção invalida (linha %s).\n", numLinha);
  }
  /*se ele achar um simbolo novo na secao de texto ele é um label*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "text") == 0) secText = 1;

  /*se for na secao de dados ele é uma variavel e verifica se veio depois da section data*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "data") == 0){
    if (secText == 1) secText = 0;
    else {
      erroCompilacao = 1;
      printf("Secao de dados sem secao de texto anterior (linha %s)\n", numLinha);
    }
  }
}

int getInstPos(const char tokens[10][50], const char *numLinha, int i) {
  int instPos = 0, k;
  if (strchr(tokens[0], ':') != NULL) {
    for (k = 1; k < i + 1; k++) {
      if (strchr(tokens[k], ':') != NULL) {
        erroCompilacao = 1;
        printf("Dois labels declarados na mesma linha (linha %s)\n", numLinha);
        return (k == 1 ? 2 : 1);
      }
    }
    return 1;
  }
  return instPos;
}

int calculaEspaco(const char tokens[10][50], const char *numLinha, int instPos, int i) {
  int instrucao, diretiva, espaco = 0;
  int expParams = i - instPos - 1;

  if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0) {
    instrucao = procuraInstrucaoNom(tokens[instPos]); /*verifica qual eh a instrucao*/
    if (instrucao == NAO_ENCONTRADO){
      erroCompilacao = 1;
      if (procuraDiretiva(tokens[instPos], expParams) != NAO_ENCONTRADO) printf("Diretiva '%s' na secao de texto (linha %s)\n", tokens[instPos], numLinha);
      else printf("Instrucao desconhecida: '%s' (linha %s)\n", tokens[instPos], numLinha);
      return 0;
    } else espaco = instrucoes[instrucao].operandos + 1;
  } else if (secText == 0 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0) {
    diretiva = procuraDiretiva(tokens[instPos], expParams);/*verifica se a diretiva eh valida*/
    if (diretiva  < 0 ) {
      erroCompilacao = 1;
      switch (diretiva) {
        case NAO_ENCONTRADO:
          if (procuraInstrucao(tokens[instPos], expParams) != NAO_ENCONTRADO) printf("Instrucao '%s' na secao de dados (linha %s)\n", tokens[instPos], numLinha);
          else printf("Diretiva desconhecida: '%s' (linha %s)\n", tokens[instPos], numLinha);
          break;
        case EXCESSO_OPERANDOS:
          printf("Diretiva com excesso de operandos (linha %s)\n", numLinha);
          break;
        case FALTA_OPERANDOS:
          printf("Diretiva com operandos insuficientes (linha %s)\n", numLinha);
          break;
        default:
          printf("ERRO DE EXECUCAO\n");
          break;
      }
      return 0;
    }
    else {
      espaco = diretivas[diretiva].espaco;
      /*O espaco eh usado diferente para space com parametros, o espaco eh ocupado pelo valor passado como parametro*/
      if (strcasecmp(diretivas[diretiva].nome, "SPACE") == 0 && diretivas[diretiva].operandos == 1)
        espaco *= atoi(tokens[instPos + 1]);
    }
  }
  return espaco;
}

int getErroCompilacao(){
  return erroCompilacao;
}

void primeiraPassagem(FILE *fp){
	int i;
	char rotulo[50];
  char tokens[10][50], numLinha[10];
  int simPos, instPos = 0, espaco = 0, valor = 0;
  int simTipo = 0;
  Simbolo simb;


	i = separaTokens(fp, tokens);
  /*remove o '(' do indicador de linha*/
  if(i > -1){
    getNumLinha(numLinha, tokens[i]);

    /*Verifica se a instrucao eh o primeiro ou segundo token*/
    instPos = getInstPos(tokens, numLinha, i);

    /*verifica se os tokens sao validos*/
    validaTokens(i, tokens, numLinha, instPos);

    /*verifica se a secao eh valida*/
    validaSecao(tokens, numLinha);


    espaco = calculaEspaco(tokens, numLinha, instPos, i);


    /*Construcao da tabela de simbolos*/
    if (strchr(tokens[0], ':') != NULL)  {
      strcpy(rotulo, tokens[0]);
      rotulo[strlen(rotulo)-1] = '\0';
      simPos = contPos;

      /*define o tipo de simbolo*/
      if (strcasecmp(tokens[instPos], "CONST") == 0){ /*se for chamado por um const eh constante*/
        simTipo = CONSTANTE;
        valor = atoi(tokens[instPos + 1]);
      }
      if (secText == 1) simTipo = LABEL; /*se estiver na secao de texto eh um label*/
      else if (simTipo != CONSTANTE) simTipo = VARIAVEL;/*caso contrario eh variavel*/

      /*verifica se o simbolo ja existe antes de adicionar à tabela*/
      if (procuraSimbolo(rotulo) == NULL) {
        strcpy(simb.nome, rotulo);
        simb.tipo = simTipo;
        simb.posicao = simPos;
        simb.valor = valor;
        simb.tam = espaco;
        adicionaSimbolo(simb);
      }
      else {
        erroCompilacao = 1;
        printf("Erro '%s' declarado pela segunda vez (linha %s)\n", rotulo, numLinha);
      }
    }

    /*avanca o contador de posicoes*/
    contPos += espaco;
  }
  else if (secText == 1) {
    erroCompilacao = 1;
    printf("Secao de dados ausente.\n");
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

void verificaSecaoAtual(const char tokens[10][50]) {
  if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "text") == 0) secText = 1;
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "data") == 0) secText = 0;
}

void verificaEspacoAlocado(Simbolo simb, int offset, const char *numLinha){
  if(offset > (simb.tam - 1)){
    erroCompilacao = 1;
    printf("Erro! Tentativa de manipulacao de espaco nao alocado. (linha %s)\n", numLinha);
  }
}

void segundaPassagem(FILE *fp, FILE *fpfinal){
  int i, j; /*i eh o maior indice do vetor de tokens da linha dada*/
  char *rotulo;
  char tokens[10][50], numLinha[10];
  int instrucao, diretiva, expParams, instPos = 0, offset = 0, offset2 = 0, operando = 0; /*o operando so eh usado no space e no const*/
  ListSimbolo *Lsimb1 = NULL, *Lsimb2 = NULL;

  i = separaTokens(fp, tokens);
  if(i > -1){ /*Esse if evita bugs no fim da leitura do arquivo*/

    /*remove o '(' do indicador de linha*/
    getNumLinha(numLinha, tokens[i]);

    /*Verifica se a instrucao eh o primeiro ou segundo token (ou seja, o indice 0 ou 1 do vetor de tokens)*/
    instPos = getInstPos(tokens, numLinha, i);

    /*checa se a secao atual eh a de texto ou a de dados, modificando a flag secText*/
    verificaSecaoAtual(tokens);

    /*Calcula a quantidade de operandos*/
    expParams = i - instPos - 1;

    if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0) { /*Se for uma instrucao...*/
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
      } 
      else { /*ja que o numero de operandos eh valido, resta agora saber se os tipos deles sao validos, para cada instrucao. Se for, gera codigo objeto*/
        if((instrucao >= 0 && instrucao <= 3) || instrucao == 9 || instrucao == 12){ /*ADD, SUB, MULT, DIV, LOAD, OUTPUT*/
          rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
          Lsimb1 = procuraSimbolo(rotulo);
          if(Lsimb1 != NULL){
            verificaEspacoAlocado(Lsimb1->simbolo, offset, numLinha);
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
            verificaEspacoAlocado(Lsimb1->simbolo, offset, numLinha);
            verificaEspacoAlocado(Lsimb2->simbolo, offset2, numLinha);
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
            verificaEspacoAlocado(Lsimb1->simbolo, offset, numLinha);
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
    } 
    else if (strcasecmp(tokens[0], "SECTION") != 0 && i != 0) { /*Se for uma diretiva...*/
      diretiva = procuraDiretiva(tokens[instPos], expParams);
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
          } 
          else {
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