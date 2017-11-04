
#include <fcntl.h>
#include <pthread.h>
#include "readline.h"
#include "cmds.h"

int stats;
int realtime=0;

//caminho onde começa o localiza
#define STARTDIR "/"

//estrutura usada para o handler do ctrl + c
static sigjmp_buf jmpbuf; 

int cmd_realtime(){

	if(realtime == 1){
		realtime = 0;
		printf("\nRealtime OFF\n");
	}
	else{
		realtime = 1;
		printf("\nRealtime ON %d\n",realtime);
	}
	
	int fd;

	fd= open("/tmp/sosh.cmd", O_WRONLY);

	if (fd== -1){
		perror("Opening FIFO for writing");
		exit(1);
	}

	if(realtime == 0){
		if (write(fd,"OFF", 3) == -1){
			perror("writing to FIFO");
			exit(1);
		}
	}
	else if(realtime == 1){
			if (write(fd,"ON", 2) == -1){
				perror("writing to FIFO");
				exit(1);
			}
		}
	

	close(fd);

	return 0;
}

//thread para ler do results
void *readResultsTh() {
	
	int fpResults;
	char * buf;
	buf = malloc(MALLOCMINSIZE);
	
	//abrir FIFO
	fpResults = open("/tmp/sosh.results", O_RDONLY);	
	if(fpResults == -1){
		perror("Error opening FIFO '/tmp/sosh.results'");
		exit(1);
	}
	
	//ler linha da FIFO
	int ret = read(fpResults,buf, MALLOCMINSIZE);
	
	if(ret == -1){
		perror("Error reading FIFO '/tmp/sosh.results'");
		exit(1);
	}
	//printf("\nNovo top5\n");
	printf("\n%s\n" ,buf);

	pthread_exit(NULL);// codigo de terminacao NULL (ou seja 0)!
}


//ler do results
int readResults(){

	pthread_t threadId;	// criador guarda TIDs

	if(stats == 0){
		//criar thred para ler do cmd
		if (pthread_create(&threadId, NULL, readResultsTh,&threadId) != 0)
			exit(-1);	// em caso de erro, terminar!
	}
	stats = 0;	
	
	//esperar pela thread
	//pthread_join(threadId, NULL);

	return 0;

}

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

//faz um pedido de estatisticas de ocorrencias de caracteres ao servidor freq
//mostra as estatisticas enviadas pelo servidor
int cmd_stats(){

	int fd;

	fd= open("/tmp/sosh.cmd", O_WRONLY);

	if (fd== -1){
		perror("Opening FIFO for writing");
		exit(1);
	}

	if (write(fd,"stats", 5) == -1){
		perror("writing to FIFO");
		exit(1);
	}

	close(fd);

	char * buf;
	buf = malloc(MALLOCMAXSIZE);


	int fpResults = open("/tmp/sosh.results", O_RDONLY);	
	if(fpResults == -1){
		perror("Error opening FIFO '/tmp/sosh.results'");
		exit(1);
	}
	
	int lResults = read(fpResults,buf,MALLOCMAXSIZE);

	if(lResults == -1){
		perror("Error reading FIFO '/tmp/sosh.results'");
		exit(1);
	}

	
	printf("\nESTATISTICA DE FREQUENCIAS\n%s" ,buf);


	free(buf);
	close(fpResults);
	
	stats=1;	

	return 0;
	

}

int putFIFO(char * command){

	int fd;
	if(stats == 0){
		switch(fork()){

			case -1:
				perror("Creating process");
				exit(1);

			case 0:
				fd= open("/tmp/sosh.canal", O_WRONLY);

			if (fd== -1){
				perror("Opening FIFO for writing");
				exit(1);
			}

			if (write(fd,command, strlen(command)) == -1){
				perror("writing to FIFO");
				exit(1);
			}

			exit(0);
		}
	}
	stats = 0;
	return 0;
}

void command(){

	while(1){	
		
		setbuf(stdout, NULL);
		setbuf(stdin, NULL);
		char * line = NULL;
		
		line = malloc(MALLOCMAXSIZE);

		mygetline(line,MALLOCMAXSIZE);

		if(flagQuit == 0 && strlen(line) > 0){	
			
			putFIFO(line);		
		
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
			else if(strcmp(line,"stats") == 0)
				cmd_stats();	
			else if(strcmp(line,"realtime") == 0)
				cmd_realtime();		
			else 
				printf("\n ''%s'' comando desconhecido. \n",line);
		
		}
		
		flagQuit = 0;	
		if(line)		
			free(line);	
			
		//printf("\ndentro\n");	
	}
}

int makeFIFOS(){
		
	if (mkfifo("/tmp/sosh.canal", 0660) == -1){
		remove("/tmp/sosh.canal");

		if (mkfifo("/tmp/sosh.canal", 0660) == -1){
	   		perror("creating FIFO");
	   		exit(1);
		}
	}

	if (mkfifo("/tmp/sosh.cmd", 0660) == -1){
		remove("/tmp/sosh.cmd");

		if (mkfifo("/tmp/sosh.cmd", 0660) == -1){
	   		perror("creating FIFO");
	   		exit(1);
		}
	}


	return 0;
}

int main (int argc, char **argv){

	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	flagQuit = 0;
	count = 0;
	countPids = 0;
	
	signal(SIGINT, handler);
	sigsetjmp(jmpbuf, 1);

	makeFIFOS();
	
	int pp[2]; //pipe: 0-le; 1-escreve
	int proc;

	stats = 0;

	//cria pipe
	if (pipe(pp) == -1)
	{
		perror("pipe()");
		//exit(1);
		return (-1);
	}
	
	//cria 2 processos
	if ((proc = fork()) == -1)
	{
		perror("fork()");
		//exit(2);
		return (-1);
	}
	
	if (proc == 0) {	//processo filho - executa o comando
		
		
		close(pp[0]); //fechar leitura no pipe
		dup2(pp[1],1);  //redirige a saída padrão para pp[1]
		
		//executa os comandos da sosh
		command();
		
		close(pp[1]);	  //fechar escrita no pipe
		
			
	}
	else	 //processo pai - comunicar com o named pipe
	{
	
		
		close(pp[1]); //fechar escrita no pipe
		dup2(pp[0],0);  //redirige a entrada padrão para pp[0]
		
		do {
		
		char * string = NULL;
		string = malloc(MALLOCMAXSIZE);	

		//le pipe
		if (read(pp[0], string, MALLOCMAXSIZE) == -1)
		{
			perror("read()");
			//exit(4);
			return (-1);
		}
		printf("%s",string);
		
		readResults();

		if(stats == 0){	
	
			int fd;
		
			switch(fork()){

				case -1:
					perror("Creating process");
					exit(1);

				case 0:
					//abrir a fifo para escrever a resposta
					fd = open("/tmp/sosh.canal", O_WRONLY);
	

					//escrever na FIFO
					if (write(fd,string, strlen(string)) == -1){
						perror("writing to FIFO '/tmp/sosh.canal'");
						exit(1);
					}

					//fechar FIFO	
					close(fd);

					exit(0);
			}
		}
		stats=0;
		
		int i=0;
		
		//limpar string que contem os outputs
		for (i=0; i<MALLOCMAXSIZE; i++) {
               		string[i] = '\0';
   		}

		free(string);
		
		} while (1);
		
		
		close(pp[0]);	//fechar leitura no pipe		
		//process[1] = getpid();
	}
	return 0;
}
