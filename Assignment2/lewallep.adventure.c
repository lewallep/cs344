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
void createRoomDir(char *roomDirName);
void gameMenu();
void makeRooms(char *roomDirName, char *rtc[]);
void printRoomData(struct roomData *gameRooms[]);
void copyRoomConnectors(struct roomData *gameRooms[]);
void _reverseCopy(struct roomData *gameRooms[], int roomIndx, int i);
void writeRoomData(struct roomData *gameRooms[], char *roomDirName);
void initRooms(char *rtc[], struct roomData *gameRooms[]);


//Constant Global Variables
const char * const roomNames[] = { "Garage", "Happy_Place", "Zelda", "Metroid",
	"Library", "Bat_Cave", "Awesome", "Zoo", "Mothra", "This_Island_Earth"};
const char * const roomType[] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
const int numRooms = 10;


/*
Creates the 7 Random rooms in the local working directory.  First Creates a new directory with
the usrname hard coded from my OSU username lewallep.rooms.<processid> number of 
*/ 
void createRoomDir(char *roomDirName) {
	char userRoom[100] = "lewallep.rooms.";
	int pid = getpid();
	char pidChar[20];

	sprintf(pidChar, "%d", pid);		//Change the process id number into a string.
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
	printf("\n\tWelcome to CS344's second homework assignment.\n");
	printf("It is a choose your own adventure where you will be navigating\n");
	printf("Through some of the most devious mazes ever constructed.\n");
	printf("Or maybe something more along the lines of the involvement of FlappyBird.\n");
	printf("\n");
	printf("\tYour mission should you choose to accept it is to navigate from a starting room\n");
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

/*
This function simply makes the room files in the directory with the current 
process id.  
*/
void makeRooms(char *roomDirName, char *rtc[]) {
	int fd;
	int random;
	int i = 0;
	int room_checker[9];	//This is going to be used to check to make sure everyone room is only picked once.
	char cwd[300];			//Get the current working directory.  


	getcwd(cwd, sizeof(cwd));
	strcat(cwd, "/");
	strcat(cwd, roomDirName);

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
			//strcat(rtc[i], ".txt");			//This can be commented out with no effect on the rest of the code.
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
	char roomName[100];

	strcpy(roomName, rtc[0]);		//I am not sure why the next for loop overwrites this first array element.

	//Allocating mmeory for the array of room structures.
	for (i = 0; i < 7; i++) {
		gameRooms[i] = malloc(sizeof(struct roomData));
	}

	strcpy(rtc[0], roomName);		//I am not sure why this gets overwritten.  I must see what is going on.

	//Copying over the room names.
	for (i = 0; i < 7; i++) {
		strcpy(gameRooms[i]->name, rtc[i]);
	}

	//since the rooms are generated automatically I statically assign the beginnign and end.
	strcpy(gameRooms[0]->type, roomType[0]);
	strcpy(gameRooms[6]->type, roomType[1]);

	for (i = 1; i < 6; i++) {
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

			_reverseCopy(gameRooms, roomIndx, i);
			//This must only be reset after I have copied over the room connections.
			roomIndx = 0;
		}
	}

	return;
}

/*
Argument definition
i:	this is the index of the room itself in the array of gameRooms.  I use this to search
	through the potential rooms in the list.  Then use the next variable to iterate through
	the available connections of that room.
roomIndx:  This is the index of the iteration through the room list
gameRooms:  This is the array holding the pointers to the roomData structs.
*/
void _reverseCopy(struct roomData *gameRooms[], int roomIndx, int i){
	//declaring Local Variable.
	int connCount;
	int foundReverseLink = 0;
	int connIndx = 0;		//Checks to make sure I have run through all of the available connections.

	while(connIndx < gameRooms[roomIndx]->numConns) {

		//printf("%s\n ", gameRooms[roomIndx]->conn[connIndx]);

		while (connIndx < gameRooms[roomIndx]->numConns) {

			if (strcmp(gameRooms[i]->name, gameRooms[roomIndx]->conn[connIndx]) == 0) {
				foundReverseLink = 1;
			}
			connIndx++;
		}

		if (foundReverseLink == 0 ) {
			connCount = gameRooms[roomIndx]->numConns;
			strcpy(gameRooms[roomIndx]->conn[connCount], gameRooms[i]->name);
			connCount++;
			gameRooms[roomIndx]->numConns = connCount;
		}
	}

	return;
}


/*
The game menu reads the roooms and let's the user choose their path to victory.
It will record a number of turns the user takes to get to the END_ROOM to the 
START_ROOM.  
*/
void gameMenu(struct roomData *gameRooms[]) {
	//Declare local variables.
	int i;					//Generic counters.
	int curRoom;			//Index of the current room location.
	int movCnt = 0;			//This will count the amount of moves between rooms.
	int entryMatch;			//Acts as a match boolean.
	int ptv[1000];			//I figure if the user takes more than a thousand turns they deserve to segfault.
	char usrInput[50];		//The user's input from the keyboard.
	char nextRoom[50];		//Stores what the next room is going to be.
	char curType[50];		//Stores the current room type
	const char endRoom[50] = "END_ROOM";

	curRoom = 0;	//Initialize the starting room as curRoom as 0.
	strcpy(curType, gameRooms[curRoom]->type);

	//I need a while loop which terminates when the room type is END_ROOM.
	while (strcmp(endRoom, gameRooms[curRoom]->type) != 0) {	//#1

		entryMatch = 0;		//Initialize and reset the entry

		//Loops through all of the connections in this
		while (entryMatch == 0) {		//#2
			printf("CURRENT LOCATION: %s\n", gameRooms[curRoom]->name);	//Print the current location
			printf("POSSIBLE CONNECTIONS: ");			//pretty self explanatory.

			for (i = 0; i < (gameRooms[curRoom]->numConns - 1); i++) {
				printf("%s, ", gameRooms[curRoom]->conn[i]);
			}
			printf("%s. ", gameRooms[curRoom]->conn[i]);		//Printing out the last name with the "."

			printf("\nWHERE TO? >");		//Ouptut and wait for user.
			scanf("%s", usrInput);			//Get the user's choice

			//Loop through the conn variable to see if the user has entered an acceptable name.
			//if found the entry Match goes to 1 and the user moves on.
			for (i = 0; i < gameRooms[curRoom]->numConns; i++) {//#3
				if (strcmp(usrInput, gameRooms[curRoom]->conn[i]) == 0) {
					entryMatch = 1;			//Marked we have found a good match.
					strcpy(nextRoom, gameRooms[curRoom]->conn[i]);	//Copying over the next room to go to.
					printf("\n");			//Newline for formatting.

					//move to the new room and increment the counter of movCnt;
					for (i = 0; i < 7; i++) {//#4 Loop
						//checks each room name to find where the next room is.
						if(strcmp(gameRooms[i]->name, nextRoom) == 0) {	
						curRoom = i;			//Get the index of the next room to move to.

						movCnt++;
						ptv[movCnt] = i;
						}
					}//#4 Loop End
				}
			}//#3
			if (entryMatch == 0) {
				printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			}

			
		}//#2
	}//#1 End of the game loop.  This is where we would test for the END_ROOM.
	
	printf("\n");
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", movCnt++);

	//Prints out the names of all of the rooms visited in the order
	//they were visited.
	for (i = 1; i < movCnt; i++) {
		printf("%s\n", gameRooms[ptv[i]]->name);
	}
	printf("\n");

	return;
}

void writeRoomData(struct roomData *gameRooms[], char *roomDirName) {
	char R_NAME[] = "ROOM NAME: ";		//Naming some constants for writing to the files
	char R_CONN[] = "CONNECTION ";		
	char R_TYPE[] = "ROOM TYPE: ";
	int i, z;			//More generic counters.
	FILE *fp;				//File pointer.



	for (i = 0; i < 7; i++) {
		fp = fopen(gameRooms[i]->name, "w");
		fprintf(fp, "%s%s\n", R_NAME, gameRooms[i]->name);

		for (z = 1; z <= gameRooms[i]->numConns; z++){
			fprintf(fp, "%s%d: %s\n", R_CONN, z, gameRooms[i]->conn[z - 1]);
		}

		fprintf(fp, "%s%s", R_TYPE, gameRooms[i]->type);

		fclose(fp);
	}


	return;
}

int main(int argc, char **argv) {

//	int i; 							//Generic counter
	char *rtc[7];					//Stands for an array of rooms to create.
	char *roomDirName = malloc(sizeof(char) * 100);			//Array to hold the name of the rooms direcctory.
	struct roomData *gameRooms[6];	//Array of all of the room data.
	srand (time(NULL));				//Initalizing my random number genrator.

	//Begin function calls.
	createRoomDir(roomDirName);
	makeRooms(roomDirName, rtc);
	initRooms(rtc, gameRooms);		
	roomConns(gameRooms);
	revRoomConns(gameRooms);		
	//The rooms are now created in memory and ready for traversal and to be written out to the files.


	writeRoomData(gameRooms, roomDirName);
	userInstructions();

	gameMenu(gameRooms);
	
	free(roomDirName);
	return 0;
}