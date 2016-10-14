#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passagens.h"

int main(int argc, char const *argv[]) {
  FILE *fp;
  fp = fopen("triangulo.mcr", "r");
  if(!fp){
    printf("ERRO AO ABRIR O ARQUIVO!\n");
    return -1;
  }
  while (!feof(fp)) separa_tokens(fp,1);
  imprimeSimbolos();
  esvaziaTabela();
  fclose(fp);
  return 0;
}
