

#include "cmds.h"

//caminho onde começa o localiza
#define STARTDIR "/"

//estrutura usada para o handler do ctrl + c
static sigjmp_buf jmpbuf; 

//handler para o ctrl + c
void handler(int sig) {

	flagQuit = 1;

	printf (" \nDeseja sair? (s/n) ");
	
	char c = getchar();
	
	if(c == 's' || c == 'S') {
	       exit(0);
	} 
	else if(c == 'n' || c == 'N'){			
		siglongjmp(jmpbuf, 1);
	}
	else{
		puts("\rresposta invalida\n");
		siglongjmp(jmpbuf, 1);	
	}
		
}

int main (int argc, char **argv){
	
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	flagQuit = 0;
	count = 0;
	countPids = 0;

	while(1){

		char * line = NULL;

		signal(SIGINT, handler);

		sigsetjmp(jmpbuf, 1);	
		
		line = malloc(MALLOCMAXSIZE);

		mygetline(line,MALLOCMAXSIZE);
	
		if(flagQuit == 0 && strlen(line) > 0){		

			if(strcmp(line,"quem") == 0){
				
				copy2hist(line,count);
			
				cmd_quem();
				
				count++;
			}
			
			else if(strcmp(strtok(line," "),"localiza") == 0){
				
				char * toSearch,*toHist,*startDir;

				toSearch = malloc(MALLOCMINSIZE);
				toHist = malloc(MALLOCMINSIZE);
				startDir = malloc(MALLOCMINSIZE);

				strcpy(toHist,line);

				splitString(line,toSearch,MALLOCMINSIZE,8);
				
				strcat(toHist," ");
				strcat(toHist,toSearch);

				if(strlen(toSearch)>=1){

					copy2hist(toHist,count);	
					
					count++;
					
					//verificar que é um directorio valido	
					while(verificaDir(startDir) != 0) {
						puts("\n Comecar a procura em: \n");	
						mygetline(startDir,MALLOCMAXSIZE);
					}

					printf("\n A procurar por '%s' , aguarde... \n", toSearch);
					puts("");

					cmd_localiza(startDir,toSearch);

					printf("\n Terminou a  procurar por '%s' \n", toSearch);
			
					//free(toSearch);
				}
				else
					puts("comando localiza devera ter argumentos");

				free(toSearch);
				free(toHist);
				free(startDir);
			}

			else if(strcmp(line,"psu") == 0){
				copy2hist(line,count);
			
				cmd_psu();
				
				count++;
			}

			else if(strcmp(line,"ver") == 0){
				copy2hist(line,count);
			
				cmd_ver();
				
				count++;
			}
			else if(strcmp(line,"ajuda") == 0){
			
				copy2hist(line,count);
				
				cmd_ajuda();
				
				count++;
				 
			}
			else if(strcmp(line,"exit") == 0)
				cmd_exit();
				
			else if(strcmp(strtok(line," "),"usrbin") == 0){
			
				char * toExecute,*toHist;
				
				toExecute = malloc(MALLOCMINSIZE);
				toHist = malloc(MALLOCMINSIZE);

				strcpy(toHist,line);
				
				splitString(line,toExecute,MALLOCMINSIZE,6);

				strcat(toHist," ");
				strcat(toHist,toExecute);

				if(strlen(toExecute)>=1){
				
					copy2hist(toHist,count);

					printf("\nExecutar '/usr/bin/%s' \n", toExecute);
					puts("");
				
					cmd_usrbin(toExecute,"/usr/bin/");
				
					free(toExecute);
				
					count++;
				}
				else
					puts("comando usrbin devera ter argumentos");	

			}
			else if(strcmp(line,"hist") == 0)
				cmd_hist();		

			else if(line[0] == '!'){
				
				cmm_search(line,count);
			}
			else 
				printf("\n ''%s'' comando desconhecido. \n",line);
		
		}
		
		flagQuit = 0;	
		
		if(line)		
			free(line);	
	}

	return 0;
}
