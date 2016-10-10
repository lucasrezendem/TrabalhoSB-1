typedef struct simbolo {
  char nome[50];
  int label;
} Simbolo;

typedef struct listSimbolo {
  Simbolo simbolo;
  Simbolo* prox;
} ListSimbolo;

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
