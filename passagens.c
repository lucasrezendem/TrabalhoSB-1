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
  int i;
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
  return i;
}

void getNumLinha(char *dest, const char *token) {
  if (strchr(token, '(')){
    strcpy(dest,token);
    memmove(dest, dest+1, strlen(dest));
  }
}

void validaTokens(int i, const char tokens[10][50], const char *numLinha) {
  int k, j, errTok = 0;
  for (k = 0; k < i - 1; k++) {
    if (tokens[k] != NULL){
      if (!isalpha(tokens[k][0]) && tokens[k][0] != '_') {
        printf("Token '%s' invalido (linha %s)\n", tokens[k], numLinha);
        erroCompilacao = 1;
      }
      for (j = 1; j < strlen(tokens[k]); j++) {
        if (!isalnum(tokens[k][j]) && tokens[k][j] != '_') {
          if (!(j == strlen(tokens[k]) - 1 && (tokens[k][j] == ':' && k == 0))){
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

int getInstPos(const char tokens[10][50], const char *numLinha) {
  int instPos = 0;
  if (strchr(tokens[0], ':') != NULL) {
    if (strchr(tokens[1], ':') != NULL) {
      erroCompilacao = 1;
      printf("Dois labels declarados na mesma linha (linha %s)\n", numLinha);
      instPos = 2;
    }
    else instPos = 1;
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
  } else if (strcasecmp(tokens[0], "SECTION") != 0 && i != 0) {
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

    /*verifica se os tokens sao validos*/
    validaTokens(i, tokens, numLinha);

    /*verifica se a secao eh valida*/
    validaSecao(tokens, numLinha);

    /*Verifica se a instrucao eh o primeiro ou segundo token*/
    instPos = getInstPos(tokens, numLinha);

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
