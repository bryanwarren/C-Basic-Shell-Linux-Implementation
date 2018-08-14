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

/*
 * Need to implement |, wc (word count), sort, <, >
 * Exit upon ctrl+Z twice (hold ctrl, use signals) (DONE) or "exit" (DONE)
 * Assume at most 3 piped commands...
 */


void runcommand(char* command, char** args) {
  //printf("\nCommand2: ");
  //printf(command);
  pid_t pid = fork();
  if(pid) { // parent
    	waitpid(pid, NULL, 0);
  } else { // child
    	execvp(command, args);
  }

}

int main(){
    char line[MAX_LINE_LENGTH];
    //printf("shell: "); 
    void exit_call(int sig);
    signal(SIGTSTP, exit_call);
    while(fgets(line, MAX_LINE_LENGTH, stdin)) {

	// Build the command and arguments, using execv conventions.
    	line[strlen(line)-1] = '\0'; // get rid of the new line
    	//char* command = NULL;
    	char* arguments[MAX_TOKEN_COUNT];
    	int argument_count = 0;

    	char* token = strtok(line, " ");
        char* last_cmd = token;  //must be a command
        char* filename;

        //check for < at beginning
	token = strtok(NULL," ");
        if(token=="<") {
            //read input from file
            //**WIP: must check to see whether missing open arg is needed...
            filename = strtok(NULL," ");
            int fd = open(filename,O_RDONLY);
            if(fd<0)
                printf("Error:  file %s does not exist\n", filename);
            dup2(fd,0);  //stdin becomes fd
            //execvp(...);
        }

        //now check for pipe
	
	/*if(token=="|") {
            //pipe stuff (1 of 3?)

        }*/


        if(token==">") {
            //read input from file
            //**WIP: must check to see whether missing open arg is needed...
            filename = strtok(NULL," ");
            int fd = open(filename,O_RDONLY);
            if(fd<0)
                printf("Error:  file %s does not exist\n", filename);
            dup2(fd,0);  //stdin becomes fd
            //execvp(...);
        }

        if (strcmp(last_cmd, "exit") == 0)
            exit(0);
        runcommand(last_cmd, arguments);
        

	//next token has been read
	//maybe set up char * last_cmd, since at least 2 cmds must be known for pipe/etc?
	//
	//
	//- load first command
	//- check for < x immediately after
	//  - if so, see above: link
	//- check for | next
	//  - handle...
	//  - either loop this or use three consecutive | checks.  (if > or NULL, continue immediately)
	//- check for >
	//  - if so, see above
	//- exec final command (?)
	//- profit
	//
	//
	//


	/*while(token) {  //command options are NOT PROVIDED; can ignore all of them...
      		if(!command) command = token;
      		arguments[argument_count] = token;
	      	argument_count++;
      		token = strtok(NULL, " ");
    	}
    	arguments[argument_count] = NULL;
	printf("\nCommand: ");
        printf(command);
	if(argument_count>0){
		if (strcmp(arguments[0], "exit") == 0)
            		exit(0);
    		runcommand(command, arguments);
	}*/
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

