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

static const Instrucao instrucoes[NUM_INSTRUCOES] = {
                                        {"ADD", "01", 1},
                                        {"SUB", "02", 1},
                                        {"MUL", "03", 1},
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

void adicionaSimbolo(Simbolo sim) {
  ListSimbolo *aux = ls;
  if (ls == NULL) {
    ls = malloc(sizeof(ListSimbolo));
    ls->simbolo = sim;
    ls->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListSimbolo));
    aux->simbolo = sim;
    aux->prox = NULL;
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
  while (aux != NULL) {
    printf("%s: %d -> ", aux->simbolo.nome, aux->simbolo.posicao);
    if (aux->simbolo.label == 1) printf("label\n");
    else printf("variavel\n");
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

void separa_tokens(FILE *fp, int passagem){ /*ASSIM QUE POSSIVEL, RETIRE TODOS OS PRINTFS DESSA FUNCAO!!*/
  /*TODO: adicionar linha onde ocorreu o erro*/
	int i;
	char linha[160], *aux, rotulo[50]; /*cada linha tem NO MAXIMO cerca de 160 caracteres*/
  char tokens[5][50];
  int simPos, params = 0, instPos, instrucao, diretiva, espaco, valor = 0;

	fscanf(fp, "%[^\n]", linha); /*pega a linha a qual o fp estava apontando*/
	if(feof(fp)) {
		return;
	}
	fseek(fp, 1, SEEK_CUR); /*faz o fp sair do \n e ja apontar pra proxima linha*/

	aux = strtok(linha, " ,\t");
	strcpy(tokens[0], aux);
	for(i=1; i<5; i++){
		aux = strtok(NULL, " ,\t");
		if(aux == NULL) {
      i--;
      break;
    } /*se a linha ja tiver acabado, antes de completar os 5 elementos, acaba com o loop*/
		strcpy(tokens[i], aux);
	}

  /*se ele achar um simbolo novo na secao de texto ele é um label*/
  if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "text") == 0) secText = 1;

  /*se for na secao de dados ele é uma variavel*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "data") == 0) secText = 0;

  instPos = 0;
  if (strchr(tokens[0], ':') != NULL) instPos = 1;
  if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0) {
    instrucao = procuraInstrucao(tokens[instPos], i - instPos);
    if (instrucao < 0 ){
      if (passagem == 2){/*verifica erros se for a segunda passagem*/
        switch (instrucao) {
          case NAO_ENCONTRADO:
            printf("Instrucao desconhecida: %s\n", tokens[instPos]);
            break;
          case EXCESSO_OPERANDOS:
            printf("Excesso de operandos\n");
            break;
          case FALTA_OPERANDOS:
            printf("Operandos insuficientes\n");
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
  } else {
    diretiva = procuraDiretiva(tokens[instPos], i - instPos);
    if (diretiva  < 0 ) {
      if (passagem == 2){ /*verifica erros se for a segunda passagem*/
        switch (diretiva) {
          case NAO_ENCONTRADO:
            printf("Diretiva desconhecida: %s\n", tokens[instPos]);
            break;
          case EXCESSO_OPERANDOS:
            printf("Excesso de operandos\n");
            break;
          case FALTA_OPERANDOS:
            printf("Operandos insuficientes\n");
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

  if (strchr(tokens[0], ':') != NULL && passagem == 1)  {
    strcpy(rotulo, tokens[0]);
    rotulo[strlen(rotulo)-1] = '\0';
    simPos = contPos;
    if (strcasecmp(tokens[instPos], "CONST") == 0) valor  = atoi(tokens[instPos + 1]);
    if(strcasecmp(tokens[instPos], "SPACE") == 0 && params == 1) {
      espaco *= atoi(tokens[instPos + 1]);
    }
    printf("nome: %s\tlabel: %d\tposicao: %d\tvalor: %d\n", rotulo, secText, simPos, valor);
    if (procuraSimbolo(rotulo) == NULL) {
      /*printf("adiciona\n" );*/
    }
    else printf("Erro %s declarado duas vezes\n", rotulo);
  }
  contPos += espaco;
}
