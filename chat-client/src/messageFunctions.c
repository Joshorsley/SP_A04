/* FILE :       messageFunctions.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
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

#define MAX_BUFFER 1024 // max buffer size
#define MAX_SNT_MESSAGE 80 // max message length for sent messages


#define MAX_SNT_MESSAGE 80 // max message length for sent messages 
#define MAX_RECVD_MESSAGE 40 // max message size for sending/recieving messages
#define MAX_BUFFER 1024 // max buffer size									         **********TODO************ resize this to correct size
#define MAX_TIMESTAMP 10 // max timestamp size
#define MAX_IP 16 // max IP address size
#define MSG_ROW_START 1 




/*
* FUNCTION:
* DESCRIPTION:    This function sends a message to the server to establish the connection and start communication
*                   This message is not displayed in the output window
*/

bool hiMessage(ClientInfo* clientInfo)
{
    char message[MAX_BUFFER]; // Declare message variable with appropriate size

    strcpy(message, clientInfo->userID);

    // Send the message to the server
    if (send(clientInfo->socketID, message, strlen(message), 0) == -1)
    {
        return false;
    }

    return true;
}







/*
* FUNCTION:     byeMessage
* DESCRIPTION:  This function sends a message to the server to close the connection and exit the program
*               This message is not displayed in the output window
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


void receiveMessages(int socketID) 
{
    char buffer[MAX_BUFFER];
    while (1) 
	{
        memset(buffer, 0, MAX_BUFFER);
        int bytesReceived = recv(socketID, buffer, MAX_BUFFER - 1, 0);
        if (bytesReceived <= 0) 
		{
            //printf("Server disconnected or error occurred.\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        //printf("%s", buffer); 
    }
}