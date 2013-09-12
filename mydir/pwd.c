#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

main(int argc, char *argv[])
{
	char *currDir=NULL;
	if(argc==1)
	{
		long size;
        	char *buf;
        	size = pathconf(".", _PC_PATH_MAX);
       		if ((buf = (char *)malloc((size_t)size)) != NULL)
               		currDir = getcwd(buf, (size_t)size);
		char *print=malloc(1024*sizeof(char));
                *print = '\0';
                strcat(print,currDir);
                strcat(print,"\n");
                write(STDOUT_FILENO,print,strlen(print));
	}
	else
	{
		char *print=malloc(200*sizeof(char));
                *print = '\0';
                strcat(print,argv[0]);
                strcat(print,": illegal parameter to command: pwd\n");
                write(STDERR_FILENO,print,strlen(print));
	}
}
