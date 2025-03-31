/* FILE :       main.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    Kalina Cathcart, Josh Horsley, John Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file hold the entry point for the chat - client program. The main function will first set up the client information structure and variables for the program's use. 
 * 		 Once the set up is completed, the arguments are parsed for the user ID and serverName given by the user on the command line. 
 *		 The client will use this information to connect to the server.
 *		 Once connected to the server, the program will send a hi message to establish initial details for the server. 
 *		 The program will then set up the ncurses library and UI for for the input and output windows. 
 *		 Next, a thread is created to handle incoming messages from the server while the client performs other actions. 
 *		 The program will then enter a loop until the client ends the program. 
 *		 In the loop, the user will be able to enter messages into the output window and send them to the server.
 *		 If the user enters >>bye<<, then a message will be sent to end to the connection with the server and the client program will close. 
 *
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
		.inWin = NULL,	
		.outWin = NULL
	};

	char buf[81] = {0};
	int msgRow = MSG_ROW_START;
	int maxPrintRow;

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

	initscr();
	cbreak();
	keypad(stdscr, TRUE);

	drawWin(&clientInfo.inWin, &clientInfo.outWin, &msgRow, &maxPrintRow);

	if (pthread_create(&displayThread, NULL, incomingMessages, (void*)&clientInfo) != 0)
	{
		return -1;
	}

	while (clientInfo.status)
	{
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

	close(clientInfo.socketID); 

	endProg(clientInfo.inWin, clientInfo.outWin);
	return 0;
}
