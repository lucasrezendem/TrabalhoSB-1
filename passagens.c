#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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
static int constroiTabela = 1;

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

int procuraDiretiva(const char *nome, int operandos){
  int retVal = NAO_ENCONTRADO;
  int i;
  for (i = 0; i < NUM_DIRETIVAS; i++) {
    if (strcasecmp(nome, diretivas[i].nome) == 0) {
      if (operandos > diretivas[i].operandos) retVal = EXCESSO_OPERANDOS;
      else if (operandos < diretivas[i].operandos) retVal = FALTA_OPERANDOS;
      else {
        retVal = i;
        if (strcasecmp(diretivas[i].nome, "SPACE") == 0) break; /*evita que SPACE sem operandos lance erro de falta de operands*/
      }

    }
  }
  return retVal;
}

void separa_tokens(FILE *fp, int passagem){
  /*TODO: fazer a traducao e escrita no arquivo objeto*/
	int i, j , k;
	char linha[160], *aux, rotulo[50]; /*cada linha tem NO MAXIMO cerca de 160 caracteres*/
  char tokens[10][50], numLinha[10];
  int simPos, params = 0, instPos = 0, instrucao, diretiva, espaco, valor = 0, expParams = 0;
  int simTipo = 0, errTok = 0;
  Simbolo simb;


	fscanf(fp, "%[^\n]", linha); /*pega a linha a qual o fp estava apontando*/
	if(feof(fp)) {
		return;
	}
	fseek(fp, 1, SEEK_CUR); /*faz o fp sair do \n e ja apontar pra proxima linha*/

	aux = strtok(linha, " ,\t");
	strcpy(tokens[0], aux);
	for(i=1; i<NUM_TOKENS; i++){
		aux = strtok(NULL, " ,\t");
		if(aux == NULL) break; /*se a linha ja tiver acabado, antes de completar os 5 elementos, acaba com o loop*/
		strcpy(tokens[i], aux);
	}
  i--;
  if (strchr(tokens[i], '(')){
    strcpy(numLinha,tokens[i]);
    memmove(numLinha, numLinha+1, strlen(numLinha));
  }
  /*verifica se a secao eh valida*/
  if (strcasecmp(tokens[0], "section") == 0 && !(strcasecmp(tokens[1], "data") == 0 || strcasecmp(tokens[1], "text") == 0)){
    constroiTabela = 0;
    printf("Seção invalida (linha %s).\n", numLinha);
  }
  /*se ele achar um simbolo novo na secao de texto ele é um label*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "text") == 0) secText = 1;

  /*se for na secao de dados ele é uma variavel e verifica se veio depois da section data*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "data") == 0){
    if (secText == 1) secText = 0;
    else {
      constroiTabela = 0;
      printf("Secao de dados sem secao de texto anterior (linha %s)\n", numLinha);
    }
  }

  instPos = 0;
  if (strchr(tokens[0], ':') != NULL) {
    if (strchr(tokens[1], ':') != NULL) {
      constroiTabela = 0;
      printf("Dois labels declarados na mesma linha (linha %s)\n", numLinha);
      instPos = 2;
    }
    else instPos = 1;
  }
  expParams = i - instPos - 1; /*subtrair 1 quando botar o esquema da linha*/
  if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0) {
    instrucao = procuraInstrucao(tokens[instPos], expParams);
    if (instrucao < 0 ){
      constroiTabela = 0;
      if (passagem == 2){/*verifica erros se for a segunda passagem*/
        switch (instrucao) {
          case NAO_ENCONTRADO:
            printf("Instrucao desconhecida: %s (linha %s)\n", tokens[instPos], numLinha);
            break;
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
    } else {
      params = instrucoes[instrucao].operandos;
      espaco = params + 1;
    }
  } else if (strcasecmp(tokens[0], "SECTION") != 0 && i != 0) {
    diretiva = procuraDiretiva(tokens[instPos], expParams);
    if (diretiva  < 0 ) {
      constroiTabela = 0;
      if (passagem == 2){ /*verifica erros se for a segunda passagem*/
        switch (diretiva) {
          case NAO_ENCONTRADO:
            printf("Diretiva desconhecida: %s (linha %s)\n", tokens[instPos], numLinha);
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
      }
    }
    else{
      params = diretivas[diretiva].operandos;
      espaco = diretivas[diretiva].espaco;
    }
  }

  for (k = 0; k < i - 1; k++) {
    if (tokens[k] != NULL){
      if ((tokens[k][0] < 'a' || tokens[k][0] > 'z') && (tokens[k][0] < 'A' || tokens[k][0] > 'Z') && tokens[k][0] != '_') {
        printf("Nome de token invalido 1( %c linha %s)\n", tokens[k][0], numLinha);
        constroiTabela = 0;
      }
      for (j = 1; j < strlen(tokens[k]); j++) {
        if ((tokens[k][j] < 'a' || tokens[k][j] > 'z') && (tokens[k][j] < 'A' || tokens[k][j] > 'Z') && tokens[k][j] != '_' && (tokens[k][j] < 48 || tokens[k][j] > 57)) {
          errTok = 1;
          if (j == strlen(tokens[k]) - 1 && tokens[k][j] == ':' && k == 0){
            errTok = 0;
          }
        }
        printf("%d\n", errTok);
      }
      if (errTok == 1) {
        printf("Nome de token invalido 2(linha %s)\n", numLinha);
        constroiTabela = 0;
      }
    }
  }

  /*Construcao da tabela de simbolos*/
  if (strchr(tokens[0], ':') != NULL && passagem == 1 && constroiTabela == 1)  {
    strcpy(rotulo, tokens[0]);
    rotulo[strlen(rotulo)-1] = '\0';
    simPos = contPos;
    if (strcasecmp(tokens[instPos], "CONST") == 0){
      simTipo = CONSTANTE;
      valor = atoi(tokens[instPos + 1]);
    }
    if (secText == 1) simTipo = LABEL;
    else if (simTipo != CONSTANTE) simTipo = VARIAVEL;
    if(strcasecmp(tokens[instPos], "SPACE") == 0 && params == 1) {
      espaco *= atoi(tokens[instPos + 1]);
    }
    if (procuraSimbolo(rotulo) == NULL) {
      strcpy(simb.nome, rotulo);
      simb.tipo = simTipo;
      simb.posicao = simPos;
      simb.valor = valor;
      adicionaSimbolo(simb);
    }
    else {
      constroiTabela = 0;
      printf("Erro %s declarado pela segunda vez (linha %s)\n", rotulo, numLinha);
    }
  }
  if (constroiTabela == 0) esvaziaTabela();
  contPos += espaco;
}
