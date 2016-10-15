#ifndef PASSAGENS_H
#define PASSAGENS_H

typedef struct simbolo {
  char nome[50];
  int tipo;
  int posicao;
  int valor;
} Simbolo;

typedef struct listSimbolo ListSimbolo;

struct listSimbolo {
  Simbolo simbolo;
  ListSimbolo *prox;
};

typedef struct instrucao {
  char nome[7];
  char opcode[3];
  int operandos;
} Instrucao;

typedef struct diretiva {
  char nome[6];
  int operandos;
  int espaco;
} Diretiva;

/*Operacoes sobre a tabela de símbolos*/
void adicionaSimbolo(Simbolo sim);
ListSimbolo *procuraSimbolo(const char *NomeSim);
void removeSimbolo(Simbolo sim);
void esvaziaTabela();
void imprimeSimbolos();

/*Operacoes sobre as tabelas de instrucoes e diretivas*/
int procuraInstrucao(const char *nome, int operandos);
int procuraDiretiva(const char *nome, int operandos);

void primeiraPassagem(FILE *fp);

#endif
