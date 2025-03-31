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

	char buf[81] = {0};
	int msgRow = MSG_ROW_START;
	int maxPrintRow;

	char message[MAX_BUFFER];      
	static char timestamp[MAX_TIMESTAMP];		
	char clientIP[MAX_IP];					



	printf("========================================\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
	printf("Welcome to the Chat - Client Terminal\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY

	if (parseArgs(argc, argv, clientInfo.userID, clientInfo.serverName) == false)
	{
		printf("ERROR: Failed to parse arguments.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return -1;
	}

	clientInfo.socketID = connectToServer(&clientInfo);
	if (clientInfo.socketID == -1)
	{
		printf("ERROR: Failed to create connection.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return -1;
	}

	hiMessage(&clientInfo);



	if (pthread_create(&displayThread, NULL, incomingMessages, (void*)&clientInfo) != 0)
	{
		printf("ERROR: Failed to create incoming thread.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		
		return -1;
	}


	//send loop
	while (clientInfo.status)
	{

		printf("Enter your message: ");
		fgets(buf, sizeof(buf), stdin);

		buf[strcspn(buf, "\n")] = '\0';


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