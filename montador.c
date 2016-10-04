#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


typedef struct listAux {
	int valor;
	char *nome;
	struct listAux *next;

} list;

void libera (list *aux1){
list *aux2 = aux1;

   while (aux1 != NULL)
    {
       aux2 = aux1;
       aux1 = aux1->next;
       free(aux2);
    }

}

list *add (list *l, int val, char *label){
	list *novo = NULL;

	novo = malloc(sizeof(list));
	novo->valor = val;
	novo->nome =label;
	novo-> next = l;
	return novo;

}

int checaIf (list *l, char *nome){
	list *aux = l;
	int a;
	printf("\n\n%s>> ", nome);
	while (aux != NULL){
		for(a = 0; aux->nome[a]; a++){
  				aux->nome[a] = tolower(aux->nome[a]);
		}
		for(a = 0; nome[a]; a++){
  				nome[a] = tolower(nome[a]);
		}
		if (strcmp(aux->nome,nome)==0){
			printf("%s // %s>> ", aux->nome, nome);
			/* if foi definido antes.*/
			return aux->valor;
		}
		aux = aux->next;
	}


return -1; /* erro. if não foi definido antes*/
}

void leAsm (char *nome){
	char palavra[50], b, c,nomeExt[200], label[50];
	int a, tam, valor, resposta;
	list *equl = NULL;
	FILE *fp = fopen (nome, "r");
	if(!fp){
		printf("Erro ao abrir arquivo .asm");
	}
	tam = strlen (nome);
	strcpy(nomeExt, nome);
	nomeExt[tam-3]='\0';
	strcat(nomeExt,"pre");
	

	FILE *fp1 = fopen (nomeExt, "w+");

	if(!fp1){
		printf("Erro ao criar abrir arquivo .pre");
	}
	

	while (c!=EOF){
		c= fgetc(fp);
		if (c==';'){
			do{
				c= fgetc(fp);
			}while (c!=EOF && c!=';');
			c= fgetc(fp);
		}



		if(c!='\n' && c!= ' ' && c!= '\t'&& c!=EOF){
			fseek(fp, -1 ,SEEK_CUR);
			fscanf(fp, "%s", palavra);
			for(a = 0; palavra[a]; a++){
  				palavra[a] = tolower(palavra[a]);
			}

			if (strcmp (palavra,"equ")==0){ /*checa se palavra encontrada é diretiva EQU*/
				while (c!='\n'){ /*volta para começo da linha*/
					fseek(fp, -2, SEEK_CUR);
					fseek(fp1, -1, SEEK_CUR);
					c=fgetc(fp);
				}
				fseek(fp1, 3, SEEK_CUR);
				fscanf(fp, "%s", label);
				printf("%s", label); /* pega nome label*/
				fscanf(fp, "%s", palavra); /*pega equ de novo*/
				fscanf(fp, "%d", &valor); /* pega valor*/
				equl = add (equl, valor, label); /*adiciona em uma lista, os valores encontrados dos labels de equ*/
				getchar();
				c='\0';
				while (c!='\n'){ /* pulando linha do equ*/
					c=fgetc(fp);
				}
				strcpy(palavra,"\0");

			}
			if (strcmp (palavra,"if")==0){ /*checa se palavra encontrada é diretiva IF*/
				fscanf(fp, "%s", palavra);
				resposta = checaIf (equl, palavra);
				while (c!='\n'){
						fseek(fp1, -2, SEEK_CUR);
						c=fgetc(fp1);
				}
				c=fgetc(fp);
				c=fgetc(fp);
				printf("RESPOSTA - %d", resposta);
				getchar();
				c = '\0';
				if (resposta == 0){ /*caso for 0, pula linha seguinte ao if*/
					while (c!='\n'){
						c=fgetc(fp);
					}

				}
			strcpy(palavra,"\0");
			}
			printf("\n %s", palavra);
			fprintf(fp1, "%s", palavra);
			c= fgetc(fp);
			fprintf(fp1, "%c", c);
			if (c=='\n' || c== '\t' || c == ' '){
				do{
					b = fgetc(fp);
				}while(c==b && b!=EOF);
				fseek(fp, -1 ,SEEK_CUR);

			} 
		}
		else{
			do{
				b = fgetc(fp);
			}while(c==b);
			if(b==EOF || c==EOF)
				break;
			fseek(fp, -1 ,SEEK_CUR);
			fprintf(fp1, "%c", c);
		}
	}
	getchar();
	fclose(fp);
	fclose(fp1);
	libera(equl);
}

void lePre (char *nome){
	char nomeExt[200], pri[50], sec[50], c;
	int tam;
	FILE *fp = fopen (nome, "r");
	if(!fp){
		printf("Erro ao abrir arquivo .asm");
	}
	tam = strlen (nome);
	strcpy(nomeExt, nome);
	nomeExt[tam-3]='\0';
	strcat(nomeExt,"mcr");
	

	FILE *fp1 = fopen (nomeExt, "w+");

	if(!fp1){
		printf("Erro ao criar abrir arquivo .pre");
	} 

	if (!feof(fp)){
		fscanf(fp, "%s", pri);
		/* botar pra escrever APARTIR de section data*/
		fscanf(fp, "%s", sec);
		if (strcmp(sec, "TEXT")){
		/* aqui começa section text. botar pra escrever tbm tudo dela*/
			while (c!=EOF){
				while (c!=EOF && c!= '\n'){
					c= fgetc(fp);
				}
				fscanf(fp, "%s", pri);
				if (strcmp(pri,"add")!=0 && strcmp(pri,"sub")!=0 && strcmp(pri,"mul")!=0 && strcmp(pri,"div")!=0 && strcmp(pri,"jmp")!=0 && strcmp(pri,"jmpn")!=0 && strcmp(pri,"jmpp")!=0 && strcmp(pri,"jmpz")!=0 && strcmp(pri,"copy")!=0 && strcmp(pri,"load")!=0 && strcmp(pri,"store")!=0 && strcmp(pri,"input")!=0 && strcmp(pri,"output")!=0 && strcmp(pri,"stop")!=0){
			/*procurar a macro*/

		}

			}
		}
	

	}
	




	fclose(fp);
	fclose(fp1);



}

int main (){

	leAsm("triangulo.asm");
	printf("saiu!!");
	lePre("triangulo.pre");
return 0;
}
