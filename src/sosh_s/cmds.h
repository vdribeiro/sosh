/*
* cmds.h − sosh command module
*/
#ifndef _CMDS_H
#define _CMDS_H

#include <stdio.h>
#include <stdlib.h>
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


/*
* DEFINES
*/

//tamanhos dos mallocs
#define MALLOCMAXSIZE 4096
#define MALLOCMINSIZE 256

/*
* VARIAVEIS GLOBAIS
*/

//flagQuit esta a 1 quando a resposta a saida é n e 0 quando é s
int flagQuit;

//array bidimensional para guardar os comandos
char histCmm[256][1024];

//contador para o numero de comandos
int count;

//arrai de pids e contador de pids
int pids[256];
int countPids;

/*
* FUNCOES
*/

//função que procura os comandos executados ultimamente, comecando por toFind
int cmm_search(char * toFind, int nrCmm);

//copia um determinado comando para o histórico de comandos
int copy2hist(char * line, int index);

//mostra o vector bidimensional do historico
int cmd_hist();

//em ssecondHalf fica a parte de string de from para a frente
char * splitString(char * str, char * secondHalf , int size, int from);

//função que lê uma linha e substitui o \n por \0
//fonte: http://www.daniweb.com
char *mygetline(char *line, int size);

//verifica a existencia de uma determinada dorectoria especificada pelo user
int verificaDir(char * dir);

//comando who com um processo independente
int cmd_quem();

//mostra os processos do utilizador
int cmd_psu();

//executa uma aplicacao presente em /usr/bin/
int cmd_usrbin();

//sai da shell
void cmd_exit();

//localiza a string toSearch apartir de directory
int cmd_localiza(char * directory, char * toSearch);

//versao da sosh
int cmd_ver();

//mostra os comandos presentes no sosh
int cmd_ajuda();


#endif

