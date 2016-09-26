#include <stdio.h>
#include <string.h>

void leAsm (char *nome){
	char palavra[50],c, b,nomeExt[200];
	int tam;
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
			printf("\n %s", palavra);
			fprintf(fp1, "%s", palavra);
			c= fgetc(fp);
			fprintf(fp1, "%c", c);
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
}

int main (){

	leAsm("triangulo.asm");
	printf("saiu!!");
return 0;
}
