/*
Name: Philip Lewallen
Email: lewallep@onid.oregonstate.edu
Class: CS344-400
Assignment: Assignment3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include <sysexits.h>
#include <errno.h>
#include "myshell.h"

static void sigHandler(int sig);

/*	signal handler for the main command line of my shell.  This meets the criteria of the assignment to not have
	my shell exit when it encounters the SIGINT signal.
*/
static void sigHandler(int sig)
{
    if (sig == SIGINT) {
        return; 				/* Resume execution at point of interruption */
    }
    
    /* Must be SIGQUIT - print a message and terminate the process */
    printf("Caught %s - that's all folks!\n", strsignal(sig));  //This is working.
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {

	//Declare the pointer to the data structure to be passed around to the functions.  Allocate it's memory.
	struct commandInfo *userC = (struct commandInfo*)malloc(sizeof(struct commandInfo));
	char *fgetsFlag = "test";

	userC->sizeParsedComm = MAX_ARGS;
	
	parseCommInitalize(userC);

	getcwd(userC->homeDir, sizeof(userC->homeDir));

	while(fgetsFlag != NULL) {

		/* Establish same handler for SIGINT and SIGQUIT */
	    if (signal(SIGINT, sigHandler) == SIG_ERR)
	        perror("signal handler error");
	    if (signal(SIGQUIT, sigHandler) == SIG_ERR)
	        perror("signal handler error");

		printf(":");

		fgetsFlag = fgets(userC->input, sizeof(userC->input), stdin);

		if (fgetsFlag != 0) {

			smallshParseComm(userC);

		}
	}

	parseCommFree(userC);

	return 0;
}