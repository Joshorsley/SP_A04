/* FILE :       messageFunctions.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :	Kalina Cathcart, Josh Horsley, Jon Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the functions related to creating, sending and recieving messages.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
#include "programFunctions.h"
#include "messageFunctions.h"
#include "UIFunctions.h"
#include "main.h"

#define MAX_SNT_MESSAGE 80 
#define MAX_RECVD_MESSAGE 40 
#define MAX_BUFFER 1024 
#define MAX_TIMESTAMP 10 
#define MAX_IP 16 
#define MSG_ROW_START 1 



/*
* FUNCTION:     hiMessage
* DESCRIPTION:  Sends an initial connection message to the server with the user ID.
*               This message is not displayed in the UI.
* PARAMETERS:   ClientInfo* clientInfo - pointer to ClientInfo structure
* RETURNS:      bool - true if message was sent successfully, false otherwise
*/

bool hiMessage(ClientInfo* clientInfo)
{
    char message[MAX_BUFFER];

    strcpy(message, clientInfo->userID);

    if (send(clientInfo->socketID, message, strlen(message), 0) == -1)
    {
        return false;
    }

    return true;
}




/*
* FUNCTION:     byeMessage
* DESCRIPTION:  Sends a termination message to the server to close the connection.
*               This message isn't displayed in the UI.
* PARAMETERS:   ClientInfo* clientInfo - pointer to ClientInfo structure
* RETURNS:      bool - true if message was sent successfully, false otherwise
*/

void byeMessage(ClientInfo* clientInfo)
{
    
    char message[MAX_BUFFER]; 
    strcpy(message, ">>bye<<");

    if (send(clientInfo->socketID, message, strlen(message), 0) == -1)
    {
        return false;
    }

    return true;
}



/*
* FUNCTION:     receiveMessages
* DESCRIPTION:  Continuously receives messages from the server and displays them in the output window.
*               Manages message display rotation when the window fills up.
* PARAMETERS:   ClientInfo* info - pointer to ClientInfo structure containing connection details
* RETURNS:      void
*/
void receiveMessages(ClientInfo* info) 
{
    int socketID = info->socketID;
    int rowNum = MSG_ROW_START;
    int maxPrintRow = 10;
    char buffer[MAX_BUFFER];
    while (1) 
	{
        memset(buffer, 0, MAX_BUFFER);
        int bytesReceived = recv(socketID, buffer, MAX_BUFFER - 1, 0);
        if (bytesReceived <= 0) 
		{
            break;
        }
        buffer[bytesReceived] = '\0';
        printMsg(info->outWin, rowNum, buffer);
        rowNum++;
        if (rowNum > maxPrintRow)
        {
            rowNum = MSG_ROW_START;
            werase(info->outWin);
        }
    }
}
