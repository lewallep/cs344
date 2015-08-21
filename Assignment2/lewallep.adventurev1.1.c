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

//Structs are declared here.
struct roomData {
	char name[100];
	char type[50];
	char *conn[6];
	int numConns;
};

//function prototypes here.
void revRoomConns(struct roomData *gameRooms[]);
void userInstructions();
void testRoomNames(struct roomData *gameRooms[]);
void createRoomDir(char roomDirName[]);
void gameMenu();
void makeRooms(char roomDirName[], char *rtc[]);
void printRoomData(struct roomData *gameRooms[]);
void copyRoomConnectors(struct roomData *gameRooms[]);
void _reverseCopy(struct roomData *gameRooms[], int roomIndx, int i);


//Constant Global Variables
const char * const roomNames[] = { "Garage", "Happy_Place", "Zelda", "Metroid",
	"Library", "Bat_Cave", "Awesome", "Zoo", "Mothra", "This_Island_Earth"};
const char * const roomType[] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
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
void testRoomNames(struct roomData *gameRooms[]) {
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
/*
	Copies over the room names into the list of connected rooms. 
	For instance if room 1 has 3 connections this gets three room names to 
	add to the connections.
*/
void copyRoomConnectors(struct roomData *gameRooms[]) {
	int i, z, y;
	int rConn;
	int duplicateCheck[7];

	//Start at the first room and move my way down the list.
	for (i = 0; i < 7; i++) {

		//Reset the checking array.
		for (y = 0; y < 7; y++) {
			duplicateCheck[y] = 0;
		}

		z = 0;
		while (z < gameRooms[i]->numConns) {
		//Generate a random number to decided which room to link to.
			rConn = rand() % 7;
			//Check to make sure I do not reference the room I am in.
			if((rConn != i) && (duplicateCheck[rConn] != 1)) {
				strcpy(gameRooms[i]->conn[z], gameRooms[rConn]->name);
				duplicateCheck[rConn] = 1;
				z++;
			}
		}
	}
	return;
}

/*
	This will add the connection data of the rooms structs.  
	Adds how many links there should be.  
	It then allocates that memory for each array of connections to store the names of the rooms
	this room will be connected to.
*/
void roomConns(struct roomData *gameRooms[]) {

	int i, z;
	int numConn;
	
	for (i = 0; i < 7; i++) { 
		//randomly generate how many links there should be.
		numConn = rand() % 7;

		//Since a room has to have at least one connection this checks for that condition.
		if (numConn == 0 ) {
			numConn += 1;
		}

		gameRooms[i]->numConns = numConn;	//Stores the number of connections for each room.
	}//End of for loop.

	for ( i = 0; i < 7; i++) {
		for ( z = 0; z < 6; z++) {
			gameRooms[i]->conn[z] = malloc(sizeof(char) * 50);
		}//End inner for loop
	}//End outer for loop.

	copyRoomConnectors(gameRooms);

	return;
}

/*
Used for testing the room data I have entered.
*/
void printRoomData(struct roomData *gameRooms[]) {
	int i, z;
	for (i = 0; i < 7; i++) {
		printf("Room # %d is named %s.\n", i, gameRooms[i]->name);

		printf("The number of connections %d.\n", gameRooms[i]->numConns);
		printf("the room type is %s.\n", gameRooms[i]->type);
	
		for (z = 0 ; z < 6; z++) {
			printf("Connection #%d is %s.\n", z, gameRooms[i]->conn[z]);
		}

		printf("\n");	
	}
	printf("\n");
	return;
}

void makeRooms(char roomDirName[], char *rtc[]) {
	int fd;
	int random;
	int i = 0;
	int room_checker[9];	//This is going to be used to check to make sure everyone room is only picked once.
	//char roomPath[200];
	char cwd[150];			//Get the current working directory.  

	getcwd(cwd, sizeof(cwd));
	strcat(cwd, "/");
	strcat(cwd, roomDirName);

	//printf("the current cwd is %s\n", cwd);

	fd = chdir(cwd);

	//Initialize all of my variables to my rooms.
	for(i = 0; i < 9; i++){
		room_checker[i] = 0;
	}

	//Generating a list of room names to use to use for the middle rooms.
	//All I need are seven with the beginnning and end rooms always at beginning and end.
	for (i = 0; i < 7; i++) {
		rtc[i] = (char *) malloc(50 * sizeof(char));
	}

	i = 0;		//Resetting my generic counter to zero.
	//Picking out the rooms to use for this adventure.
	while (i < 7) {
		random = rand() % 10;
		//If I pull a duplicate room I don't increment the counter until I pull a new room from the list.
		if (room_checker[random] == 0) {
			strcpy(rtc[i], roomNames[random]);
			strcat(rtc[i], ".txt");
			room_checker[random] = 1;

			i++;
		}
	}

	//The rooms are created here.  The contents are going to be appended at a later time.
	for (i = 0; i < 7; i++) {
		fd = creat(rtc[i], 0666);
	}
	
	return;
}

/*
This function allocates the memory needed for the game rooms.
It also copies over the name of the rooms in order they were made to the new room data
structure.
*/
void initRooms(char *rtc[], struct roomData *gameRooms[]) {
	int i;
	//Allocating mmeory for the array of room structures.
	for (i = 0; i < 7; i++) {
		gameRooms[i] = malloc(sizeof (struct roomData));
	}

	//Copying over the room names.
	for (i = 0; i < 7; i++) {
		strcpy(gameRooms[i]->name, rtc[i]);
	}

	//since the rooms are generated automatically I statically assign the beginnign and end.
	strcpy(gameRooms[0]->type, roomType[0]);
	strcpy(gameRooms[6]->type, roomType[1]);

	for (i = 1; i < 5; i++) {
		strcpy(gameRooms[i]->type, roomType[2]);
	}

	return;
}

/*
The function reads through the connections of each room.  It then follows each connection and ensures
there is a reverse connection back to the origin room.  So if room 1 has a connection to 2 but room
2 does not have a connection to 1 ,then it calls the section function which adds a new room connection.
*/
void revRoomConns(struct roomData *gameRooms[]) {
	char *startRoom = malloc(sizeof(char) * 50);
	int i, z;		//Generic counter.
	int roomIndx = 0;

	int connArr[7];

	for (i = 0; i < 7; i++) {
		connArr[i] = gameRooms[i]->numConns;
	}


	for ( i =  0; i < 7; i++) {
		for (z = 0; z < connArr[i]; z++) {
			strcpy(startRoom, gameRooms[i]->conn[z]); 

			//This will find the room index I am to copy the reverse links from.
			while(strcmp(startRoom, gameRooms[roomIndx]->name) != 0) {
				roomIndx++;
			}
//			printf("I have found %s\n", gameRooms[roomIndx]->name);
//			printf("At roomIndx %d.\n", roomIndx);

			_reverseCopy(gameRooms, roomIndx, i);






			//This must only be reset after I have copied over the room connections.
			roomIndx = 0;
		}
	}


	return;
}

/*
Argument definition
i:	this is the index of the room itself in the array of gameRooms.
roomIndx:  This is the index of the iteration through the room list
gameRooms:  This is the array holding the pointers to the roomData structs.
*/

void _reverseCopy(struct roomData *gameRooms[], int roomIndx, int i){

	int connCount;

	int foundReverseLink = 0;
	int connIndx = 0;		//Checks to make sure I have run through all of the available connections.
//	printf("_reverseCopy the roomIndx is %d.\n", roomIndx);
//	printf("i value is %d.\n", i);
//	printf("the number of conns for room one is %d.\n", gameRooms[i]->numConns);
//	printf("We are in Room \t%s.\n", gameRooms[i]->name);
//	printf("we are looking at connectioins is %s.\n", gameRooms[roomIndx]->name);

	//printf("The connections in room %s are \n", gameRooms[roomIndx]->name);

	while(connIndx < gameRooms[roomIndx]->numConns) {

		//printf("%s\n ", gameRooms[roomIndx]->conn[connIndx]);

		while(connIndx < gameRooms[roomIndx]->numConns) {

			if (strcmp(gameRooms[i]->name, gameRooms[roomIndx]->conn[connIndx]) == 0) {
				foundReverseLink = 1;
			}
			connIndx++;
		}

		if (foundReverseLink == 0 ) {
			//printf("No reverse link has been found.  attemping to copy.\n");
			//printf("Current room count before copy %d.\n", gameRooms[roomIndx]->numConns);
			connCount = gameRooms[roomIndx]->numConns;
			//printf("Connections %d.\n", connCount);
			//printf("gameRooms[roomIndx]->conn[numCount]  %d\n", gameRooms[roomIndx]->numConns);
			//printf("gameRooms[i]->name    %s\n", gameRooms[i]->name);
			strcpy(gameRooms[roomIndx]->conn[connCount], gameRooms[i]->name);
			connCount++;
			gameRooms[roomIndx]->numConns = connCount;
		}


		//printf("The new connIndex is %d.\n", connIndx);
	}

	//gameRooms[roomIndx]->numConns = connCount;

	//printf("\n\n");

	//scanf("%s", input);

	return;
}


/*
The game menu reads the roooms and let's the user choose their path to victory.
*/
void gameMenu() {




	return;
}

int main(int argc, char **argv) {

//	int i; 						//Generic counter
	char *rtc[7];					//Stands for an array of rooms to create.
	char roomDirName[100];			//Array to hold the name of the rooms direcctory.
	struct roomData *gameRooms[6];	//Array of all of the room data.
	srand (time(NULL));			//Initalizing my random number genrator.


	//Begin main run of functions.
	userInstructions();
	//testRoomNames();				//This is to be commented out later in the program.
	createRoomDir(roomDirName);
	makeRooms(roomDirName, rtc);
	initRooms(rtc, gameRooms);		//See function description.
	roomConns(gameRooms);

	printRoomData(gameRooms);		//Just for testing.

	revRoomConns(gameRooms);


	printRoomData(gameRooms);		//testing to see the effect of revRoomConns.  Will remove.

	gameMenu();



	return 0;
}

//If I get a chance deallocate the memory for everything.