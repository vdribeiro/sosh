#include "freq.h"


typedef struct caractFreq {
	char caract;
	int freq;
}lFreq;

typedef struct readCanalStr {
	int fd;
	int inc;
}readC;

int realtime;

lFreq freqlCresc[FREQSIZE];
lFreq oldFreqlCresc[FREQSIZE];

int verificaTop5(){

	
	int i=0;

	if(showMsg == 1)
		printf("\nVerificar de top5\n");
	
	for(i=FREQSIZE-1;i>FREQSIZE-5;i--){

		if(oldFreqlCresc[i].caract != freqlCresc[i].caract){
			if(showMsg == 1)
				printf("\nAlteração de top5 - %c - %c\n",oldFreqlCresc[i].caract,freqlCresc[i].caract);
			return 1; 
		}
	}	

	return 0;
}

int orderDesc(int top){

	int i=0,j=0,fpRes;
	lFreq aux;
	char * result, * resAux;
	result = malloc(MALLOCMAXSIZE);
	resAux = malloc(MALLOCMINSIZE);

	for(i=0;i<FREQSIZE;i++){
		freqlCresc[i].freq = freql[i];
		freqlCresc[i].caract = i+96;
	}

	for(i=0;i<FREQSIZE-1;i++){
		for(j=0;j<FREQSIZE-1;j++){

			if(freqlCresc[j+1].freq < freqlCresc[j].freq){
				aux = freqlCresc[j];
				freqlCresc[j] = freqlCresc[j+1];	
				freqlCresc[j+1] = aux;	
			}
		}
	}

	int topV=0;

	//Se ha uma alteracao no top5
	//enviar para a results
	if(top != 25)
		topV = verificaTop5();


	if(top == 25 || (topV == 1 && top == 6) ){

		
		for(i=0;i<FREQSIZE;i++){
			oldFreqlCresc[i].freq = freqlCresc[i].freq;
			oldFreqlCresc[i].caract = freqlCresc[i].caract;
		}

		for(i=FREQSIZE-1;i>FREQSIZE-top;i--)
		{

			sprintf(resAux, "[%c,%d] \n", freqlCresc[i].caract,freqlCresc[i].freq);
		
			strcat(result,resAux);
		
				
		}
		fpRes = open("/tmp/sosh.results", O_WRONLY);

		if (write(fpRes,result, strlen(result)) == -1){
			perror("writing to FIFO");
			exit(1);
		}
	}
	sleep(1000);
	close(fpRes);
	free(result);
	free(resAux);
	
	return 0;
}


int freq(char * string)
{

	int loop=0;
	int iascii=0;

	
	//converte string para lower case
	for(loop=0; loop<strlen(string); loop++)
		string[loop]=tolower(string[loop]);

	if(showMsg == 1)
		printf("\nlido da FIFO: '%s' \n", string);

	//escreve em freql
	for(loop=0; loop<strlen(string); loop++)
	{
		iascii = (int) string[loop];  //retira o codigo ascii do caracter (97 a 122)
		iascii = iascii-96;  //converte para indice

		if(iascii > 0 && iascii < FREQSIZE)	
			freql[iascii]+=1;  //o indice correspone a letra (25 no total)
	}

	return 0;
}

int makeResultsFIFO(){

	if (mkfifo("/tmp/sosh.results", 0660) == -1){
		remove("/tmp/sosh.results");

		if (mkfifo("/tmp/sosh.results", 0660) == -1){
	   		perror("creating FIFO");
	   		exit(1);
		}
	}
	return 0;
}

//thread para ler do cmd
void *readCmdTh() {
	
	int fpCmd;
	char * buf;
	buf = malloc(MALLOCMINSIZE);
	
	if(showMsg == 1)
		printf("\nAbrindo FIFO \"/tmp/sosh.cmd \"...\n");
	
	//abrir FIFO
	fpCmd = open("/tmp/sosh.cmd", O_RDONLY);	
	if(fpCmd == -1){
		perror("Error opening FIFO '/tmp/sosh.cmd'");
		exit(1);
	}
	
	if(showMsg == 1)
		printf("\nLendo da FIFO \"/tmp/sosh.cmd \"\n");
	
	//ler linha da FIFO
	int ret = read(fpCmd,buf, MALLOCMINSIZE);
	
	if(ret == -1){
		perror("Error reading FIFO '/tmp/sosh.cmd'");
		exit(1);
	}

	//printf("\nread cmd =  %s" ,buf);

	if(strcmp(buf,"ON")==0)
		realtime = 1;

	if(strcmp(buf,"OFF")==0)
		realtime = 0;
	
	if(strcmp(buf,"stats")==0){

		if(showMsg == 1)
			printf("\nActualizacao das frequencias\n");

		freq(buf);

		if(showMsg == 1)
			printf("\nEnviar resultados...\n");

		orderDesc(25);

		free(buf);
	}
	close(fpCmd);
	pthread_exit(NULL);// codigo de terminacao NULL (ou seja 0)!
}


//ler do cmd
int readCmd(){

	//sleep(1000);
	
	pthread_t threadId;	// criador guarda TIDs

	//criar thred para ler do cmd
	if (pthread_create(&threadId, NULL, readCmdTh,&threadId) != 0)
		exit(-1);	// em caso de erro, terminar!

	//esperar pela thread
	//pthread_join(threadId, NULL);

	return 0;

}
//thread para ler do canal
void *readCanalTh(void* fd){

	readC* structCanal = (readC*) fd;

	if(structCanal->inc != 0)
		pthread_mutex_lock(&Mutex);
	
	int l;
	char * buf;
	buf = malloc(MALLOCMAXSIZE);

	l = readline(structCanal->fd,buf, MALLOCMAXSIZE);

	if(l == -1){
		perror("Error reading FIFO '/tmp/sosh.canal'");
		exit(1);
	}
	
	//printf("read \n\t %s \n" ,buf);

	if(showMsg == 1)
		printf("\nActualizacao das frequencias\n");

	freq(buf);
	
	if(realtime == 1)
		orderDesc(6);
	
	pthread_mutex_unlock(&Mutex);	

	pthread_exit(NULL);// codigo de terminacao NULL (ou seja 0)!
}

//ler do canal
int readCanal(){

	pthread_t threadId;	// criador guarda TIDs
	int fpCanal;
	int incr=0;
	readC infoCanal;
	
	if(showMsg == 1)
		printf("Abrindo FIFO \"/tmp/sosh.canal \"...");

	fpCanal = open("/tmp/sosh.canal", O_RDONLY);
	infoCanal.fd = fpCanal;

	if(fpCanal == -1){
		perror("Error opening FIFO '/tmp/sosh.canal'");
		exit(1);
	}

	for(incr=0;incr<numeroThreads;incr++){
		//criar thred para ler do cmd
		infoCanal.inc = incr;
		if (pthread_create(&threadId, NULL, readCanalTh,(void *) &infoCanal) != 0)
			exit(-1);	// em caso de erro, terminar!

	}

	//close(fpCanal);

	return 0;
	
}

// REfs -> http://forum.codecall.net
int main (int argc, char **argv){
	
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	showMsg = 0;
	numeroThreads = 1;
	realtime = 0;

	int c;
	while ((c=getopt(argc, argv, "vt:")) != -1) {

		if(c == 'v'){
			printf("opcao -v \n");
			showMsg = 1;
			
		}
		if(c == 't'){
			numeroThreads = atoi((char*)optarg);
			printf("opcao -t com %d threads \n",numeroThreads);
			
		}		

	}
	
	makeResultsFIFO();
	
	int i;
	for(i=0; i<FREQSIZE;i++)
		freql[i]=0;
		
	while(1){
	
		readCmd();

		readCanal();

		//sleep(100);
	}

	return 0;
}

