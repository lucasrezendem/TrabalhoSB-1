#ifndef PASSAGENS_H
#define PASSAGENS_H

typedef struct simbolo {
  char nome[50];
  int tipo;
  int posicao;
  int valor;
  int tam;
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
int procuraInstrucaoNom(const char *nome);
int procuraDiretiva(const char *nome, int operandos);

int separaTokens(FILE *fp, char tokens[10][50]);
void getNumLinha(char *dest, const char *token);
void validaTokens(int i, char tokens[10][50], const char *numLinha, int instPos);
void validaSecao(const char tokens[10][50], const char *numLinha);
int getInstPos(const char tokens[10][50], const char *numLinha, int i);
int calculaEspaco(const char tokens[10][50], const char *numLinha, int instPos, int i);

void primeiraPassagem(FILE *fp);

char* separaTokenDoOffset(char *token, int *offset);
int getErroCompilacao();
void verificaSecaoAtual(const char tokens[10][50]);
void verificaEspacoAlocado(Simbolo simb, int offset, const char *numLinha);
void verificaStops();

void segundaPassagem(FILE *fp, FILE *fpfinal);

void duasPassagens(char *nomeArquivoIN, char *nomeArquivoOUT);

#endif
