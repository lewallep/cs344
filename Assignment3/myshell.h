/*
Name: Philip Lewallen
Email: lewallep@onid.oregonstate.edu
Class: CS344-400
Assignment: Assignment3
*/

#define MAX_ARGS 512
#define MAX_ARG_CHARS 512

struct commandInfo {
	char *parsedComm[MAX_ARGS];
	char *commArgs[MAX_ARGS];
	char input[4096];
	char caughtInt[1000];
	char homeDir[100];
	int sizeParsedComm;
	int numCommands;
	int exitStatus;
	pid_t expid;
};

static const char COMM_HASH = '#';
static const char COMM_EXIT[] = "exit";
static const char COMM_CD[] = "cd";
static const char COMM_STATUS[] = "status";

void catchIntSignal(int sig);
void smallshParseComm(struct commandInfo *userC);
void parseCommInitalize(struct commandInfo *userC);
void parseCommFree(struct commandInfo *userC);
int sysCommand(struct commandInfo *userC);
void myExit();
int myCd(struct commandInfo *userC);
void myStatus(struct commandInfo *userC);
void printCommLine(struct commandInfo *userC);
void sysCommandOut(struct commandInfo *userC);
int sysCommandIn(struct commandInfo *userC);
int sysCommandBG(struct commandInfo *userC);