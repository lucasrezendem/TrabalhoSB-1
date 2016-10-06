#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct listMcr {
	char orig[50], eq[50];
	struct listMcr *next;

} listMcr;

void liberaMcr (listMcr *aux1){
listMcr *aux2 = aux1;

   while (aux1 != NULL)
    {
       aux2 = aux1;
       aux1 = aux1->next;
	printf("\n\nLIBERANDO : %s  - %s", aux2->orig, aux2->eq);
       free(aux2);
    }

}

listMcr *addMcr (listMcr *l, char label[50]){
	int size = strlen(label);
	if(label[size-1]==',')
		label[size-1]='\0';
	if(l){
		printf("\ninsert>>>: %s", l->eq);
	}
	listMcr *novo = (listMcr *)malloc(sizeof(listMcr)), *aux = l;

	if(novo == NULL) exit(0);
	strcpy(novo->orig,"");
	strcpy(novo->eq,label);
	    
	if(aux == NULL){
		novo->next = NULL;
		 return novo; 
	}
	while(aux->next !=NULL){
		aux=aux->next;
	}
	aux->next = novo;
	return l;
}

listMcr *updateMcr (listMcr *l, char label[50]){
	listMcr *aux = l;
	int size = strlen(label);
	if(label[size-1]==',')
		label[size-1]='\0';
	if(aux==NULL){
		return l;
	}

	while (aux->next != NULL && strcmp(aux->orig,"")!=0){
		aux = aux->next;
	}
	strcpy(aux->orig,label);
	
	return l;

}
	


typedef struct listAux {
	int valor;
	char nome[50];
	struct listAux *next;

} list;

void libera (list* aux1){
list *aux2 = aux1;

   do {
       aux2 = aux1->next;
	printf("aqui oh\n\nLIBERANDO : %s", aux1->nome);
	getchar();
       free(aux1);
	aux1=aux2;
    }while (aux1 != NULL);

}

list *add (list* l, int val, char label[50]){
	printf("\n\nADD%s..", label);
	if(l){
		printf(">>%s..", l->nome);
	}
	list * novo =(list *) malloc( sizeof(list) ), *aux;
	aux = l;
	if(novo == NULL) exit(0);
	novo->valor = val;
	strcpy(novo->nome,label);
	    
	if(aux == NULL){
		novo->next = NULL;
		 return novo; 
	}
	novo->next = aux;
return novo;

}

void imprime (list *lis) {
    list *aux=lis;
        printf("\n");
    if (aux) do {
        printf("\n\nIMPRIMINDO %s", aux->nome);
        aux = aux->next;
    }while(aux!=NULL);

}
int troca (FILE *fp,int val, char c,char aux[50], listMcr *lis){
	listMcr *laux = lis;
	int teste=0, virg=0;
	int size = strlen(aux);
	if(aux[size-1]==','){
		aux[size-1]='\0';
		virg=1;
	}
	fseek(fp,val,SEEK_SET);
	while (laux!=NULL){
		printf("\n\n>>>(%s/%s/%s)",aux,laux->eq,laux->orig);
		if(strcmp(aux,laux->eq)==0){
			fprintf(fp,"%c%s",c,laux->orig);
			if(virg==1)
				fprintf(fp,",");
			getchar();
			teste=1;
		}
		laux = laux->next;
	}
	if (teste==0 && (strcmp(aux,"add")==0 || strcmp(aux,"sub")==0 || strcmp(aux,"mult")==0 || strcmp(aux,"div")==0 || strcmp(aux,"jmp")==0 || strcmp(aux,"jmpn")==0 || strcmp(aux,"jmpp")==0 || strcmp(aux,"jmpz")==0 || strcmp(aux,"copy")==0 || strcmp(aux,"load")==0 || strcmp(aux,"store")==0 || strcmp(aux,"input")==0 || strcmp(aux,"output")==0 || strcmp(aux,"stop")==0 || strcmp(aux,"section")==0 ||  strcmp(aux,"data")==0 || strcmp(aux,"text")==0|| strcmp(aux,"end")==0|| strcmp(aux,"macro")==0|| aux[0]=='&')){
		fprintf(fp,"%c%s",c,aux);
		teste=1;
	}
	if (teste==0 && strcmp(aux,"add")!=0 && strcmp(aux,"sub")!=0 && strcmp(aux,"mult")!=0 && strcmp(aux,"div")!=0 && strcmp(aux,"jmp")!=0 && strcmp(aux,"jmpn")!=0 && strcmp(aux,"jmpp")!=0 && strcmp(aux,"jmpz")!=0 && strcmp(aux,"copy")!=0 && strcmp(aux,"load")!=0 && strcmp(aux,"store")!=0 && strcmp(aux,"input")!=0 && strcmp(aux,"output")!=0 && strcmp(aux,"stop")!=0&& strcmp(aux,"section")!=0&& strcmp(aux,"data")!=0&& strcmp(aux,"text")!=0){
			return 1;
		}
return 0;
}

int checaMacro(char *func, char *nome, char *nome2, int pos, int pospre){
	long int val;
	listMcr *parametros = NULL;
	int tam, posM, resultado=0, resulaux=0;
	char aux[50], c;
	if (strcmp(func,"add")!=0 && strcmp(func,"sub")!=0 && strcmp(func,"mult")!=0 && strcmp(func,"div")!=0 && strcmp(func,"jmp")!=0 && strcmp(func,"jmpn")!=0 && strcmp(func,"jmpp")!=0 && strcmp(func,"jmpz")!=0 && strcmp(func,"copy")!=0 && strcmp(func,"load")!=0 && strcmp(func,"store")!=0 && strcmp(func,"input")!=0 && strcmp(func,"output")!=0 && strcmp(func,"stop")!=0&& strcmp(func,"section")!=0&& strcmp(func,"data")!=0&& strcmp(func,"text")!=0){
		FILE *fp = fopen (nome2, "r+"); /* .mcr*/
		if (!fp){
			printf("Erro ao abrir arquivo");
		}
		fseek(fp,-strlen(func),SEEK_CUR);

		FILE *fp1 = fopen (nome, "r"); /* .pre*/
		if (!fp1){
			printf("Erro ao abrir arquivo");
		}
		do{
			fscanf(fp1,"%s",aux);
			printf(">>%s", aux);
			tam = strlen(aux);
			aux[tam-1]='\0';
			printf(">>%s", aux);
			getchar();
			if(strcmp(aux,func)==0){
				printf("ACHEI A MACRO CERTA!! %d", pos);
				fscanf(fp1,"%s",aux); /*caso precise, confere que é uma macro mesmo*/
				posM = ftell(fp1);
				while(c!='\n'){
					c = fgetc(fp1);
					if(c!='\n'){
						fscanf(fp1,"%s",aux);
						printf("PARAMETRO 2: %s ",aux);
						parametros = addMcr (parametros, aux);
					}
				}
				c = '\0';
				fseek(fp1,pospre,SEEK_SET);
				while(c!='\n'){
					c = fgetc(fp1);
					if(c!='\n'){
						fscanf(fp1,"%s",aux);
						printf("PARAMETRO 1: %s ",aux);
						parametros = updateMcr (parametros, aux);
						getchar();
					}
				}
				c = '\0';
				fseek(fp1,posM,SEEK_SET);
				while(c!='\n'){
					c = fgetc(fp1);
					printf("%c", c);
					getchar();
				}
				
				fseek(fp,pos-1,SEEK_SET);
				while(c!=EOF && strcmp(aux,"macro")!=0){
					c = fgetc(fp1);
					if(c!='\n'){
						fscanf(fp1,"%s",aux);
						/*fprintf(fp,"%c%s",c,aux);*/
						
						if(strcmp(aux,"macro")==0)
								break;
						resultado = troca (fp, ftell(fp),c,aux,parametros);
						if(resultado==1){
							fseek(fp,1,SEEK_CUR);
							resulaux = checaMacro(aux, nome, nome2, ftell(fp), ftell(fp1));
							printf("\n\nSAIIIII BANZAI%d",resulaux);
							resultado = 0;
						}
						if(resulaux!=0){
							fseek(fp,resulaux,SEEK_SET);
							strcpy(aux,"");
							c='\0';
							while(c!='\n'){
								c = fgetc(fp1);
							}
							resulaux=0;
						}
						else{
							c = fgetc(fp1);
							fprintf(fp,"%c",c);
						}
						fseek(fp1,-1,SEEK_CUR);
					}
				}
				c='\0';
				while(c!='\n'){ /*retira o end macro*/
					fseek(fp,-2,SEEK_CUR);
					c = fgetc(fp);
				}
				break;
				getchar();
			}
			
		}while (!feof(fp1) || strcmp (aux,"section")!=0);
		c = '\0';

		while(c!='\n'){ /*pula a linha que chamou a macro*/
			c = fgetc(fp1);
		}
		
		val = ftell(fp);
		printf("\n\nFTEEL %ld",val);
		getchar();
		printf("\n\nFTEEL %ld",ftell(fp));
		fclose(fp1);
		fclose(fp);
		liberaMcr (parametros);
		return val/*+resultado*/;

	}
	else
		return 0;


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
				while (c!='\n'&& c!=EOF){ /* pulando linha do equ*/
					c=fgetc(fp);
				}
				strcpy(palavra,"\0");

			}
			if (strcmp (palavra,"if")==0){ /*checa se palavra encontrada é diretiva IF*/
				fscanf(fp, "%s", palavra);
				resposta = checaIf (equl, palavra);
				while (c!='\n'&& c!=EOF){
						fseek(fp1, -2, SEEK_CUR);
						c=fgetc(fp1);
				}
				c=fgetc(fp);
				getchar();
				c = '\0';
				if (resposta == 0){ /*caso for 0, pula linha seguinte ao if*/
					while (c!='\n'){
						c=fgetc(fp);
					}

				}
			strcpy(palavra,"\0");
			}
			for(a = 0; palavra[a]; a++){
  				palavra[a] = tolower(palavra[a]);
			}
			printf("\n %s", palavra);
			fprintf(fp1, "%s", palavra);
			c= fgetc(fp);
			fprintf(fp1, "%c", tolower(c));
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
			fprintf(fp1, "%c", tolower(c));
		}
	}
	getchar();
	fclose(fp);
	fclose(fp1);
	imprime(equl);
	libera(equl);
}

void lePre (char *nome){
	printf("\n\nAQUI COMEÇA MACROOOOOOOOO\n\n");
	char nomeExt[200], pri[50], c, b;
	int tam, cont=0, begin=0, result=0, text=0;
	FILE *fp = fopen (nome, "r"); /*.pre*/
	if(!fp){
		printf("Erro ao abrir arquivo .pre");
	}
	tam = strlen (nome);
	strcpy(nomeExt, nome);
	nomeExt[tam-3]='\0';
	strcat(nomeExt,"mcr");
	

	FILE *fp1 = fopen (nomeExt, "w+"); /*.mcr*/

	if(!fp1){
		printf("Erro ao criar abrir arquivo .mcr");
	}
	
	while (!feof(fp) || c!=EOF){
		c='\0';
		fscanf(fp, "%s", pri);
		printf("%s", pri);
		if ((strcmp(pri,"section")==0 || strcmp(pri,"data")==0|| strcmp(pri,"text")==0)&&begin==0) {
			if (strcmp(pri,"section")==0)
				fprintf(fp1, "%s ", pri);
			cont++;
		}
		else{
			cont=0;
		}
		if(cont==2){
			begin = 1;
			
		}
		if (begin==1){
			c='\0';
			if(text==1){
				printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH\n\nPRIPRI %s", pri);
				getchar();
				result = checaMacro(pri, nome, nomeExt, ftell(fp1),ftell(fp));
			}
			printf("\n\nresult: %d", result);
			
			getchar();
			if(result!=0){
				fseek(fp1,result,SEEK_SET);
				strcpy(pri,"");
				c='\0';
				while(c!='\n'){
					c = fgetc(fp);
				}
			} 
			fprintf(fp1, "%s", pri);
			if (strcmp(pri,"section")==0 && begin ==1)
				text=1;
			while (c!='\n'&& c!=EOF){
				c=fgetc(fp);
				fprintf(fp1,"%c",c);
				printf("%c",c);
			}
			c=fgetc(fp);
			if(c==EOF)
				break;
			fseek(fp,-3,SEEK_CUR);
			
		}

		if (begin==1){
			if (c== '\t' || c == ' ')
				fprintf(fp1,"%c",c);
		}
		do{
			b = fgetc(fp);
		}while(c==b);
		if(b==EOF || c==EOF)
			break;


	}
	
	printf("\n\nCHEGA ATE FINAL YEY");
	fclose(fp);
	fclose(fp1);


}

int main (){

	leAsm("triangulo.asm");
	printf("saiu!!");
	lePre("triangulo.pre");
	printf("saiu!!");
return 0;
}
