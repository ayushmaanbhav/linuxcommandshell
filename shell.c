#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<signal.h> 

int exitp=0;

char* trim(char *str)
{
        char *end;
        while(isspace(*str)) str++;
        if(*str == 0)
                return str;
        end = str + strlen(str) - 1;
        while(end > str && isspace(*end)) end--;
        *(end+1) = 0;
        return str;
}

void changeDir(char *param[50])
{
	if(param[1]==NULL)
	{
		char *homeDir = getenv("HOME");
		if(homeDir!=NULL)
		{
			param[1] = homeDir;
			param[2] = NULL;
		}
		else
		{
			char *print=malloc(200*sizeof(char));
                        *print = '\0';
                        strcat(print,"cd: failed to identify home dir\n");
                        write(STDERR_FILENO,print,strlen(print));
		}
	}
	if(param[2]==NULL)
	{
		int res = chdir(param[1]);
		if(res != 0)
		{
			char *print=malloc(200*sizeof(char));
                	*print = '\0';
                	strcat(print,"cd: No such file or directory\n");
                	write(STDERR_FILENO,print,strlen(print));
		}
	}
	else
	{
		char *print=malloc(200*sizeof(char));
                *print = '\0';
                strcat(print,param[1]);
                strcat(print,": illegal parameter to command: cd\n");
                write(STDERR_FILENO,print,strlen(print));
	}
}

void exitf(char *param[50])
{
	if(param[1]==NULL)
	{
		exitp=1;
	}
	else
        {
		char *print=malloc(200*sizeof(char));
                *print = '\0';
                strcat(print,param[1]);
                strcat(print,": illegal parameter to command: exit\n");
                write(STDERR_FILENO,print,strlen(print));
        }
}

void checkCommand(char *comm)
{
	int i=1,status;
	char *param[50];
	param[1]=NULL;
	comm = strtok(comm," \t");
	char *command=comm;
	param[0]=comm;
	comm = strtok(NULL," \t");
	while (comm != NULL)
        {
		param[i++]=comm;
		param[i]=NULL;
                comm = strtok (NULL," \t");
        }
	
	if(strcmp("cd",command)==0)
        {
		changeDir(param);
		return;
        }
	else if(strcmp("exit",command)==0)
	{
		exitf(param);
		return;
	}
	
	pid_t  pid;
	pid=fork();
	if(pid==-1)
	{
		char *print=malloc(200*sizeof(char));
                *print = '\0';
                strcat(print,command);
                strcat(print,": failed to create process\n");
                write(STDERR_FILENO,print,strlen(print));
		return;
	}
	else if(pid==0)
	{
		char *temp=malloc(sizeof(char)*1024);
		*temp = '\0';
		strcat(temp,"mydir/");
		strcat(temp,param[0]);
		if(execvp(temp,param)<0)
        	{
			temp = malloc(sizeof(char)*1024);
			*temp = '\0';
			strcat(temp,"/bin/");
                	strcat(temp,param[0]);
			if(execvp(temp,param)<0)
			{
				char *print=malloc(200*sizeof(char));
				*print = '\0';
               			strcat(print,command);
                		strcat(print,": command not found\n");
                		write(STDERR_FILENO,print,strlen(print));
			}
        	}
		exit(1);
	}
	else
	{
		waitpid(pid,&status,0);
	}
}

int setOutput(char *comm)
{
	if(comm==NULL||*comm==0)
                return -1;
	comm = trim(comm);
	if(comm==NULL||*comm==0)
		return -1;
	
	//PIPE

	int i=0;
        char *pp = strchr(comm,'|'),*file=pp;
        if (pp!=NULL)
               	i++;

        if(i>=1 && strlen(comm)>2)
        {
		char *filename=malloc(1024*sizeof(char));
	        strcpy(filename,file+1);
                comm = strtok(comm,"|");
		if(filename==NULL||comm==NULL||*filename==0||*comm==0)
                        return -1;
                comm = trim(comm);
                filename = trim(filename);
		if(filename==NULL||comm==NULL||*filename==0||*comm==0)
                        return -1;
		//printf("%s\n%s\n",filename,comm);
                if(*filename=='\0')
                        return -1;

		int pipefd[2];
  		int pid;

		//printf("h\n\n\n\n");
		int a=dup(STDIN_FILENO),b=dup(STDOUT_FILENO);
		//printf("a = %d\nb = %d\n",a,b);
  		pipe(pipefd);
	 	//printf("pfd0 = %d\npfd1 = %d\n",pipefd[0],pipefd[1]);
  		pid = fork();
		if (pid == -1)
		{
			return -1;
		}
  		if (pid == 0)
    		{
      			dup2(pipefd[1], STDOUT_FILENO);
			dup2(a,STDIN_FILENO);
      			//close(pipefd[0]);
			setOutput(comm);
			//printf("%d\n",res);
			exit(1);
    		}
  		else
    		{
			int status;
			wait(pid,&status,0);
			//printf("%d\n",status);
			//if(status<0)
				//return -1;
      			dup2(pipefd[0], STDIN_FILENO);
      			close(pipefd[1]);
			//dup2(b,STDOUT_FILENO);
			int res=setOutput(filename);
			dup2(b,STDOUT_FILENO);
			dup2(a,STDIN_FILENO);
    			return res;
		}
        }

	//INPUT FROM FILE

	i=0;
        pp = strchr(comm,'<');
	file=pp;
        while (pp!=NULL)
        {
                i++;
                //printf ("found at %d\n",pch-str+1);
		file=pp;
                pp = strchr(pp+1,'<');
        }

        if(i>1)
                return -1;
        if(i==1 && strlen(comm)>2)
        {
		char *filename=malloc(1024*sizeof(char));
		strcpy(filename,file+1);
		comm = strtok(comm,"<");
		if(filename==NULL||comm==NULL||*filename==0||*comm==0)
                        return -1;
		comm = trim(comm);
		filename = trim(filename);
		if(filename==NULL||comm==NULL||*filename==0||*comm==0)
                        return -1;
		if(*filename=='\0' || strchr(filename,' ')!=NULL || strchr(filename,'\t')!=NULL)
			return -1;
		int fd = open(filename, O_RDONLY);
		//printf("file: %s\n",filename);
		if(fd < 0)
		{
			char *print=malloc(200*sizeof(char));
                        *print = '\0';
                        strcat(print,"error: failed to open file\n");
                        write(STDERR_FILENO,print,strlen(print));
			return 0;
		}
		int stdi=dup(STDIN_FILENO);
		dup2(fd,STDIN_FILENO);
		checkCommand(comm);
		close(fd);
		dup2(stdi,STDIN_FILENO);
		return 0;
        }

	//REDIRECT TO FILE

        i=0;
        pp = strchr(comm,'>');
	file=pp;
        while (pp!=NULL)
        {
                i++;
                //printf ("found at %d\n",pch-str+1);
		file=pp;
                pp = strchr(pp+1,'>');
        }

        if(i>1)
                return -1;
        if(i==1 && strlen(comm)>2)
        {
		char *filename=malloc(1024*sizeof(char));
		strcpy(filename,file+1);
		comm = strtok(comm,">");
		if(filename==NULL||comm==NULL||*filename==0||*comm==0)
                        return -1;
		comm = trim(comm);
		filename = trim(filename);
		if(filename==NULL||comm==NULL||*filename==0||*comm==0)
                        return -1;
		if(*filename=='\0' || strchr(filename,' ')!=NULL || strchr(filename,'\t')!=NULL)
			return -1;
		int fd = open(filename, O_TRUNC | O_RDWR | O_CREAT , S_IROTH | S_IWOTH | S_IXOTH | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
		if(fd < 0)
		{
			char *print=malloc(200*sizeof(char));
                        *print = '\0';
                        strcat(print,"error: failed to open/create file\n");
                        write(STDERR_FILENO,print,strlen(print));
			return 0;
		}
		int stdo=dup(STDOUT_FILENO);
		dup2(fd,STDOUT_FILENO);
		checkCommand(comm);
		close(fd);
		dup2(stdo,STDOUT_FILENO);
		return 0;
        }

	checkCommand(comm);
	return 0;
}

void printPrompt(char *prompt)
{
	int i;
	char *currDir=NULL;
        long size;
        char *buf;
        size = pathconf(".", _PC_PATH_MAX);
        if ((buf = (char *)malloc((size_t)size)) != NULL)
                currDir = getcwd(buf, (size_t)size);

        for(i=strlen(currDir)-1;*(currDir+i)!='/';i--);

        strcat(prompt,"[CS09 ");
        strcat(prompt,(currDir+i+1));
        strcat(prompt,"]> ");
        write(STDOUT_FILENO,prompt,strlen(prompt));
}

void handle_signal(int signo)
{
	char *prompt=malloc(1024*sizeof(char));
        *prompt = '\n';
	*(prompt+1) = '\0';
	printPrompt(prompt);
}

main(int argc, char *argv[])
{
	signal(SIGINT, handle_signal);
	char *prompt=malloc(1024*sizeof(char));
        *prompt = '\0';
	int stdi,fd;
	if(argc > 2)
	{
		strcat(prompt,"error: illegal arguments\n");
                write(STDERR_FILENO,prompt,strlen(prompt));
		exit(0);
	}
	if(argc == 2)
	{
		fd = open(argv[1] , O_RDONLY );
		if(fd<0)
		{
			*prompt = '\0';
			strcat(prompt,"error: failed to open file\n");
                	write(STDERR_FILENO,prompt,strlen(prompt));
                	exit(0);
		}
                stdi=dup(STDIN_FILENO);
                dup2(fd,STDIN_FILENO);
	}
	
	int i;
	char *buff=malloc(1024*sizeof(char));	
	while(!exitp)
	{
		if(argc == 1)
		{
			*prompt = '\0';
			printPrompt(prompt);
		}
		i=0;
		int res=read(STDIN_FILENO,buff+i,1);
		if(res==0 && argc==2)
			break;
		while ( buff[i++] != '\n')
			read(STDIN_FILENO,buff+i,1);
		buff[i-1] = '\0';
		//fgets(buff,1024,stdin);
		
		if(argc == 2)
		{
			*prompt = '\0';
                        strcat(prompt,buff);
                        strcat(prompt,"\n");
                        write(STDOUT_FILENO,prompt,strlen(prompt));
		}
		
		buff = trim( buff );
		if(*buff == 0)
			continue;
		
		res=setOutput(buff);
		if(res==-1)
		{
			*prompt = '\0';
                        strcat(prompt,"error: failed to redirect, illegal arguments\n");
                        write(STDERR_FILENO,prompt,strlen(prompt));
		}
	}
	
	if(argc == 2)
	{
		close(fd);
                dup2(stdi,STDIN_FILENO);
	}
}
