#ifndef operate_h
#define operarate_h

#include <stdlib.h>		// also has execvp , exit
#include <sys/wait.h> // waitpid and associated macros
#include <unistd.h> // for fork() , exec() , pid_t
#include <stdio.h>  //aslo has getchar() , perror , stderror ... 
#include <string.h>  // has strtok..
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <termios.h>

#define RL_BUFFERSZ 1024 /// buffer for read line
#define TOK_BUFFERSZ 64 // buffer size for token
#define TOK_DELIM " \r\t\a\n" // token delimitors
#define SEMI_DELIM ";" // semicolon delimitor
#define PIPE_DELIM "|" // pipe delimitor
#define PIPE_MAX 1000 // maximum pipes per command
#define EXCOMS 8
#define MAXJOBS 10

char * readLine();
char ** parseLine1(char * line);
char ** parseLine3(char * line);
char ** parseLine2(char * line);
int launch(char ** args);
int execute(char ** args);
int execute_pipes(char *** args);
void removes(pid_t);
void append_job(pid_t,char *);	
void sigtstpHandler(int sig_num);
void sigintHandler(int sig_num);
void child_handler(int );
//built in commands
int com_cd(char ** args);
int com_echo(char **args);
int com_pinfo(char **args);
int com_jobs(char ** args);
int com_kjob(char ** args);
int com_fg(char ** args);
int com_overkill(char ** args);
int com_quit(char ** args);



//number of external commands
extern int ex_coms;
extern int pos; // number of arguments for ; seperated command
extern int sc; // number of ;
extern int pipe_l; // number of pipe seperated commands;
extern int bg; // flag for background process
//extern int no_of_scs; 
extern char pwd[1024]; 
extern pid_t pid; // present process pid
//storing external command list
extern char * ex_str[EXCOMS];
extern char * fdoutput;
extern char * fdinput ; 
extern char * fdappend;
extern int job_no; // number of jobs
extern int jno;
extern int flag_ctrlz;
extern int flag_out,flag_inp,flag_app;
// storing eexternal command functions and their arguments
extern int (*ex_func[EXCOMS]) (char **);

// phase 2
typedef struct job
{
	pid_t pid;
	char  name[10000];
	struct job * next;	
}job;

extern job* jobs;
extern job* start;
#endif
