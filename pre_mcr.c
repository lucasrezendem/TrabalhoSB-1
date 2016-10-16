#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct listMcr { /*estrutura de dados para armazenar informações de nome das macros*/
	char orig[50], eq[50];
	struct listMcr *next;

} listMcr;

void liberaMcr (listMcr *aux1){
listMcr *aux2 = aux1;

   while (aux1 != NULL)
    {
       aux2 = aux1;
       aux1 = aux1->next;
       free(aux2);
    }

}

listMcr *addMcr (listMcr *l, char label[50]){
	int size = strlen(label);
	if(label[size-1]==',')
		label[size-1]='\0';

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
	


typedef struct listAux { /*estrutura que armazena parametros definidos com equ*/
	int valor;
	char nome[50];
	struct listAux *next;

} list;

void libera (list* aux1){
list *aux2 = aux1;

   while (aux1 != NULL){
       aux2 = aux1->next;
       free(aux1);
	aux1=aux2;
    }

}

list *add (list* l, int val, char label[50]){ /*insere valor definido com equ*/
	int size = strlen(label);
	if(label[size-1]==':')
		label[size-1] = '\0';
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
int checaIf (list *l, char *nome){
	list *aux = l;
	int a;
	while (aux != NULL){
		for(a = 0; aux->nome[a]; a++){
  				aux->nome[a] = tolower(aux->nome[a]);
		}
		for(a = 0; nome[a]; a++){
  				nome[a] = tolower(nome[a]);
		}
		
		if (strcmp(aux->nome,nome)==0){/* if foi definido antes.*/
			return aux->valor;
		}
		aux = aux->next;
	}


return -1; /* erro. if não foi definido antes*/
}
void tiraComentario (char *nome){
	char c ='\0', nomeExt[200];

	int tam;

	FILE *fp = fopen (nome,"r+"); /*arquivo original*/
	if(!fp){
		printf("Erro ao abrir arquivo");
		exit(1);
	}

	tam = strlen (nome);
	strcpy(nomeExt, nome);
	nomeExt[tam-4]='\0';
	strcat(nomeExt,"2.asm");

	FILE *fp1 = fopen (nomeExt,"w+"); /*arquivo com o numero das linhas*/
	if(!fp1){
		printf("Erro ao abrir arquivo");
		exit(1);
	}

	while(c!=EOF || !feof(fp)){
		c= fgetc(fp);
		if (c==';'){
			do{
				c= fgetc(fp);
			}while (c!=EOF && c!='\n');
			
		}
		fprintf(fp1,"%c",c);
	
	}
	fclose(fp);
	fclose(fp1);

	remove(nome); /*deleta arquivo original*/
	rename(nomeExt,nome); /*e o substitui pelo novo*/



}
void colocaLinha (char *nome){ /*insere numero das linhas para detecção de erros*/
	char c ='\0', nomeExt[200];
	int i=1;

	int tam;

	FILE *fp = fopen (nome,"r+"); /*arquivo original*/
	if(!fp){
		printf("Erro ao abrir arquivo");
		exit(1);
	}

	tam = strlen (nome);
	strcpy(nomeExt, nome);
	nomeExt[tam-4]='\0';
	strcat(nomeExt,"2.asm");

	FILE *fp1 = fopen (nomeExt,"w+"); /*arquivo com o numero das linhas*/
	if(!fp1){
		printf("Erro ao abrir arquivo");
		exit(1);
	}

	while(c!=EOF || !feof(fp)){
		while(c!='\n'){
			c=fgetc(fp);
			if(c==-1){ /*EOF*/
				break;
			} 
			if(c!='\n')
				fprintf(fp1,"%c",c);
		}
		if(c==-1) /*EOF*/
			break;
		fprintf(fp1,"\t(%d",i);
		fprintf(fp1,"\n");
		i++;
		if(c!=EOF) 
			c='\0';

	}
	fclose(fp);
	fclose(fp1);

	remove(nome); /*deleta arquivo original*/
	rename(nomeExt,nome); /*e o substitui pelo novo*/
}
void checaAntes (char *nome){
	char nomMacro[200], macro[50], c='\0';
	int tam, result, linha;
	list *l = NULL;
	FILE *fp = fopen (nome,"r+");
	if(!fp){
		printf("Erro ao abrir arquivo");
		exit(1);
	}
	while (c!=EOF){
		c=fgetc(fp);
		if(c==':'){
			
			while(c!='\n' && c!=EOF && ftell(fp)>1){
				fseek(fp,-2,SEEK_CUR);
				c=fgetc(fp);
			}
			fscanf(fp,"%s",nomMacro);
			
			tam = strlen(nomMacro);
			nomMacro[tam-1]='\0';
			fscanf(fp,"%s",macro);
			if(strcmp(macro,"macro")==0){
				l = add (l, ftell(fp), nomMacro);
			
			}
		}
		
	}
	fseek(fp,0,SEEK_SET);
	c='\0';
	 while (c!=EOF){
		c=fgetc(fp);
		if(c!=EOF){
			fseek(fp,-1,SEEK_CUR);
			fscanf(fp,"%s",macro);
			result = checaIf (l, macro);
			if(result!=-1){
				if(result>ftell(fp)){
					c='\0';
					while(c!='\n'){
						c=fgetc(fp);
					}
					c='\0';
					while(c!='('){
						fseek(fp,-2,SEEK_CUR);
						c=fgetc(fp);
					}
					c='\0';
					fscanf(fp,"%d", &linha);
					printf("\nERRO >> erro semantico detectado na linha: %d (Você não pode chamar uma macro antes de definir a mesma)", linha);
					
				}
				
			}
		}
		
	}
	fclose(fp);

	free(l);
}
void arrumaTopoFim (char *nome){
	char c,/* b,*/nomeExt[50];
	int pode=0, tam;
	FILE *fp = fopen (nome,"r+");
	if(!fp){
		printf("erro ao abrir arquivo");
		exit(1);
	}
	
	tam = strlen (nome);
	strcpy(nomeExt, nome);
	nomeExt[tam-4]='\0';
	strcat(nomeExt,"2.pre");

	FILE *fp1 = fopen (nomeExt,"w+"); /*arquivo com o numero das linhas*/
	if(!fp1){
		printf("Erro ao abrir arquivo");
		exit(1);
	}

	while(c!=EOF || !feof(fp)){
		c=fgetc(fp);
		if(c>58 && c<123)
			pode = 1;
		if(pode==1 && c!=EOF){ 
			fprintf(fp1,"%c",c);
		}

	}
	fclose(fp);
	fclose(fp1);

	remove(nome); /*deleta arquivo original*/
	rename(nomeExt,nome); /*e o substitui pelo novo*/
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
		if(strcmp(aux,laux->eq)==0){
			fprintf(fp,"%c%s",c,laux->orig);
			if(virg==1)
				fprintf(fp,",");
			teste=1;
		}
		laux = laux->next;
	}
	
	if (teste==0 && (strcmp(aux,"add")==0 || strcmp(aux,"sub")==0 || strcmp(aux,"mult")==0 || strcmp(aux,"div")==0 || strcmp(aux,"jmp")==0 || strcmp(aux,"jmpn")==0 || strcmp(aux,"jmpp")==0 || strcmp(aux,"jmpz")==0 || strcmp(aux,"copy")==0 || strcmp(aux,"load")==0 || strcmp(aux,"store")==0 || strcmp(aux,"input")==0 || strcmp(aux,"output")==0 || strcmp(aux,"stop")==0 || strcmp(aux,"section")==0 ||  strcmp(aux,"data")==0 || strcmp(aux,"text")==0|| strcmp(aux,"endmacro")==0|| strcmp(aux,"macro")==0|| aux[0]=='&')){
		fprintf(fp,"%c%s",c,aux);
		printf("dadsa");
		teste=1;
	}
	if (teste==0 && strcmp(aux,"add")!=0 && strcmp(aux,"sub")!=0 && strcmp(aux,"mult")!=0 && strcmp(aux,"div")!=0 && strcmp(aux,"jmp")!=0 && strcmp(aux,"jmpn")!=0 && strcmp(aux,"jmpp")!=0 && strcmp(aux,"jmpz")!=0 && strcmp(aux,"copy")!=0 && strcmp(aux,"load")!=0 && strcmp(aux,"store")!=0 && strcmp(aux,"input")!=0 && strcmp(aux,"output")!=0 && strcmp(aux,"stop")!=0&& strcmp(aux,"section")!=0&& strcmp(aux,"data")!=0&& strcmp(aux,"text")!=0){
			printf("dadsa");
			printf("\nreturn 1 >> %s",aux);
			return 1;
		}
printf("dadsa");
return 0;
}

int checaMacro(char *func, char *nome, char *nome2, int pos, int pospre){
	long int val=0;
	listMcr *parametros = NULL;
	int tam=0, posM=0, resultado=0, resulaux=0;
	char aux[50], c='a', b='\0';
	if (strcmp(func,"add")!=0 && strcmp(func,"sub")!=0 && strcmp(func,"mult")!=0 && strcmp(func,"div")!=0 && strcmp(func,"jmp")!=0 && strcmp(func,"jmpn")!=0 && strcmp(func,"jmpp")!=0 && strcmp(func,"jmpz")!=0 && strcmp(func,"copy")!=0 && strcmp(func,"load")!=0 && strcmp(func,"store")!=0 && strcmp(func,"input")!=0 && strcmp(func,"output")!=0 && strcmp(func,"stop")!=0&& strcmp(func,"section")!=0&& strcmp(func,"data")!=0&& strcmp(func,"text")!=0){
		printf("\naquiiii22222222222");
		printf("aquiiii");printf("aquiiii");printf("aquiiii");
		getchar();
		FILE *fp = fopen (nome2, "r+"); /* .mcr*/
		if (!fp){
			printf("Erro ao abrir arquivo");
			return -1;
		}
		fseek(fp,-strlen(func),SEEK_CUR);

		FILE *fp1 = fopen (nome, "r"); /* .pre*/
		if (!fp1){
			printf("Erro ao abrir arquivo");
			return -1;
		}
		printf("aquiiii");
	printf("aquiiii");printf("aquiiii");printf("aquiiii");
		do{
			c= fgetc(fp1);
			b = fgetc(fp1);
			if(c==EOF||c==b){
				break;
			}
			else{	
				fseek(fp1,-2,SEEK_CUR);
			}
			fscanf(fp1,"%s",aux);
			
			tam = strlen(aux);
			aux[tam-1]='\0';
			printf("\nMACRO EXPAN   %s",aux);
			getchar();
			if(strcmp(aux,func)==0){
				printf("achei a macro!!");
				fscanf(fp1,"%s",aux);/*caso precise, pode usar para conferir que é uma macro mesmo, pq se for aux==macro*/
				printf("%s",aux);

				c='\0';
				posM = ftell(fp1);
				while(c!='\n'){
					c = fgetc(fp1);
					if(c!='\n'){
						fscanf(fp1,"%s",aux);
						parametros = addMcr (parametros, aux);
					}
				}
				c = '\0';
				fseek(fp1,pospre,SEEK_SET);
				while(c!='\n'){
					c = fgetc(fp1);
					if(c!='\n'){
						fscanf(fp1,"%s",aux);
						parametros = updateMcr (parametros, aux);
					}
				}
				c = '\0';
				fseek(fp1,posM,SEEK_SET);
				while(c!='\n'){
					c = fgetc(fp1);
				}
				printf("((ANTES DAIS %s)))",aux);
				fseek(fp,pos-1,SEEK_SET);
				while(c!=EOF && strcmp(aux,"endmacro")!=0){
					c = fgetc(fp1);
					printf("ola(%d)",c);
					if(c!='\n'){
						c = fgetc(fp1);
						printf("ola(%d)",c);
						fseek(fp1,-1,SEEK_CUR);
						strcpy(aux,"");
						fscanf(fp1,"%s",aux);	
						tam = strlen(aux);
						printf("\n((Aad DAIS %s)))",aux);
						printf("\n((Aad DAIS %d)))", tam);
						printf("\n((Aad DAIS %s)))",aux);
						if(strcmp(aux,"endmacro")==0){
		 						printf("sim, eu achei");
								printf("sim, eu achei");
								printf("sim, eu achei");
								break;
						}
						else {
							printf("não achei");
							printf("não achei");printf("não achei");printf("não achei");printf("não achei");
						}
						printf("pq vc n sai");printf("pq vc n sai");printf("pq vc n sai");printf("pq vc n sai");
						printf("%ld, %d, %s",ftell(fp),c , aux);
						printf("pq vc n sai");printf("pq vc n sai");printf("pq vc n sai");printf("pq vc n sai");
						resultado = troca (fp, ftell(fp),c,aux,parametros);
						printf("pq vc n sai");
						if(resultado==1 && aux[0] && aux[0]!='('){
							printf("bazzing");printf("bazzing");printf("bazzing");
							fseek(fp,1,SEEK_CUR);
							resulaux = checaMacro(aux, nome, nome2, ftell(fp), ftell(fp1));
							resultado = 0;
						}
						printf("g");printf("g");printf("g");printf("g");
						if(resulaux!=0){
							printf("bankai");
							fseek(fp,resulaux,SEEK_SET);
							strcpy(aux,"");
							c='\0';
							while(c!='\n'){
								c = fgetc(fp1);
							}
							resulaux=0;
						}
						else{	printf("oi");
							c = fgetc(fp1);
							fprintf(fp,"%c",c);
						}
						fseek(fp1,-1,SEEK_CUR);
						
						printf("oi");
					}
				}
				printf("OLHA EU SAI");
				c='\0';
				while(c!='\n'){ /*retira o end macro*/
					fseek(fp,-2,SEEK_CUR);
					c = fgetc(fp);
				}
				break;
			}
		}while (!feof(fp1) || strcmp (aux,"section")!=0);
		c = '\0';
		while(c!='\n'||c!=EOF){ /*pula a linha que chamou a macro*/
			c = fgetc(fp1);
		}
		val = ftell(fp);
		fclose(fp1);
		fclose(fp);
		liberaMcr (parametros);
		return val;
	}
	else{
		printf("sa");
		return 0;
	}
}

void leAsm (char *nome,char *nomeExt){ /*recebe .asm para gerar .pre*/
	char palavra[50], b, c='a', label[50];
	int a ,valor, resposta, linha, result;
	list *equl = NULL;
	FILE *fp = fopen (nome, "r"); /*.asm*/
	if(!fp){
		printf("Erro ao abrir arquivo .asm");
		exit(1);
	}

	FILE *fp1 = fopen (nomeExt, "w+"); /*.pre*/

	if(!fp1){
		printf("Erro ao criar abrir arquivo .pre");
		exit(1);
	}
	

	while (c!=EOF){
		c= fgetc(fp);

		if(c!='\n' && c!= ' ' && c!= '\t'&& c!=EOF){
			fseek(fp, -1 ,SEEK_CUR);
			fscanf(fp, "%s", palavra);
			for(a = 0; palavra[a]; a++){
  				palavra[a] = tolower(palavra[a]);
			}
			
			if (strcmp (palavra,"equ")==0){ /*checa se palavra encontrada é diretiva EQU*/
				fseek(fp1, 0, SEEK_SET);
				while (c!='\n' && ftell(fp)>1){ /*volta para começo da linha*/
					fseek(fp, -2, SEEK_CUR);
					fprintf(fp1," ");
					c=fgetc(fp);
				}
				fseek(fp1, 0, SEEK_SET);
				if(ftell(fp)==1){
					fseek(fp,0,SEEK_SET);
				}
				else {
					fseek(fp1, 2, SEEK_CUR);
				}
				fscanf(fp, "%s", label);
				c=fgetc(fp);
				c=fgetc(fp);
				if(c!='\n'){
					fseek(fp,-2,SEEK_CUR);
				}
				fscanf(fp, "%s", palavra); /*pega equ de novo*/
				c=fgetc(fp);c=fgetc(fp);
				if(c!='\n'){
					fseek(fp,-2,SEEK_CUR);
				}
				fscanf(fp, "%d", &valor); /* pega valor*/
				if(c=='('){
					strcpy(palavra,"");
				}
				c=fgetc(fp);
				c=fgetc(fp);
				if(c!='\n'){
					fseek(fp,-2,SEEK_CUR);
				}
				for(a = 0; palavra[a]; a++){
  					palavra[a] = tolower(palavra[a]);
				}
				for(a = 0; palavra[a]; a++){
  					label[a] = tolower(label[a]);
				}


				result = checaIf (equl, label);
				if(result!=-1){
					while(c!='('){
						c=fgetc(fp);
					}
					c='\0';
					fscanf(fp,"%d", &linha);
					printf("\nERRO >> erro semantico detectado na linha: %d (Voce ja definiu algo com esse nome)", linha);
				}
				if(((label[0]>47 && label[0]<58)||(palavra[0]>47 && palavra[0]<58) || (strcmp(palavra,"equ")!=0))){
					c='\0';
					while(c!='\n'){
						c=fgetc(fp);
					}
					c='\0';
					while(c!='('){
						fseek(fp,-2,SEEK_CUR);
						c=fgetc(fp);
					}
					c='\0';
					fscanf(fp,"%d", &linha);
					printf("\nERRO >> erro sintático detectado na linha: %d (Erro na definição da diretiva EQU)", linha);
					strcpy(label,"\0");
					valor=0;

				}
				
				equl = add (equl, valor, label); /*adiciona em uma lista, os valores encontrados dos labels de equ*/
				
				c='\0';
				while (c!='\n' && c!=EOF){ /* pulando linha do equ*/
					c=fgetc(fp);
				}
				strcpy(palavra,"\0");
				strcpy(label,"\0");
				c='\0';
				fseek(fp,-1,SEEK_CUR);

			}
			if (strcmp (palavra,"if")==0){ /*checa se palavra encontrada é diretiva IF*/
				fscanf(fp, "%s", palavra);
				for(a = 0; palavra[a]; a++){
  					palavra[a] = tolower(palavra[a]);
				}
				if((palavra[0]>47 && palavra[0]<58)||palavra[0]=='\n'||palavra[0]=='\t'||palavra[0]==' '||palavra[0]=='('){
					c='\0';
					while(c!='\n'){
						c=fgetc(fp);
					}
					c='\0';
					while(c!='('){
						fseek(fp,-2,SEEK_CUR);
						c=fgetc(fp);
					}
					c='\0';
					fscanf(fp,"%d", &linha);
					printf("\nERRO >> erro sintático detectado na linha: %d (Erro na definição da diretiva IF)", linha);
				}
			
				resposta = checaIf (equl, palavra);
				while (c!='\n'&& c!=EOF){
						fseek(fp1, -2, SEEK_CUR);
						c=fgetc(fp1);
				}
				c=fgetc(fp);
				c = '\0';
				if (resposta == 0){ /*caso for 0, pula linha seguinte ao if*/
					while (c!='\n'){
						c=fgetc(fp);
					}
				}
				if(resposta == -1&&palavra[0]!='('){
					c='\0';
					while(c!='\n'){
						c=fgetc(fp);
					}
					c='\0';
					while(c!='('){
						fseek(fp,-2,SEEK_CUR);
						c=fgetc(fp);
					}
					c='\0';
					fscanf(fp,"%d", &linha);
					printf("\nERRO >> erro semantico detectado na linha: %d (Você não definiu nenhum parâmetro com esse nome)", linha);
				}
			strcpy(palavra,"\0");
			}
			for(a = 0; palavra[a]; a++){
  				palavra[a] = tolower(palavra[a]);
			}
			fprintf(fp1, "%s", palavra);
			c= fgetc(fp);
			fprintf(fp1, "%c", tolower(c));
			if (c=='\n' || c== '\t' || c == ' '){
				do{ /*pra ter certeza que vai encontrar o final do arquivo*/
					b = fgetc(fp);
				}while(c==b && b!=EOF);
				fseek(fp, -1 ,SEEK_CUR);

			} 
		}
		else{
			do{ /*pra ter certeza que vai encontrar o final do arquivo e também tirar alguns espaços, \t ou \n a mais*/
				b = fgetc(fp);
			}while(c==b);
			if(b==EOF || c==EOF)
				break;
			fseek(fp, -1 ,SEEK_CUR);
			fprintf(fp1, "%c", tolower(c));
		}
	}
	fclose(fp);
	fclose(fp1);
	libera(equl);
}

void lePre (char *nome,char *nomeExt){ /* le arquivo .pre e gera .mcr*/
	char pri[50], c='a', b='b';
	int begin=0, result=0, text=0;
	FILE *fp = fopen (nome, "r"); /*.pre*/

	if(!fp){
		printf("Erro ao abrir arquivo .pre");
		exit(1);
	}
	

	FILE *fp1 = fopen (nomeExt, "w+"); /*.mcr*/

	if(!fp1){
		printf("Erro ao criar abrir arquivo .mcr");
		exit(1);
	}
	
	while (!feof(fp) || c!=EOF){
		c='\0';
		fscanf(fp, "%s", pri);
		printf( "(((\n%s)))", pri);
		if ((strcmp(pri,"section")==0)&&begin==0) {
			begin=1;
			fscanf(fp, "%s", pri);
		
			if (strcmp(pri,"text")==0) {
				text=1;
				printf( ">>>>>>>>>>>\n%s", pri);
				fseek(fp,-5,SEEK_CUR);
				strcpy(pri,"");
			}
		}
		
		
		if (begin==1){
			c='\0';
			printf( "aqui chegando %d, %d", text, pri[0]);
			if(text==1 && pri[0]){
				printf("dasdadsa");
				printf("dasdadsa");
				printf("dasdadsa");
				printf( "\n>>>antes>>%s<<<", pri);
				
				result = checaMacro(pri, nome, nomeExt, ftell(fp1),ftell(fp));
				printf("\n\nsaiu");printf( "\ndepois>>>%s<<", pri);printf("\n\nsaiu");
			}
			printf( "\n%s(%d)", pri,result);
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
				text=0;
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
				fprintf(fp1,"\n%c",c);
		}
		do{
			b = fgetc(fp);
			printf("%d",b);
			getchar();
		}while(c==b);
		if(b==EOF || c==EOF)
			break;


	}
	fclose(fp);
	fclose(fp1);


}
void secaoDir (char *nome){
	char palavra[50],c;
	int text=0, data=0,a, linha;
	FILE *fp = fopen (nome, "r");
	if(!fp){
		printf("Erro ao abrir arquivo");
		exit(1);
	}
	while(!feof(fp) && c!=EOF){
		c=fgetc(fp);
		if(c!=EOF)
			fseek(fp,-1,SEEK_CUR);
		else
			break;
		c='\t';
		while(c=='\n'||c=='t'||c==' '){
			c=fgetc(fp);
		}
		if(c==EOF)
			break;
		fscanf(fp,"%s",palavra);
		for(a = 0; palavra[a]; a++){
  				palavra[a] = tolower(palavra[a]);
		}
		if(strcmp(palavra,"text")==0){ /*inicio da sessão de texto*/
			text = 1;
		}
		if(strcmp(palavra,"data")==0){ /*inicio da sessão de dados*/
			text = 0;
			data = 1;
		}
		if(strcmp(palavra,"equ")==0 && (text!=0 || data!=0)){

			while(c!='('){
				c=fgetc(fp);
			}
			c='\0';
			fscanf(fp,"%d", &linha);
			printf("\nERRO >> erro semantico detectado na linha: %d (Diretiva definida na sessão errada)", linha);
		}
		if(strcmp(palavra,"if")==0 && (text!=1)){

			while(c!='('){
				c=fgetc(fp);
			}
			c='\0';
			fscanf(fp,"%d", &linha);
			printf("\nERRO >> erro semantico detectado na linha: %d (Diretiva definida na sessão errada)", linha);
		}
		strcmp(palavra,"");

	}


	fclose(fp);
}

void limpa_linhas(char *nome_arquivo){
	FILE *fp;
	int i, j;
	char programa[216][159]; /*216 eh o numero maximo de palavras nesse assembly; cada linha tem no maximo 159 caracteres (50+50+50+6+3)*/
	char aux;
	fp = fopen(nome_arquivo, "r");
	if(!fp){
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		exit(1);
	}

	for(i=0; i<216 && !feof(fp); i++){

		fscanf(fp, "%[^\n(]", programa[i]); /*pega uma linha toda ate achar um \n ou um (*/
		int tam = strlen(programa[i]);
		programa[i][tam-1]='\0';
		do{
			if(feof(fp)) break;
			aux = fgetc(fp); 
		} while(aux != '\n'); /*esse loop procura por um \n*/
	}

	if(programa[i-1][0] == 0) i--; /*verifica se o primeiro caracter da ultima linha eh NULL. Se for, decrementa a quantidade de linhas.*/
	

	fclose(fp);
	fp = fopen(nome_arquivo, "w"); /*agora que eu ja tenho o programa todo num vetor, posso criar um novo arquivo com o mesmo nome, porem limpo*/
	if(!fp){
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		exit(1);
	}

	for(j=0; j<i; j++){
		fprintf(fp, "%s\n", programa[j]);
		strcpy(programa[j],"");
	}

	fclose(fp);
}


char* preprocessamento(char *nomeArquivoIN, char *nomeArquivoOUT){
	char *nomeArquivoOUT_util;
	if(nomeArquivoOUT == NULL) nomeArquivoOUT_util = "file.pre";
	else nomeArquivoOUT_util = nomeArquivoOUT;

	tiraComentario(nomeArquivoIN);
	colocaLinha(nomeArquivoIN);
	secaoDir(nomeArquivoIN);
	leAsm(nomeArquivoIN, nomeArquivoOUT_util);
	limpa_linhas(nomeArquivoIN);
	arrumaTopoFim(nomeArquivoOUT_util);
	if(nomeArquivoOUT != NULL) limpa_linhas(nomeArquivoOUT_util);

	return nomeArquivoOUT_util;
}

char* expansaoDeMacros(char *nomeArquivoIN, char *nomeArquivoOUT){
	char *nomeArquivoOUT_util;
	if(nomeArquivoOUT == NULL) nomeArquivoOUT_util = "file.mcr";
	else nomeArquivoOUT_util = nomeArquivoOUT;
printf("\ncheca");
	checaAntes(nomeArquivoIN);
printf("\nlepr");
	lePre(nomeArquivoIN, nomeArquivoOUT_util);
printf("\nlimp");
	limpa_linhas(nomeArquivoIN);
printf("\no==a");
	if(nomeArquivoOUT != NULL) limpa_linhas(nomeArquivoOUT_util);

	return nomeArquivoOUT_util;
}
