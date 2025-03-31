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
		.status = true,
		.inWin = NULL,			// Input window
		.outWin = NULL
	};

	bool serverOrIPFlag = false;		// Flag to note if 3rd arg is server name (true) or an IP address (false)
	bool programEndFlag = false;            // Flag to end the program, turned true when >>bye<< message is sent

	//Message msg;
	char buf[81] = {0};
	int msgRow = MSG_ROW_START;
	int maxPrintRow;

	char message[MAX_BUFFER];      
	static char timestamp[MAX_TIMESTAMP];		
	char clientIP[MAX_IP];					

	if (parseArgs(argc, argv, clientInfo.userID, clientInfo.serverName) == false)
	{
		printf("\tERROR : Incorrect number of arguments.\n");
        printf("\t\tUsage: chat-client –user<userID> –server<server name>\n");
		return -1;
	}

	clientInfo.socketID = connectToServer(&clientInfo);
	if (clientInfo.socketID == -1)
	{
		return -1;
	}

	hiMessage(&clientInfo);

	// Initialize the ncurses library
	initscr();
	cbreak();
	keypad(stdscr, TRUE);

	// Draw the input and output windows
	drawWin(&clientInfo.inWin, &clientInfo.outWin, &msgRow, &maxPrintRow);

	if (pthread_create(&displayThread, NULL, incomingMessages, (void*)&clientInfo) != 0)
	{
		return -1;
	}

	//send loop
	while (clientInfo.status)
	{
		// Get the message from the user
		getMsg(clientInfo.inWin, buf);
		buf[strcspn(buf, "\n")] = '\0';

		if (strcmp(buf, ">>bye<<") == 0)
		{
			byeMessage(&clientInfo);
			break;
		}

		if (send(clientInfo.socketID, buf, strlen(buf), 0) == -1)
		{
			break;
		}
	}

	pthread_cancel(displayThread);
	pthread_join(displayThread, NULL);

	endProg(clientInfo.inWin, clientInfo.outWin); // end the ncurses UI
	return 0;
}