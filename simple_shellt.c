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
  //printf("\nCommand: %s\n", command);
    pid_t pid = fork();
    if(pid) { // parent
      // printf("Parent, start\n");
        waitpid(pid, NULL, 0);
        //printf("Parent, done\n");
  } else { // child
      //	printf("Child, cmd=%s\n",command);
    	execvp(command, args);
  }
}

char * parse_args(char * last_cmd, char ** args, int *argnum, char * ltok) { //tok=last token read
    char * next_token;
    char * tok;
    //continue through input...stop if <>| encountered
    //can call this before OR AFTER >< encountered...(will need to)
    //if(!args)
    args[0]=last_cmd;
    
    //warning:  need to check for args before and after...
    int loop=1;
    if(!ltok || strcmp(ltok,"<")==0 || strcmp(ltok,">")==0 || strcmp(ltok,"|")==0) {
        loop=0;
	tok=ltok;
    }
    while(loop) {
        next_token = strtok(NULL, " ");
	tok = next_token;
	if(!next_token || strcmp(next_token,"<")==0 || strcmp(next_token,">")==0 || strcmp(next_token,"|")==0) {  //or null/EOF
	    if(!next_token) { //EOF
                tok = NULL;
	    } /*else {
    	        tok = next_token;
                printf("Assigning next_token\n");
	    }*/
	    loop=0;  //encountered <>|...keep going
        } else {
	  //printf("Getting argument %s\n",next_token);
            //get next argument
	    *argnum = *argnum + 1;
	    args[*argnum] = next_token;
        }
    }
    //*argnum = *argnum + 1;
    printf("argnum=%i\n",*argnum);
    args[*argnum+1]=NULL;

    return tok;
}

void reset_args(char ** args, int *argnum) {
    argnum = 0;
    args[0] = NULL;
}



int main(){
    char line[MAX_LINE_LENGTH];
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
        int argnum = 0;

        //*WIP:  ADD ARGUMENTS!
        token = parse_args(last_cmd, arguments, &argnum, token);

 	//check for < at beginning

	if(token && strcmp(token,"<")==0) {
            //read input from file
            //**WIP: must check to see whether missing open arg is needed...
            filename = strtok(NULL," "); //will be the filename
            int fd = open(filename,O_RDONLY);
            if(fd<0)
                printf("Error:  file %s does not exist\n", filename);
            dup2(fd,0);  //stdin becomes fd
            close(fd);
	    token = strtok(NULL," ");
	    token = parse_args(last_cmd, arguments, &argnum, token);
        }

	//exec here?
	//
        //now check for pipe
	
	/*if(token=="|") {
            //pipe stuff (1 of 3?)

        }*/

        if(token && strcmp(token,">")==0) {
            //read input from file
            //**WIP: must check to see whether missing open arg is needed...
            filename = strtok(NULL," ");
            int fd = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);  //CHANGE later?
            //if(fd<0)
            //    printf("Error:  file %s does not exist\n", filename);
            dup2(fd,1);  //stdin becomes fd
	    close(fd);
            //execvp(...);
        }

	//parse_args(last_cmd, arguments, &argnum, token);


        if (strcmp(last_cmd, "exit") == 0)
            exit(0);
        runcommand(last_cmd, arguments);
	//	dup2(1,1);
	//	dup2(0,0);

	//next token has been read
	//maybe set up char * last_cmd, since at least 2 cmds must be known for pipe/etc?
	//
	//
	//- load first command
	//- grab args until <>|N encountered
	//- check for < x immediately after
	//  - if so, see above: link
	//- continue grabbing args until <|N
	//- check for | next
	//  - handle...
	//  - grab more args until |<N
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
    if(count == 1) {
        printf("Press ctrl+Z again to exit\n");
        signal(SIGTSTP, SIG_DFL);
    }
}

