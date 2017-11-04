/*
* cmds.c − sosh command module
*/
#include "cmds.h"


//função que procura os comandos executados ultimamente, comecando por toFind
int cmm_search(char * toFind, int nrCmm){

	int i=nrCmm-1,j=0,k=0;	
	
	char *auxFind,*auxLine,*auxFind2;

	auxLine = malloc(MALLOCMAXSIZE);
	auxFind = malloc(MALLOCMAXSIZE);
	auxFind2 = malloc(MALLOCMAXSIZE);

	for(;k<strlen(auxFind);k++)
		auxFind[k]='\0';

	for(;k<strlen(auxFind2);k++)
		auxFind2[k]='\0';

	strcat(auxFind2," ");
	strcat(auxFind2,toFind);

	splitString(auxFind2,auxFind,MALLOCMAXSIZE,1);

	for(;i>=0;i--){

		for(;j<sizeof(histCmm[i]);j++){
			auxLine[j]=histCmm[i][j];
		}

		j=0;

		if(strstr(auxLine,auxFind) != NULL){
			printf("\nO ultimo comando usado foi: %s \n",auxLine);
			break;		
		}
		
	}	

	
	free(auxLine);
	free(auxFind);
	free(auxFind2);
	
	
	return 0;
}

//copia um determinado comando para o histórico de comandos
//char * line - contem o comando que ira ser copiado para a estrutura que suporta o historico
//int index - indice no array de historico
int copy2hist(char * line, int index){

	int i=0;

	//printf("\nindex %d - line %s\n",index,line);

	if(index < 256){	
	
		for(;i<strlen(line);i++){

			histCmm[index][i]=line[i];

		}
	}
	else 
		puts("\nMemoria de comandos cheia!\n");	

	return 0;
}

//mostra o vector bidimensional do historico
int cmd_hist(){

	int i = 0;

	puts("\n Historico de comandos usados");

	while(i <= (count-1)){
		
		printf("\n %d - %s \n",i+1,histCmm[i]);
		i++;
	}

	return 0;
}

//mostra os comandos presentes no sosh
int cmd_ajuda()
{
	printf("\n\nAJUDA SOSH\n\n");
	printf("quem - ");
	printf("Mostra o utilizador actual (equivalente ao 'who' em UNIX)\n");
	printf("psu - ");
	printf("lista os processos em execucao do utilizador actual\n");
	printf("ver - ");
	printf("Imprime a versao do sosh\n");
	printf("ajuda - ");
	printf("Lista os comandos suportados\n");
	printf("localiza <cmd> - ");
	printf("Imprime todas as ocorrencias de <cmd>, tanto ficheiros como directorios (equivalente ao 'locate' em UNIX).\n");
	printf("exit - ");
	printf("Sai da shell\n");
	printf("usrbin <exe> [<args>] - ");
	printf("Executa aplicaçoes,<exe>,localizadas em /usr/bin/ com numero variavel de argumentos, <args>\n");
	printf("hist - ");
	printf("Lista todos os comandos ja executados ordenadamente precedidos pelo o numero de ordem respectivo\n");
	printf("!# - ");
	printf("Executa o comando mais recente executado no sosh comecado pela string #\n\n");

    	return 0;
}

//em ssecondHalf fica a parte de string de from para a frente
//char * str - string a ser dividida
//char * secondHalf - ira conter a segunda parte da string
//int size - tamanho total da string
//int from - posicao apartir da qual ira ser dividida a string
char * splitString(char * str, char * secondHalf , int size, int from){

	int i=0,j=0;

	for(i=1;i<size;i++)
	{
		secondHalf[j] = str[i+from];		
		j++;
	}
	return secondHalf;
}

//sai da shell
void cmd_exit(){
	
	int i=0;
	
	while(i<countPids){
		kill(pids[i],0);
		printf("\nprocesso '%d' \n",pids[i]);
		i++;
	}
	
	//primeiro processo a ser criado
	kill(pids[0]-1,0);
	
	exit(0);	
}

//executa uma aplicacao presente em /usr/bin/ 
//"char * execute" contém o nome da aplicação e os argumentos
//char * path - caminho da aplicacao
int cmd_usrbin(char * execute,char * path){

	char * exec;
	char * fullpath;
	char * args;
	int result, status;

	exec = strtok(execute," ");
	
	args = malloc(MALLOCMAXSIZE);
	fullpath = malloc(MALLOCMINSIZE); 

	strcpy(fullpath,path);

	strncat(fullpath,exec,strlen(exec));
	
	splitString(execute,args,MALLOCMINSIZE,strlen(exec));

	result = fork();

	if (result == 0) {
		if(strlen(args) > 0)	
   			execl(fullpath,exec,args,NULL);
		else
			execl(fullpath,exec,NULL);

		pids[countPids]=getpid();
		countPids++;
	}
	else {
   		wait(&status);
	}

	free(args);
	free(fullpath);

	return 0;	
	
}

//mostra os processos do utilizador
int cmd_psu(){

	int result, status;
	result = fork();

	if (result == 0) {
		execlp("/bin/ps","ps",NULL); 
	}
	else {
		wait(&status);
	}	

	return 0;
}

//versao da sosh
int cmd_ver(){

	printf("\nsosh versão 0.3 \n");	

	return 0;
}

//devolve se um determinado caminho e um link ou nao
//const char *path - caminho do link
int isLnk (const char *path){

	struct stat stats;

	return stat (path, &stats) == 0 && S_ISLNK (stats.st_mode);
}

//devolve se um determinado caminho e um ficheiro ou nao
//const char *path - caminho do ficheiro
int isRegFile (const char *path){

	struct stat stats;

	return stat (path, &stats) == 0 && S_ISREG (stats.st_mode);
}

//devolve se um determinado caminho e um directorio ou nao
//const char *path - caminho do directorio
int isDir (const char *path){

	struct stat stats;

	return stat (path, &stats) == 0 && S_ISDIR (stats.st_mode);
}

//localiza a string toSearch apartir de directory
//char * directory - directory contem o directorio no qual irá ser procurado a string toSearch 
//char * toSearch - string que contem o noem a procurar
int cmd_localiza(char * directory, char * toSearch){

	DIR *dir;
	struct dirent *entry;

	char * dirName;
	char * fullPath;

	//printf(" \n dir: %s - 2Search: %s \n",directory,toSearch );

	fullPath = malloc(MALLOCMAXSIZE);
	dirName = malloc(MALLOCMINSIZE);	

	if((dir = opendir(directory))){
	
		//printf(directory);
		
		while((entry = readdir(dir)) != NULL){
		
			dirName = entry->d_name;

			if (strcmp(dirName,"..") == 0 || strcmp(dirName,".") == 0) 
				continue;
 			
 			strcpy(fullPath,directory);
 			strcat(fullPath,dirName);		

			if (access(fullPath,R_OK)!=0) 
				continue;	

			if(isDir(fullPath) != 0){
				
				if(strstr(dirName,toSearch) != NULL)
					printf("dir : %s \n",fullPath);

				cmd_localiza(strncat(fullPath,"/",1),toSearch);

			}
			else if(isRegFile(fullPath) != 0){

				if(strstr(dirName,toSearch) != NULL){
					printf("file : %s \n",fullPath);		
				}

			}
			else if(isLnk(fullPath) != 0){
			
				if(strstr(dirName,toSearch) != NULL)
					printf("lnk : %s \n",fullPath);
					
			}
		}
		

	}

	closedir(dir);
	

	return 1;
}

//comando who com um processo independente
int cmd_quem(){


	int result, status;
	result = fork();

	//dúvida se é sempre o mesmo caminho
	if (result == 0) {
   		execl("/usr/bin/who","who",NULL);
	}
	else {
   		wait(&status);
	}

	return 1;
}

//verifica a existencia de uma determinada directoria especificada pelo user
// char * dir - caminho especificado pelo utilizador
int verificaDir(char * dir){

	if(opendir(dir) == NULL){
		puts("directorio invalido");
		return 1;
	}
	else 
		return 0;
}



//função que lê uma linha e substitui o \n por \0
//char * line - ira conter o que utilizador escreveu
//tamanho da linha
//http://www.daniweb.com
char *mygetline(char *line, int size){

	if ( fgets(line, size, stdin) ){
		char *newline = strchr(line, '\n');

		if ( newline ){
			*newline = '\0';
		}
	}

	return line;
}
