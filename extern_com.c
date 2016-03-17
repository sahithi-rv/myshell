#include "operate.h"

/*built in commands done by parent process hence we dont do fork*/


void append_job(pid_t pid,char * name)
{
	job * newnode;
	if(jno==0)
	{
				newnode=(job * )malloc(sizeof(job));
				newnode->pid=pid;
				strcpy(newnode->name,name);
				start=newnode;
				jobs=newnode;
	}
	else
	{
				newnode=(job * )malloc(sizeof(job));
				newnode->pid=pid;
				strcpy(newnode->name,name);
				jobs->next=newnode;
				jobs=jobs->next;
	}
	jno++;
}

int com_cd(char ** args)
{
	if(pos==1||!strcmp("~",args[1]))
	{
		chdir(pwd);
	}
	else if(chdir(args[1])!=0)
		perror("incorrect command");
	return 1;
}

// doesnt work correct for spaces in " "
int com_echo(char ** args)
{
	int l,i,j,t,p,fdo,fdapp,out_copy,app_copy;
	char a[1024];
	if(flag_out==1)
	{
		out_copy=dup(1); // makes a copy
		close(1);
		fdo=open(fdoutput,O_TRUNC|O_WRONLY|O_CREAT,0777);
		
	}
	if(flag_app==1)
	{
		app_copy=dup(1);
		close(1);
		fdapp=open(fdappend,O_WRONLY|O_APPEND|O_CREAT,0777);
		
	}

	// pos is number of arguments
	for(i=1;i<pos;i++)
	{
		t=0;p=0;
		l=strlen(args[i]);
		for(j=0;j<l;j++)
		{
			if(args[i][j]=='\"')
			{
				p=j-t;
				//printf("%d\n",p );
				t++;
			}
			else
			{
				a[j-t]=args[i][j];
			}
		}
		if(t%2==0)
		{
			//printf("sah %d\n",p);
			printf("%s",a);
		}
		else
		{
			for(j=0;j<p;j++)
				printf("%c",a[j]);
			printf("\"");
			for ( j=p; j<l-t; j++)
			{
				printf("%c",a[j]);
			}

		}
		printf(" ");
	}
	printf("\n");
	if(flag_out==1)
	{
		close(fdo);
		dup2(out_copy,1);
		close(out_copy);
	}
	if(flag_app)
	{
		close(fdapp);
		dup2(app_copy,1); // dup2 restores stdout 
		close(app_copy);
	}
	
	return 1;
}

int com_pinfo(char **args)
{
	FILE *f;
	char *line,*l1;int l,i;
	line=(char *)malloc(sizeof(char)*1024);
	l1=(char *) malloc(sizeof(char)*1024);
	char k[1024],p[1024];
	pid_t pi = getpid();
	if(pos==1)
	{
		sprintf(k,"/proc/%d",pi);
		printf("pid--%d\n", pi);
	}
	else if(pos==2)
	{
		sprintf(k,"/proc/%s/",args[1]);
			printf("pid--%s\n", args[1]);
	}
	else
	{
		printf("extra arguments\n");
		return 0;
	}

	// for state
	strcpy(p,k);
	strcat(p,"/status");

	f=fopen(p,"r");
	while(fgets(line ,1024,f)!=NULL)
	{
		if(strncmp(line,"State:",6)!=0)
			continue;
		else
		{
			l1=line+7;
			if(isspace(*line))
				++line;
			printf("Process Status-- %s\n",l1);
			break;
		}
	}

	// for virtual memory
	strcpy(p,k);
	strcat(p,"/statm");
	f=fopen(p,"r");
	printf("Virtual Memory-- ");
	while(fgets(line ,1024,f)!=NULL)
	{
		l=strlen(line);
		i=0;
		while(i<l&&line[i]!=' ')
		{

			printf("%c",line[i] );
			i++;
		}
		printf("\n");
		break;
	}

	// for executable path

	strcpy(p,k);
	strcat(p,"/smaps");
	f=fopen(p,"r");
	fgets(line,1024,f);
	l=strlen(line);
	printf("executable path -- ");
	i=0;
	while(i<l&&line[i]!='/'&&line[i]!='\n')
	{
		i++;
	}
	l1=line+i-1;
	printf("%s\n",l1);

	return 1;
}

int com_jobs(char ** args)
{
	
	job * node;
	node=start;
	while(node!=NULL)
	{
		printf("%d %s\n",node->pid,node->name );
		node=node->next;
	}
	return 1;
}

void removes(pid_t p)
{
	printf("%d\n",jno );

	job *node, *prenode;
	node=start;
	prenode=start;
	while(jno>0&&node->pid!=p)
		node=node->next;
	if(jno>0&&node==start)
	{
		start=NULL;
	}
	else
	{
		while(jno>0&&prenode->next!=node)
			prenode=prenode->next;
		prenode->next=prenode->next->next;
	}
		jno--;

}

int com_kjob(char ** args)
{
	int status;
	int k=atoi(args[1]),i=1;
	char s[100];
	
	if(!(k>=1&&k<=jno))
	{
		perror("invalid jno");
		return 1;
	}
	job *node=start;
	while(i!=k)
	{
		node=node->next;
		i++;
	}
	sprintf(s,"%d",node->pid);
	removes(node->pid);
	char *	arguments[]={"kill",s,args[2]};
	
	pid_t pid=fork();
	if(pid==0)
	{
		if(execvp(arguments[0],arguments)==-1)
		{
			perror("can't run child process");
			exit(EXIT_FAILURE);
		}
	}
	else if(pid < 0)
	{
		perror("cant fork");

	}
	else
	{
		do 
		{
     		waitpid(pid, &status, WUNTRACED);
   		} 
    	while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}
/* problem fg to bg and bg to fg simultaneously*/
int com_fg(char ** args)
{
	/*The setpgid function puts the process pid into the process group pgid*/
	int k=atoi(args[1]),status,i=1;
		//signal(SIGINT,SIG_IGN);
		signal(SIGINT,sigintHandler);
		//signal(SIGTSTP,SIG_IGN);
		signal(SIGTSTP,sigtstpHandler);
	
	if(k>=1 && k<=jno)
	{
		//struct termios termios_p;
		job *node=start;
		while(i!=k)
		{
			node=node->next;
			i++;
		}
		
			/*tcgetpgrp(1) gets the pid of terminal which is a foreground process.
			it returns the foreground process group id of stdout*/
			//setpgid(jobs[k-1].pid,tcgetpgrp(STDOUT_FILENO));
			// getsid(pid) returns the process' grpid

		tcsetpgrp(STDOUT_FILENO,node->pid);
		printf("process %d running as fg process\n",node->pid );
		removes(node->pid);
		//signal(SIGINT,SIG_IGN);
		signal(SIGINT,sigintHandler);
		//signal(SIGTSTP,SIG_IGN);
		signal(SIGTSTP,sigtstpHandler);
		//tcsetattr(STDOUT_FILENO,TCSADRAIN,&termios_p);
		do 
		{
     		waitpid(-1, &status, WUNTRACED|WCONTINUED);
   		} 
    	while (!WIFEXITED(status) && !WIFSIGNALED(status)&&WIFCONTINUED(status));
    	//tcgetattr(STDOUT_FILENO,&termios_p);
		//tcsetattr(STDOUT_FILENO,TCSADRAIN,&termios_p);

	}
	else
	{
		perror("invalid job number");

	}
		return 1;
}

int com_overkill(char ** args)
{
	job *node=start;
	while(node !=NULL)
	{
		kill(node->pid,9);
		node=node->next;
	}
	start=NULL;
	return 1;
}


int com_quit(char ** args)
{
	return 0;
}