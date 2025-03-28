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
#include <unistd.h> 
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "programFunctions.h"
#include "UIFunctions.h"
#include "messageFunctions.h"



#define PORT 8000 // port number for the server

#define ARG_COUNT 3		// expected arg count
#define MAX_USER_ID 5	// max user ID length
#define MAX_SERVER_NAME 255 // max server name length

#define ARG1_SKIP 5 // skip the first 5 characters of the argument
#define ARG2_SKIP 7 // skip the first 7 characters of the argument
#define IP_DOT_COUNT 3 // number of periods in an IP address

#define MAX_SNT_MESSAGE 80 // max message length for sent messages 
#define MAX_RECVD_MESSAGE 40 // max message size for sending/recieving messages
#define MAX_BUFFER 1024 // max buffer size									         **********TODO************ resize this to correct size
#define MAX_TIMESTAMP 10 // max timestamp size
#define MAX_IP 16 // max IP address size





/*

Notes :
 -  in the chat-client, do not need to worry about dealing with / handling the delete, backspace or arrow keys
 -  no debugging messages being printed to the screen in your final client

 - Input Window:
		- Should display each of the messages sent through the server by this client or others (excluding >>hi<< or >>bye<< messages)
		- should be written in a specific format ( see function notes for more details)
		-  should be able to show the history of at most the last 10 “lines” from the messages sent and received.
				- HINT: The fact that there are starting and stopping positions for fields in the message output should indicate the potential solution for you

 - Output Window
		- only holds the cuurent input from the user for the next message they may wish to send.
		- a message is allowed to take up 2 “lines” in the output window (i.e. one line for each of the 40 character messages) ...
			- this requirement indicates that a maximum of 10 lines of output are present in the message window before being scrolled ...
		- When the message is sent, the message field here should clear and the message should be displayed in the incoming message window.(unless hi or bye)

- When user enters >>bye<<
		- the client should close the connection to the server and exit the program.
		- Client should never recieve other client's goodbye messages, if they do there is an issue in the server


- Current Message Composition:
		- the user should be allowed to enter a message of up to 80 characters.
		-  80 character  boundary, message is split into 40 char packages by the server side before sending to other clients
		- Message structure being send by Client:
		messsageType | username | content of message
			- messageType =
							">>hi<<" = initial message to server and only for the server, contains client IP and client’s userID, where the server will establish the connection and ensure the userID is unique, if not the user will have to send a new hi message.
							"message" = regular message type being sent to other clients (server will package into chunks and send to other clients)
							">>bye<<" = message to server and only for the server, to close connection and exit program
			- username = user ID of the client sending the message
			- content of message = the message being sent by the client or additional information to be communicated to the server


*/


int main(int argc, char* argv[])
{

	char userID[MAX_USER_ID];			// User ID
	char serverName[MAX_SERVER_NAME];	// Server Name
	char serverAddress[MAX_IP];			// Server Address
	bool serverOrIPFlag = false;		// Flag to note if 3rd arg is server name (true) or an IP address (false)
	int socketID;						// Socket ID

	WINDOW* inWin;					// ncurses windows for input 
	WINDOW* outWin;					// ncurses windows for output
	UIProps inWinProps;				// UI properties for input window
	UIProps outWinProps;				// UI properties for output window

	char message[MAX_BUFFER];           // Buffer for user messages
	static char timestamp[MAX_TIMESTAMP];			// Timestamp for messages
	char clientIP[MAX_IP];					// Client IP address for messages 
	
	bool programEndFlag = false;		// Flag to end the program, turned true when >>bye<< message is sent


	printf("========================================\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
	printf("Welcome to the Chat - Client Terminal\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY

	// Create UI windows
	initscr(); // Initialize ncurses
    refresh(); // Refresh the screen to show changes

	// Set properties for input window
	inWinProps.height = 10; // Height of the input window
	inWinProps.width = COLS; // Width of the input window
	inWinProps.starty = 0; // Starting y position of the input window
	inWinProps.startx = 0; // Starting x position of the input window

	// Set properties for output window
	outWinProps.height = LINES - inWinProps.height - 1; // Height of the output window
	outWinProps.width = COLS; // Width of the output window
	outWinProps.starty = 0; // Starting y position of the output window
	outWinProps.startx = inWinProps.height + 1; // Starting x position of the output window

	// Create input and output windows
	inWin = createNewWin(inWinProps.height, inWinProps.width, inWinProps.starty, inWinProps.startx); // Create input window
	scrollok(inWin, TRUE);

	outWin = createNewWin(outWinProps.height, outWinProps.width, outWinProps.starty, outWinProps.startx); // Create output window
	scrollok(outWin, TRUE);

	// Start the program with initial functions 
	if (!programStart(argc, argv))
	{
		return -1;
	}

	

	// Loop for User Input 
	while(programEndFlag)
	{
		// Get user input
		input(inWin, message); // Get user input from the input window
		// Check if the user wants to exit
		if (byeCheck(message))
		{
			programEndFlag = true; // Set the program end flag to true
			break; // Exit the loop
		}

		// Get user input and create message with it  
		if (strlen(message) > 0)
		{
			if(!createMessage(clientIP, programEndFlag))
			{
				printf("ERROR: Failed to create message.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
				
				return -1; 
			}

			// Display the message in the Output Window.
			output(outWin, message); // Display the message in the output window

		}

	}

	programEnd(socketID, inWin, outWin); // end the program
	return 0; 

}



