/*
* example.c - readline example
*/

#include "readline.h"

void example () {
	int fp;
	int l;
	char buf [255];
	
	fp = open ("pipe", O_RDONLY);
	l = readline(fp, buf, 244);
	printf("read\n\t %s\n", buf);
	close(fp);
	return;
}

int main(int argc, char **argv)
{
	example();
	return 0;
}

