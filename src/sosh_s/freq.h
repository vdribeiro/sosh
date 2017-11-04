
#ifndef _CMDS_H
#define _CMDS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


#include "readline.h"

//tamanhos dos mallocs
#define MALLOCMAXSIZE 1024
#define MALLOCMINSIZE 256
#define FREQSIZE 25

/*
* VARIAVEIS GLOBAIS
*/

//numero de threads que vao ser usadas para ler do FIFO canal
int numeroThreads;

//array com o numero de frequencia das letras
int freql[FREQSIZE];

//Variavl o controlo do aparecimento ou nao das mensagens no ecra -> 1 - aparecer, 0 - nao aparecer
int showMsg;

//Mutex para gerir as threads que lem do canal
pthread_mutex_t Mutex;


/*
* FUNCOES
*/

//calcula e imprime a lista de frequencias das letras
int freq(char * string);

#endif

