/*
* readline.c − readline command module
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "readline.h"


ssize_t readline( int fd , void * vptr , size_t maxlen) {
	ssize_t n , rc;
	char c , *ptr;
    
	ptr = vptr ;

	//le caracter a caracter e passa para string
	for (n = 1; n < maxlen; n++) {
		if ((rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return (0);  //pipe vazia
			else
				break;  //leu mas nao encontrou '/n'
		} else
			return(-1);  //erro na pipe
	}

     *ptr = 0;
     return (n) ;
}

