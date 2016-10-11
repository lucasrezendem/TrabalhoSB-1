typedef struct simbolo {
  char nome[50];
  int label;
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
void adicionaSimbolo(Simbolo sim, ListSimbolo *ls);
ListSimbolo *procuraSimbolo(const char *NomeSim, ListSimbolo *ls);
void removeSimbolo(Simbolo sim, ListSimbolo *ls);
void esvaziaTabela(ListSimbolo *ls);

/*Operacoes sobre as tabelas de instrucoes e diretivas*/
int procuraInstrucao(const char *nome, int operandos);
int procuraDiretiva(const char *nome, int operandos);
