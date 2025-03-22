/* FILE :       main.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    The scooby gang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file hold the entry point for the chat - client program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>



#define ARG_COUNT 3
#define MAX_USER_ID 5
#define MAX_MESSAGE 80
#define MAX_SERVER_NAME 255



 // Function Prototypes (to split into new files) 

	// mainFunctions.h
bool parseArgs(int argc, char* argv[]);
bool getHostbyName();
bool createSocket();
bool connectToServer();
// threadFunctions.h
bool incomingThreadCreate()
bool recieveMessage();
bool displayMessage();
// ncursesFunctions.h


int main(int argc, char* argv[])
{

	char userID[MAX_USER_ID];			// User ID
	char serverName[MAX_SERVER_NAME];	// Server Name
	int socketID;						// Socket ID
	WINDOW* input_win, * output_win;    // ncurses windows for input and output

	printf("Welcome to the Chat - Client Terminal\n");

	if (argc != ARG_COUNT)
	{
		printf("\tERROR : Incorrect number of arguments.\n");
		printf("\t\tUsage: -user<userID> -server<server name>\n");
		return 0;
	}

	// Parse Arguments into variables for the program 
	if (!parseArgs(argc, argv))
	{
		printf("ERROR: Failed to parse arguments.\n");
		return -1;
	}



	// Resolve the server name to an IP address
	if (!getHostbyName())
	{
		printf("ERROR: Failed to resolve server name.\n");
		return -1;
	}



	// Create socket
	if (!createSocket())
	{
		printf("ERROR: Failed to create socket.\n");
		return 0;
	}

	// Connect to the server
	if (!connectToServer())
	{
		printf("ERROR: Failed to connect to server.\n");
		return 0;
	}

	// Initialize the ncurses library
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	// Create input window for typing messages 
	// Create output window for displaying chat history 
	input_win = newwin(3, COLS, LINES - 3, 0);
	output_win = newwin(LINES - 3, COLS, 0, 0);
	scrollok(output_win, TRUE);


	// Create a thread to handle incoming messages
	incomingThreadCreate();


	// Loop for User Input 
	While(true)
	{
		// Read user input from the Input Window (up to 80 characters)
		// Display the message in the Output Window.

		// If the user presses enter: 
		if ()
		{
			// Send message to the server using send().
			// Display the message in the Output Window.
		}


		// If the message is ">>bye<<",
		if ()
		{
			// Close the connection to the server
			close(socketID);

			// Clean the ncurses windows
			delwin(input_win);
			delwin(output_win);
			endwin();

			// Exit the program
			printf("Chat - Client is now closing, Goodbye!\n");
			return 0;
		}
	}

}






// Function to parse the arguments passed to the program
bool parseArgs(int argc, char* argv[])
{
	// Loop to parse the arguments passed to the program
	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-user", 5) == 0)
		{
			strncpy(userID, argv[i] + 5, MAX_USER_ID);
			userID[MAX_USER_ID] = '\0';
		}
		else if (strncmp(argv[i], "-server", 7) == 0)
		{
			strncpy(serverName, argv[i] + 7, sizeof(serverName) - 1);
			serverName[sizeof(serverName) - 1] = '\0';
		}
		else
		{
			return false;
		}
	}
	return true;


	return true;
}




// Use gethostbyname() to resolve the server name to an IP address
bool getHostbyName()
{
	// Resolve the server name to an IP address

	// If the server name is not found, return false
	// If the server name is found, return true
	return true;
}



bool createSocket()
{
	// Create a socket using socket()
	// If the socket is created, return true
	// If the socket is not created, return false
	return true;
}


bool connectToServer()
{
	// Connect to the server using connect()
	// If the connection is successful, return true
	// If the connection is not successful, return false
	return true;
}


bool incomingThreadCreate()
{
	// Create a thread to handle incoming messages
	// If the thread is created, return true
	// If the thread is not created, return false
	return true;
}



bool recieveMessage()
{
	// Receive a message from the server using recv()
	// If the message is received, return true
	// If the message is not received, return false
	return true;
}



bool displayMessage()
{
	// Display the message in the Output Window
	// If the message is displayed, return true
	// If the message is not displayed, return false
	return true;
}
