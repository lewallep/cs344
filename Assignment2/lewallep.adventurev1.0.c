/*
Name: Philip Lewallen
Email: lewallep@onid.oregonstate.edu
Class: CS344-400
Assignment: Assignment2 - adventure
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
#include <dirent.h>

//function prototypes here.
void userInstructions();
void testRoomNames();
void createRoomDir(char roomDirName[]);
void gameMenu();
void makeRooms(char roomDirName[]);
void readRoomNames();
void roomConn();
void addLink();


//Constant Global Variables
const char * const roomNames[] = { "Garage", "Happy_Place", "Zelda", "Metroid",
	"Library", "Bat_Cave", "Awesome", "Zoo", "Mothra", "This_Island_Earth"};
const char * const roomType[] = {"START_ROOM", "END_ROOM"};
const int numRooms = 10;


/*
Creates the 7 Random rooms in the local working directory.  First Creates a new directory with
the usrname hard coded from my OSU username lewallep.rooms.<processid> number of 
*/ 
void createRoomDir(char roomDirName[]) {
	char userRoom[100] = "lewallep.rooms.";
	int pid = getpid();
	char pidChar[20];

	sprintf(pidChar, "%d", pid);	//Change the process id number into a string.
	strcpy(roomDirName, userRoom);		//Copy the string into my empty array
	strcat(roomDirName, pidChar);		//Concatonate the process id string into my current folder
	mkdir(roomDirName, 0700);			//Make the directory for the rooms.

	return;
}

//This function only tests I can see all of the global constant room names.
void testRoomNames() {
	int i;
	printf("The number of rooms is %d.\n", numRooms);

	for (i = 0; i < numRooms; i++) {
		printf("# %d Room is called %s.\n", i, roomNames[i]);
	}

	return;
}

void userInstructions() {
	printf("\tWelcome to CS344's second homework assignment.\n");
	printf("It is a choose your own adventure where you will be navigating\n");
	printf("Through some of the most devious mazes ever constructed.\n");
	printf("Or maybe something more along the lines of the involvement of FlappyBird.\n");
	printf("\n");
	printf("\tYour mission should you choose to accep it is to navigate from a starting room\n");
	printf("to a room with an exit from this maze.  Each room will have a random amount of\n");
	printf("connections to the other existing rooms.  There is only one exit.\n\n");

	return;
}

void makeRooms(char roomDirName[]) {
	int fd;
	int random;
	int i = 0;
	int room_checker[9];	//This is going to be used to check to make sure everyone room is only picked once.
	char roomPath[200];
	char cwd[150];			//Get the current working directory.  
	char *rtc[7];			//Stands for an array of rooms to create.

	getcwd(cwd, sizeof(cwd));
	strcat(cwd, "/");
	strcat(cwd, roomDirName);

	printf("the current cwd is %s\n", cwd);

	fd = chdir(cwd);

	//Initialize all of my variables to my rooms.
	for(i = 0; i < 9; i++){
		room_checker[i] = 0;
	}

	//Generating a list of room names to use to use for the middle rooms.
	//All I need are seven with the beginnning and end rooms always at beginning and end.
	for (i = 0; i < 7; i++) {
		rtc[i] = (char *) malloc(30 * sizeof(char));
	}

	i = 1;		//Resetting my generic counter to zero.
	//Picking out the rooms to use for this adventure.
	while (i < 6) {
		random = rand() % 7;
		random += 2;

		if (room_checker[random] == 0) {
			strcpy(rtc[i], roomNames[random]);
			room_checker[random] = 1;
			i++;
		}
	}

	//create the actual rooms in the directory.
	for (i = 0; i < 7; i++) {
		fd = creat(rtc[i], 0666);
	}
	
	return;
}

void addLink() {
	int ld;

	char room1[100] = "START_ROOM";
	char room2[100] = "END_ROOM";

	char curDir[100] = "/nfs/stak/students/l/lewallep/cs344/hw2/lewallep.rooms.11711";

	ld = chdir(curDir);

	ld = link(room1, room2);

	printf("ld is this %d.\n", ld);

	return;
}


void gameMenu() {


	return;
}

int main(int argc, char **argv) {

	char roomDirName[100];		//Array to hold the name of the rooms direcctory.
	srand (time(NULL));			//Initalizing my random number genrator.


	userInstructions();
	testRoomNames();		//This is to be commented out later in the program.
	createRoomDir(roomDirName);

	makeRooms(roomDirName);

	addLink();

	gameMenu();

	return 0;
}