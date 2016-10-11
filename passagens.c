#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "passagens.h"

#define NUM_INSTRUCOES 14
#define NUM_DIRETIVAS 5
#define NAO_ENCONTRADO -1
#define ERRO_OPERANDOS -2

static const Instrucao instrucoes[] = {
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

static const Diretiva diretivas[] = {
                                      {"IF", 1, 0},
                                      {"EQU", 1, 0},
                                      {"SPACE", 0, 1},
                                      {"SPACE", 1, 1},
                                      {"CONST", 1, 1}
                                    };

void adicionaSimbolo(Simbolo sim, ListSimbolo *ls) {
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

ListSimbolo *procuraSimbolo(const char *nomeSim, ListSimbolo *ls) {
  ListSimbolo *aux = ls;
  while(aux != NULL && strcasecmp(nomeSim, aux->simbolo.nome) != 0) {
    aux = aux->prox;
  }
  return aux;
}

void removeSimbolo(Simbolo sim, ListSimbolo *ls) {
  ListSimbolo *aux = procuraSimbolo(sim.nome, ls);
  ListSimbolo *aux2 = ls;
  if (aux == NULL) return;
  while (aux2->prox != aux) aux2 = aux2->prox;
  aux2->prox = aux->prox;
  free(aux);
}

void esvaziaTabela(ListSimbolo *ls) {
  ListSimbolo *aux;
  while (ls != NULL) {
    aux = ls;
    ls = ls->prox;
    free(aux);
  }
}

int procuraInstrucao(const char *nome, int operandos) {
  int i;
  for (i = 0; i < NUM_INSTRUCOES; i++) {
    if (strcasecmp(nome, instrucoes[i].nome) == 0){
      if (operandos != instrucoes[i].operandos) return ERRO_OPERANDOS;
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
      if (operandos != diretivas[i].operandos) retVal = ERRO_OPERANDOS;
      else retVal = i;
    }
  }
  return retVal;
}
