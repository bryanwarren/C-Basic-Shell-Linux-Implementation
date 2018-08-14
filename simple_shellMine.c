#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_TOKEN_LENGTH 50
#define MAX_TOKEN_COUNT 100
#define MAX_LINE_LENGTH 512

// Simple implementation for Shell command
// Assume all arguments are seperated by space
// Erros are ignored when executing fgets(), fork(), and waitpid(). 

/**
 * Sample session
 *  shell: echo hello
 *   hello
 *   shell: ls
 *   Makefile  simple  simple_shell.c
 *   shell: exit
**/
void shift(char ** args, int index, int size){
  for(int i=index; i<size-1; i++){
    args[i]=args[i+1];
    printf ("hi%s",args[i]);
  }
  }

void input_output_redirect(char**args, int count){
  int i = count-1;
  int count2 = count;
  while(i > 0){
    if(strcmp(args[i],"<")==0) {
      printf("hi");
     int fd = open(args[i+1],O_RDONLY);
     //close(0);
     //if(fd<0)
       //printf("Error:  file %s does not exist\n", filename);
     //args[i]=NULL;
     //args[i+1]=NULL;
     shift(args, i, count);
     //shift(args, i+1, count);
     dup2(fd,0);
     close(fd);//stdin becomes fd
     //execvp(...);
    }
    if(strcmp(args[i],">")==0) {
      printf("yo");
      int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      //close(1);
      args[i]=NULL;
      //args[i+1]=NULL;
      //if(fd<0)
	//printf("Error:  file %s does not exist\n", filename);
       //args[i]=NULL;
      //shift(args, i, count);
       //shift(args, i+1, count);
      dup2(fd,1);
      close (fd);//stdin becomes fd
 
            //execvp(...);
    }
    i--;
  }
    
}

void runcommand(char* command, char** args, int count) {
  pid_t pid = fork();
  if(pid) { // parent
    	waitpid(pid, NULL, 0);
  }
  else { // child
    input_output_redirect(args, count);
    execvp(command, args);
  }
}

int main(){
    char line[MAX_LINE_LENGTH];
    void exit_call(int sig);
    signal(SIGTSTP, exit_call);
    //printf("shell: "); 
    while(fgets(line, MAX_LINE_LENGTH, stdin)) {
    	// Build the command and arguments, using execv conventions.
    	line[strlen(line)-1] = '\0'; // get rid of the new line
    	char* command = NULL;
    	char* arguments[MAX_TOKEN_COUNT];
    	int argument_count = 0;

    	char* token = strtok(line, " ");
    	while(token) {
      		if(!command) command = token;
      		arguments[argument_count] = token;
	      	argument_count++;
      		token = strtok(NULL, " ");
    	}
    	arguments[argument_count] = NULL;
	if(argument_count>0){
		if (strcmp(arguments[0], "exit") == 0)
            		exit(0);
    		runcommand(command, arguments, argument_count);
	}
        //printf("shell: "); 
    }
    return 0;
}

void exit_call(int sig) {
   static int count = 0;
   count++;
   //printf("\ntest\n");
   printf("Press ctrl+Z again to exit\n");
   if(count == 1)
	   signal(SIGTSTP, SIG_DFL);   
}
