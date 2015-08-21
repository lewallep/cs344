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

//Here I catch and display which signal is caught.
void catchIntSignal(int sig) {
	
	printf("Caught %s\n", strsignal(sig));

	return;
}


//Allocates the memory for an aray of pointers to arrays of chars which will
//make up my command line arguments.
void parseCommInitalize(struct commandInfo *userC) {

	assert(userC->parsedComm != NULL);

	int i = 0;

	for (i = 0; i < userC->sizeParsedComm; i++) {
		userC->parsedComm[i] = (char*)malloc(sizeof(char) * MAX_ARG_CHARS);
		userC->commArgs[i] = (char*)malloc(sizeof(char) * MAX_ARG_CHARS);
	}

	i = 0;

	return;
}

//Simply frees the allocated memory the program has allocated during its run.
void parseCommFree(struct commandInfo *userC) {
	assert(userC->parsedComm != NULL);
	int i = 0;

	for (i = 0; i < userC->sizeParsedComm; i++) {
		free(userC->parsedComm[i]); 
		free(userC->commArgs[i]);
	}

	free(userC->parsedComm);

	return;
}

//Parses the command line into its component parts.  
void smallshParseComm(struct commandInfo *userC) {

	//declaring local variables
	int i = 0;		//Basic counter.
	char *token = NULL;
	int argLength = 0;
	int exitStatus = -5;
	char *arrPlaceholder = NULL;

	//Getting the first token from the command line.
	token = strtok(userC->input, " \n");
	//Verifying the token string is not null before performing strcmp.
	if (token != NULL) 
	{
		strcpy(userC->parsedComm[i], token);
	}

	//Looping through the rest of the input string copying out the individual tokens.
	// http://www.cplusplus.com/reference/cstring/strtok/
	while (token != NULL) 
	{	
		if (token != NULL) 
		{	
			strcpy(userC->parsedComm[i], token);
			token = strtok(NULL, " \n");
		}
		i++;
	}

	userC->numCommands = i;							//Transfering the local countet of arguments to the commandInfo struct.
	argLength = strlen(userC->parsedComm[0]);		//checking for an empty line.

	arrPlaceholder = userC->parsedComm[userC->numCommands];		//making a pointer to record where the array is.

	userC->parsedComm[userC->numCommands] = NULL; 	//Preparing the parsed array for execvp().
		
	i = 0;		//Reset the generic counter.

	/*	Each of the below if statements is used to determine which command has been entered on the command line.
		I use char array constants to compare the built-in commands to what was entered.
		I also check the amount of arguments and the length of the first argument to determine if the line is 
		blank or not.
	*/
	if (userC->parsedComm[0] == NULL) 
	{
		
	}
	else if (COMM_HASH == userC->parsedComm[0][0]) 
	{
		
	}	
	else if (argLength == 0) 
	{
		return;
	}
	else if (strcmp(userC->parsedComm[0], COMM_EXIT) == 0) 
	{
		myExit();
	}
	else if (strcmp(userC->parsedComm[0], COMM_CD) == 0) 
	{
		exitStatus = myCd(userC);
	}
	else if (strcmp(userC->parsedComm[0], COMM_STATUS) == 0) 
	{
		myStatus(userC);
	}
	//This checks for the redirect in from a file.
	else if (userC->numCommands > 1 && strcmp(userC->parsedComm[1], "<") == 0)
	{
		exitStatus = sysCommandIn(userC);

		if (exitStatus !=0) {
			userC->exitStatus = -1;
		}
	}
	//Checks to ensure we have more than a single argument, then it checks for the ">" character to redirect out
	//to a file.
	else if (userC->numCommands > 1 && strcmp(userC->parsedComm[1], ">") == 0)
	{
		sysCommandOut(userC);
	}
	else if (userC->numCommands > 1 && strcmp(userC->parsedComm[userC->numCommands -1], "&") == 0) 
	{
		exitStatus = sysCommandBG(userC);
		printf("exit status = %d\n", exitStatus);

		if (exitStatus != 0) {
			userC->exitStatus = -1;
		}

	}
	else 
	{
		exitStatus = sysCommand(userC);

		if (exitStatus != 0) {
			userC->exitStatus = -1;
		}
	}

	//resetting my pointer away from the null back to the allocated memory for the next 
	//user command line.
	userC->parsedComm[userC->numCommands] = arrPlaceholder;
	userC->exitStatus = exitStatus;		//be alert about this line.  I am not sure if it is breaking anything.

	return;
}

//The exit function we were specified to write.
void myExit() 
{

	exit(0);

	return;
}

//This function changes the directory if the user calls it. 
int myCd(struct commandInfo *userC) 
{
	//char *homePath = getenv("HOME");
	int dirChange;  	//this variable returns the success or failure of my chdir() function call.

	//If only cd is enterd by itself the user is sent to their home directory.
	if (userC->numCommands == 1) {
		dirChange = chdir(userC->homeDir);
	}
	else {
		dirChange = chdir(userC->parsedComm[1]);

		//Here I pipe out to the parent processs any errors which occur during the directory change.
		int r, pipeFDs[2];
		pid_t spawnpid;

		if (pipe(pipeFDs) == -1)
		{
			perror("Hull Breach!");
			exit(1);
		}

		spawnpid = fork();

		switch (spawnpid)
		{
			case 0: // Child
				close(pipeFDs[0]); // close the input file descriptor
				write(pipeFDs[1], &dirChange, sizeof(dirChange));
				_exit(0);
			default: // parent
				close(pipeFDs[1]); // close output file descriptor
				r = read(pipeFDs[0], &userC->exitStatus, sizeof(userC->exitStatus));
		}

		if (dirChange != 0) {
			perror("There was a problem with your change directory\n");
		}
	}

	return dirChange;
}

//This function prints out to standard output the exit status of the last command.
void myStatus(struct commandInfo *userC) 
{
	if (userC->exitStatus > 1) 
	{
		userC->exitStatus = 1;
	}

	printf("%d\n", userC->exitStatus);

	return;
}

//This function opens up the system command the user has specified.  
int sysCommand(struct commandInfo *userC) 
{	//Stackoverflow
	//http://stackoverflow.com/questions/1584956/how-to-handle-execvp-errors-after-fork
    int pipefds[2];
    int count, err;
    pid_t child;

    struct sigaction act;
	act.sa_handler = catchIntSignal;
	act.sa_flags = 0;
	sigfillset(&(act.sa_mask));
	sigaction(SIGINT, &act, NULL);

	//Check for any error with the pipes.
    if (pipe(pipefds)) {
        perror("pipe");
        return EX_OSERR;
    }
    if (fcntl(pipefds[1], F_SETFD, fcntl(pipefds[1], F_GETFD) | FD_CLOEXEC)) {
        perror("fcntl");
        return EX_OSERR;
    }

    /*	Switch the function based depending on which process the function is being called in.
    	The switch statemnt pipes any errors from the child to the parent and returns the err message
    	to the calling function.
	*/
    switch (child = fork()) {
    case -1:
        perror("fork");
        return EX_OSERR;
    case 0:
        close(pipefds[0]);
        err = execvp(userC->parsedComm[0], &userC->parsedComm[0]);
        write(pipefds[1], &errno, sizeof(int));
        _exit(0);
    default:
        close(pipefds[1]);
        while ((count = read(pipefds[0], &err, sizeof(errno))) == -1)
            if (errno != EAGAIN && errno != EINTR) break;
        if (count) {
            fprintf(stderr, "Last command: %s\n", strerror(err));
            return EX_UNAVAILABLE;
        }
        close(pipefds[0]);
        //puts("waiting for child...");
        while (waitpid(child, &err, 0) == -1)
            if (errno != EINTR) {
                perror("waitpid");
                return EX_SOFTWARE;
            }
    }		//End stackoverflow code.
    
    return err;
};

/*	Function calls the system commands specified by the user.  It redirects the output from the stdout to 
	the file specified.  If the file does not exist it is created, truncated with the permissions 0666 which 
	equate to read write access for everyone.
*/
void sysCommandOut(struct commandInfo *userC) {

	pid_t spawnpid;
	int fd1, fd2;

	spawnpid = fork();

	switch (spawnpid)
	{
		case 0: // Child
			/*  The below code is from lecture 12 Pipes and IPC.
				Modified to work with my data structures.
			*/
			fd1 = open(userC->parsedComm[userC->numCommands - 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (fd1 == -1)
			{
				perror("open");
				_exit(1);
			}

			fd2 = dup2(fd1, 1);			//duplicating my file descriptors.

			if (fd2 == -1)			//Checking to make sure there wasn't an error duplicating the file descriptors.
			{
				perror("dup2");
				_exit(1);
			}

			//Calling my command.
			execlp(userC->parsedComm[0], userC->parsedComm[userC->numCommands - 1], NULL);	
			perror("exec");

			_exit(0);
		default: // parent
	
			break;
	}

	return;
}

/*	Function is the same as sysCommandOut excpet it reads in from a file and outputs the contents to the 
	stdout.  As this code is similar to sysCommandOut it was also referenced from StackOverflow.  The link is below.
	//http://stackoverflow.com/questions/1584956/how-to-handle-execvp-errors-after-fork
*/
int sysCommandIn(struct commandInfo *userC) {

    int pipefds[2];
	pid_t spawnpid;
	int err, count;

    if (pipe(pipefds)) {
        perror("pipe");
        return EX_OSERR;
    }
    if (fcntl(pipefds[1], F_SETFD, fcntl(pipefds[1], F_GETFD) | FD_CLOEXEC)) {
        perror("fcntl");
        return EX_OSERR;
    }

	spawnpid = fork();

	switch (spawnpid)
	{
		case -1: 
			perror("fork");
        	return EX_OSERR;
		case 0:	//child
			close(pipefds[0]);
			err = execlp(userC->parsedComm[0], userC->parsedComm[1], userC->parsedComm[2], NULL);
			write(pipefds[1], &errno, sizeof(int));
		
			_exit(0);
		default:	//Parent process wc
			
			close(pipefds[1]);
			while ((count = read(pipefds[0], &err, sizeof(errno))) == -1)
            	if (errno != EAGAIN && errno != EINTR) break;
        	if (count) {
        		fprintf(stderr, "Last command: %s\n", strerror(err));
       	  		return EX_UNAVAILABLE;
      	  	}
      	  	close(pipefds[0]);
    	    
    	    while (waitpid(spawnpid, &err, 0) == -1)
	            if (errno != EINTR) {
	                perror("waitpid");
	                return EX_SOFTWARE;
	            }
	}
	return err;
}

/*	Calls a system function and runs it in the background.  It redirects the output from the stdout to the
	file /dev/null
*/
int sysCommandBG(struct commandInfo *userC) {

	pid_t spawnpid;
	int err;
	int fd1, fd2;
	char *tempPC;

	printf("Entering a background process = %s\n", userC->parsedComm[0]);
	spawnpid = fork();

	switch (spawnpid)
	{
		case -1:		//This case is entered if there was an error with the fork() function.
			perror("fork");
			return EX_OSERR;
		case 0:	//in the child process.
	
			fd1 = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (fd1 == -1)
			{
				perror("open");
				_exit(1);
			}

			fd2 = dup2(fd1, 1);			//duplicating my file descriptors.

			if (fd2 == -1)			//Checking to make sure there wasn't an error duplicating the file descriptors.
			{
				perror("dup2");
				_exit(1);
			}

			tempPC = userC->parsedComm[2];
			userC->parsedComm[2] = NULL;
			err = execvp(userC->parsedComm[0], &userC->parsedComm[0]);

			userC->parsedComm[2] = tempPC;

			if (err != 0 ) {
				_exit(1);		
			}

		default:		//in the parent process funciton.
    	    while (waitpid(spawnpid, &err, WNOHANG) == -1)
	            if (errno != EINTR) {
	            	printf("in parent err = %d\n", err);
	                perror("waitpid");
	                return EX_SOFTWARE;
	            }
	       printf("bg ps is = %d\n", spawnpid);
	}

	return err;
}

/*	void printCommLine is a function used for testing.  This can be ignored.
*/
void printCommLine(struct commandInfo *userC){

	int i = 0;

	for (i = 0; i < userC->numCommands; i++) {
		printf("commArgs[%d] = %s\n", i, userC->parsedComm[i]);
	}

	return;
}