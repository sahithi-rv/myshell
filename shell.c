#include <stdio.h>
#include <unistd.h>
#include "operate.h"

int jno=0;
char pwd[1024];
int flag_ctrlz;
void sigintHandler(int sig_num)
{
	signal(SIGINT,sigintHandler);
	fflush(stdout);
}

void sigtstpHandler(int sig_num)
{
	{		
		setpgid(pid,0);
		kill(pid,SIGCONT);
		signal(SIGTSTP,sigtstpHandler);
		append_job(pid,"");
		printf("%d running in background\n",pid	 );
	}
}


int shellLoop()
{
	char * line;// just read line
	char ** arguments; // semicolon seperated arguments
	char ** argues; // tab seperated arguments for each ;
	char ** args_pipes[PIPE_MAX];
	int status,l,l1,j,i;
	char cwd[1024],path[1024];

	//where the folder is
	getcwd(pwd,sizeof(pwd));
	l=strlen(pwd);
	do
	{
		//where the prompt is...
		getcwd(cwd,sizeof(cwd));
		
		if(strncmp(cwd,pwd,l)==0)
		{
			l1=strlen(cwd);
			path[0]='~';
			path[1]='/';
			for(i=l+1,j=2;i<l1;i++,j++)
				path[j]=cwd[i];
			path[j++]='\0';
		}
		else
		{
			strcpy(path,cwd);
		}
		//prompt
		printf("%s@UBUNTU:%s:",(getlogin()),path);
		line=readLine();
		arguments=parseLine1(line);
		
		i=0;
		// sc - count of ';'
		while(sc--)
		{
			argues=parseLine3(arguments[i]);

			for(j=0;j<pipe_l;j++)
			{
				args_pipes[j]=parseLine2(argues[j]);
			}
			if(pipe_l==1)
			{
				status=execute(args_pipes[0]);
			}
			else
			{
				//for(j=1;j<pipe_l;j++)
				status=execute_pipes(args_pipes);
				
			}
			fdoutput ="/dev/stdout";
			fdinput= "/dev/stdin";
			fdappend= "/dev/stdout";
			i++;
		}
		sc=0;		
		pos=0;
	
		free(line);
		free(arguments);
		

	}while(status);
	return 0;
}

int main(int argc,char ** argv)
{
	/*sigtstp vs sigstop : we cant ignore sigstop .. same functionality but sigtstp is used for ^Z*/
	//if(pid!=0)
	{
		signal(SIGCHLD,child_handler);	
		signal(SIGHUP,SIG_IGN);
		signal(SIGINT,SIG_IGN);
		signal(SIGINT,sigintHandler);
		//signal(SIGTSTP,SIG_IGN);
		signal(SIGTSTP,sigtstpHandler);
	}
	shellLoop();

	return EXIT_SUCCESS;
}
