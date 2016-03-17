#include "operate.h"
int pos=0;
int ex_coms=EXCOMS;
int sc=0,pipe_l=0;
int bg=0,flag_out=0,flag_inp=0,flag_app=0;
pid_t pid;
int job_no=0;
//storing external command list
char * ex_str[EXCOMS] ={"echo","cd","pinfo","jobs","kjob","fg","overkill","quit"};

char * fdoutput="/dev/stdout";
char * fdinput="/dev/stdin";
char * fdappend="/dev/stdout";

// storing eexternal command functions and their arguments
int (*ex_func[EXCOMS]) (char **) = {&com_echo,&com_cd,&com_pinfo,&com_jobs,&com_kjob,&com_fg,&com_overkill,&com_quit};

job *jobs;job *newnode;
job *start;



//child handler
void child_handler(int sig)
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    	if(WIFEXITED(status))
    	{
    		printf("%d exited normally\t",pid);
    		removes(pid);
    	}
}



char * readLine()
{
	char c;
	int buffer_size=RL_BUFFERSZ;
	char * buffer=(char *)malloc(sizeof(char)*buffer_size);
	int posi=0;

	while(1)
	{
		if (!buffer)
		{
			fprintf(stderr, "shell:Allocation error\n");
			exit(EXIT_FAILURE);
		}

		c=getchar();
		if (c==EOF || c=='\n')
		{
			buffer[posi]='\0';
			return buffer;
		}
		else
		{		
			buffer[posi]=c;
		}
		posi++;
		if (posi>=RL_BUFFERSZ)
		{
			buffer_size+=RL_BUFFERSZ;
			buffer=realloc(buffer,buffer_size);
			if (!buffer)
			{
				fprintf(stderr, "shell:Allocation error\n" );
				exit(EXIT_FAILURE);
			}
		}
	}
}

// breaking at ';'
char ** parseLine1(char * line)
{
	int bufsize=TOK_BUFFERSZ;
	char ** tokens= (char ** )malloc(sizeof(char *)*bufsize);
	char * token;

	if (!tokens)
	{
		fprintf(stderr, "memory Allocation error\n" );
		exit(EXIT_FAILURE);
	}
	token=strtok(line,SEMI_DELIM);
	sc =0;

	while(token!=NULL)
	{
		tokens[sc]=token;
		sc++;

		if(sc>=bufsize)
		{
			bufsize+=TOK_BUFFERSZ;
			tokens=realloc(tokens,bufsize);
		}
		if (!tokens)
		{
			fprintf(stderr, "memory Allocation error\n" );
			exit(EXIT_FAILURE);
		}
		if(token!=NULL)
		{
			token=strtok(NULL,SEMI_DELIM);
		}
	

	}
	tokens[sc]=NULL;
	return tokens;
}

char ** parseLine3(char * line)
{
	int bufsize=TOK_BUFFERSZ;
	char ** tokens= (char ** )malloc(sizeof(char *)*bufsize);
	char * token;

	if (!tokens)
	{
		fprintf(stderr, "memory Allocation error\n" );
		exit(EXIT_FAILURE);
	}
	token=strtok(line,PIPE_DELIM);
	pipe_l =0;

	while(token!=NULL)
	{
		tokens[pipe_l]=token;
		pipe_l++;

		if(pipe_l>=bufsize)
		{
			bufsize+=TOK_BUFFERSZ;
			tokens=realloc(tokens,bufsize);
		}
		if (!tokens)
		{
			fprintf(stderr, "memory Allocation error\n" );
			exit(EXIT_FAILURE);
		}
		if(token!=NULL)
		{
			token=strtok(NULL,PIPE_DELIM);
		}
	}
	tokens[pipe_l]=NULL;
	return tokens;

}

char ** parseLine2(char * line)
{
	pos =0;
	int bufsize=TOK_BUFFERSZ;
	char ** tokens= (char ** )malloc(sizeof(char *)*bufsize);
	char * token;
	if (!tokens)
	{
		fprintf(stderr, "memory Allocation error\n" );
		exit(EXIT_FAILURE);
	}
	token=strtok(line,TOK_DELIM);

	while(token!=NULL)
	{
		

		if(strcmp(token,"<")==0)
		{
			flag_inp=1;
			fdinput=strtok(NULL,TOK_DELIM);
			token=strtok(NULL,TOK_DELIM);
			continue;
		}
		if(strcmp(token,">>")==0)
		{
			flag_app=1;
			fdappend=strtok(NULL,TOK_DELIM);
			token=strtok(NULL,TOK_DELIM);
			continue;
		}
		if(strcmp(token,">")==0)
		{
			flag_out=1;
			fdoutput=strtok(NULL,TOK_DELIM);
			token=strtok(NULL,TOK_DELIM);
			continue;
		}
		if(strcmp(token,"&")==0)
		{
			bg=1;
			break;
		}
		else
			bg=0;
		tokens[pos]=token;
		pos++;

		if(pos>=bufsize)
		{
			bufsize+=TOK_BUFFERSZ;
			tokens=realloc(tokens,bufsize);
		}
		if (!tokens)
		{
			fprintf(stderr, "memory Allocation error\n" );
			exit(EXIT_FAILURE);
		}
		if(token!=NULL)
		{
			token=strtok(NULL,TOK_DELIM);
		}
	}
	tokens[pos]=NULL;
	return tokens;
}

int launch(char ** args) // for builtin internal commands....
{
	int status;
	int fdo,fdi,fdapp;
	pid=fork();
	//child process
	if(pid==0)
	{
		
		if(bg==0)
		 //maybe prints to standard out which is not on terminal now
		{	
				if(flag_out==1)
				{
					fdo=open(fdoutput,O_TRUNC|O_WRONLY|O_CREAT,0777);
					dup2(fdo,1);
					close(fdo);
				}
				if(flag_inp==1)
				{
					fdi=open(fdinput,O_RDONLY);
					dup2(fdi,0);
					close(fdi);
				}
				//printf("%s %d\n",fdappend,flag_app );
				if(flag_app==1)
				{
					fdapp=open(fdappend,O_WRONLY|O_APPEND|O_CREAT,0777);
					dup2(fdapp,1);
					close(fdapp);
				}

			if(execvp(args[0],args)==-1)
			{

				perror("can't run child process");
				exit(EXIT_FAILURE);
			}
			
			

		}
		else
		{	
			setpgid(0,0);
			execvp(args[0],args);
		}
	}
	else if(pid<0)
	{
		perror("cant fork");
	}

	else if(pid > 0)
	{
		if(bg==0)
		{
			do 
			{
     			waitpid(pid, &status,WCONTINUED|WUNTRACED);
    		} 
    		while (!WIFEXITED(status) && !WIFSIGNALED(status)&&!WIFCONTINUED(status));
    		flag_out=0;flag_inp=0;flag_app=0;
		}
		else
		{
			append_job(pid,args[0]);
			signal(SIGCHLD,child_handler);	
			signal(SIGHUP,SIG_IGN);
			printf("%d\n",pid);
		}
	}

	return 1;
}

int execute(char ** args)
{
	int i;
	if(args[0]==NULL) // empty command
	{
		return 1;
	}
	// built-ins
	for(i=0;i<ex_coms;i++)
	{
		if(strcmp(args[0],ex_str[i])==0)
		{
			return (*(*ex_func[i]))(args);
		}
	}
	return launch(args);
}

/*int execute_pipes(char ** args1, char ** args2)
{
	int status;
	pid_t pid1,pid2;
	int pipefd[2];

	//printf("asdf \n");

	pipe(pipefd);
	// first process
	// first process should duplicate the write end to stdout i.e pipefd[1] and close read end;
	pid1=fork();
	if(pid1==0)
	{
		dup2(pipefd[1],STDOUT_FILENO);
		close(pipefd[0]);
		//close(1);
		if(execvp(args1[0],args1)==-1)
		{
			perror("can't run child process");
			exit(EXIT_FAILURE);
		}
	}
	
	
	// second process
	// it should duplicate the read end to stdin and close write end;
	pid2=fork();
	if(pid2==0)
	{
		dup2(pipefd[0],STDIN_FILENO);
		close(pipefd[1]);
		//close(0);
		if(execvp(args2[0],args2)==-1)
		{
			perror("can't run child process");
			exit(EXIT_FAILURE);
		}
	}
	
	close(pipefd[0]);

   	close(pipefd[1]);
	do 
	{
   		waitpid(pid1, &status, WUNTRACED);
   	} 
  	while (!WIFEXITED(status) && !WIFSIGNALED(status));
	
	do 
	{
     	waitpid(pid2, &status, WUNTRACED);
   	} 
    while (!WIFEXITED(status) && !WIFSIGNALED(status));
	
	return 1;
}*/

int execute_pipes(char *** args)
{
	int fd_in=0;
	pid_t pid;
	int status,pipefd[2];
	int fdo,fdapp;
	
	if(flag_inp==1)
	{
		flag_inp=0;
		fd_in=open(fdinput,O_RDONLY);
		
	}
	while(*args!=NULL)
	{
		pipe(pipefd);
		
		pid=fork();
		if(pid==0)
		{
			dup2(fd_in,STDIN_FILENO);
			if(*(args+1) !=NULL )
				dup2(pipefd[1],STDOUT_FILENO);
			else
			{
				if(flag_out==1)
				{
					flag_out=0;
					fdo=open(fdoutput,O_TRUNC|O_WRONLY|O_CREAT,0777);
					dup2(fdo,1);
					close(fdo);
				}
			
				else if(flag_app==1)
				{
					flag_app=0;
					fdapp=open(fdappend,O_WRONLY|O_APPEND|O_CREAT,0777);
					dup2(fdapp,1);
					close(fdapp);
				}

			}
			close(pipefd[0]);
			
			if(execvp((*args)[0],*args)==-1)
			{
				perror("can't run child process");
				exit(EXIT_FAILURE);
			}
		}
		else if(pid<0)
		{
			{
		perror("cant fork");
			}

		}

		else
		{
			do 
			{
     			waitpid(pid, &status, WUNTRACED);
   			} 
    		while (!WIFEXITED(status) && !WIFSIGNALED(status));
    		close(pipefd[1]);
    		fd_in=pipefd[0];
    		args++;
		}
	}
	return 1;
}
