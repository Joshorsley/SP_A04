/* FILE :       main.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    Kalina Cathcart, Josh Horsley, John Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file hold the entry point for the chat - client program.
 */

#include "programFunctions.h"
#include "UIFunctions.h"
#include "messageFunctions.h"
#include "main.h"

#include <ncurses.h>


int main(int argc, char* argv[])
{
	pthread_t displayThread;

	ClientInfo clientInfo = {
		.userID = {0},
		.socketID = -1,
		.serverName = {0},
		.serverAddress = {0},
		.outWin = NULL,
		.inWin = NULL,
		.status = true
	};

	bool serverOrIPFlag = false;		// Flag to note if 3rd arg is server name (true) or an IP address (false)
	bool programEndFlag = false;            // Flag to end the program, turned true when >>bye<< message is sent

	// UI Variables
	WINDOW* inWin;			// Input window
	WINDOW* outWin;			// Output window
	//Message msg;
	char buf[81];
	int msgRow = MSG_ROW_START;
	int maxPrintRow;
	// message variables 
	char message[MAX_BUFFER];           // Buffer for user messages
	static char timestamp[MAX_TIMESTAMP];			// Timestamp for messages
	char clientIP[MAX_IP];					// Client IP address for messages 



	//printf("========================================\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
	//printf("Welcome to the Chat - Client Terminal\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY

	if (parseArgs(argc, argv, clientInfo.userID, clientInfo.serverName) == false)
	{
		//printf("ERROR: Failed to parse arguments.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return -1;
	}

	clientInfo.socketID = connectToServer(&clientInfo);
	if (clientInfo.socketID == -1)
	{
		//printf("ERROR: Failed to create connection.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return -1;
	}

	hiMessage(&clientInfo); // send the hi message to the server


	// Initialize the ncurses library
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	drawWin(&inWin, &outWin, &msgRow, &maxPrintRow); // Draw the input and output windows

	// if (pthread_create(&displayThread, NULL, incomingMessages, (void*)&clientInfo) != 0)
	// {
	// 	printf("ERROR: Failed to create incoming thread.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		
	// 	return -1;
	// }


	//send loop
	while (clientInfo.status)
	{
		// Get the message from the user
		getMsg(clientInfo.inWin, buf);

		// Check if the user wants to exit
		if (strcmp(buf, ">>bye<<") == 0)
		{
			byeMessage(&clientInfo);
			break;
		}

		if (send(clientInfo.socketID, buf, strlen(buf), 0) == -1)
		{
			printf("ERROR: Failed to create message.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
			break;
		}
		resetInputWin(clientInfo.inWin);
	}






	pthread_cancel(displayThread);
	pthread_join(displayThread, NULL);

	//programEnd(socketID, inWin, outWin); // end the program
	return 0;

}

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

