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

#define MAX_BUFFER 1024 // max buffer size
#define MAX_SNT_MESSAGE 80 // max message length for sent messages


#define MAX_SNT_MESSAGE 80 // max message length for sent messages 
#define MAX_RECVD_MESSAGE 40 // max message size for sending/recieving messages
#define MAX_BUFFER 1024 // max buffer size									         **********TODO************ resize this to correct size
#define MAX_TIMESTAMP 10 // max timestamp size
#define MAX_IP 16 // max IP address size
#define MSG_ROW_START 1 


/*
* FUNCTION:             getTimestamp
* DESCRIPTION:      This function gets the current time and stores it in the timestamp variable
*/
void getTimestamp(char* timestamp) 
{
    time_t currentTime;
    time(&currentTime);
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timestamp, MAX_TIMESTAMP, "%H:%M:%S", timeInfo);
}




/*
* FUNCTION:             
* DESCRIPTION:    This function sends a message to the server to establish the connection and start communication
*                   This message is not displayed in the output window  
*/

bool hiMessage(char* message, char* clientIP, bool programEndFlag, char* userID, char* timestamp, char* clientIP, int socketID) 
{
    // Send initial message to the server (Say >>Hi<< to the server)
    // send the message to the server using send() **********TODO**********  add the IP address of the client to the beginning of the message

    // Get the current time stamp
    getTimestamp(timestamp);

    strcpy(message, ">>Hi<< Server, lets connect.");

    // Send the message to the server
    if (!sendMessage(message, clientIP, userID, timestamp, programEndFlag, socketID)) 
    {
        printf("ERROR: >>Hi<< Message to Server failed to send");
        return false;
    }

    // DO NOT DISPLAY THIS MESSAGE IN THE OUTPUT WINDOW ************************************DELETE BEFORE SUBMISSION

    return true;
}







/*
* FUNCTION:     byeMessage
* DESCRIPTION:  This function sends a message to the server to close the connection and exit the program
*               This message is not displayed in the output window
*/

void byeMessage(char* message, char* clientIP, bool programEndFlag, char* userID, char* timestamp, char* clientIP, int socketID) 
{
    // Send a message to the server to close the connection and exit the program
    // send the message to the server using send() **********TODO**********  add the IP address of the client to the beginning of the message

    strcpy(message, ">>Bye<< Server, closing connection.");

    // Get the current time stamp
    getTimestamp(timestamp);

    if (!sendMessage(message, clientIP, userID, timestamp, programEndFlag, socketID)) 
    {
        return false;
    }

    // DO NOT DISPLAY THIS MESSAGE IN THE OUTPUT WINDOW ************************************DELETE BEFORE SUBMISSION

    // Begin the process to close the connection and exit the program
    endProgramFlag = true;
    return true;
}





/*
* FUNCTION:
* DESCRIPTION: This function takes the user's input and creates a message to send to the server.
*               The message is displayed in the output window
*/

bool createMessage(char* message, char* clientIP, bool programEndFlag, char* userID, char* timestamp, char* clientIP, int socketID) 
{
    // Create a message to send to the server and display in the Output Window

    // the user should be allowed to enter a message of up to 80 characters.
    // When the user hits the 80 character input boundary – the UI must stop accepting characters for input with no other notification to the user.

    // if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed

    // Get the current time stamp
    getTimestamp(timestamp);

    // Get user input (up to 80 characters)
    char messageContent[MAX_SNT_MESSAGE + 1] = { 0 };
    wgetnstr(input_win, messageContent, MAX_SNT_MESSAGE); // **********************************TODO**********************************  check if this is the correct function to use for ncurses input

    // Skip if message area was empty 
    if (strlen(messageContent) == 0) 
    {
        return false;
    }

    // if user enters the >>bye<< message, send the shutdown message to the server and close the connection
    if (strcmp(messageContent, ">>bye<<") == 0) 
    {
        if (!byeMessage(message, clientIP, userID, timestamp, programEndFlag, socketID)) 
        {
            return false;
        }

        // if bye message sent, end the createMessage function here and return to main to end program 
        // DO NOT DISPLAY THE GOODBYE MESSAGE IN THE OUTPUT WINDOW ************************************DELETE BEFORE SUBMISSION
        return true;
    }

    if (!sendMessage(message, clientIP, userID, timestamp, programEndFlag, socketID)) 
    {
        return false;
    }

    // Display message in output window ***************************************TODO************  ncurses

    // Clear input window for next message
    /*
     __
 .--()°'./
'|, . ,'        NCurses Be needed here
 !_-(_\

    */

    return true;
}





/*
* FUNCTION:
* DESCRIPTION: this function prepares the message format and sends the hi, bye or user message to the server. 
*   	        The message is displayed in the output window
* 			   The function will return true if the message is successfully sent, and false otherwise
*/
bool sendMessage(char* message, char* clientIP, bool programEndFlag, char* userID, char* timestamp, char* clientIP, int socketID) 
{
    // Format message with necessary information
    snprintf(message, MAX_BUFFER,
        "%.15s_[%.5s]_%.80s_(%.8s)", // message formatting string                                 // removed >> from format string, server will handle addition of (make sure to showe this when displaying our message in the incoming message window)  ******TODO**********
        clientIP,                      // Positions 1-15: IP address
        userID,                        // Positions 17-23: [userID] in square brackets and 5 chars max
        message,                       // Positions 28-67: up to 80 char message
        timestamp);                    // Positions 69-78: timestamp in brackets (HH:MM:SS)

    if (send(socketID, message, strlen(message), 0) == -1) 
    {
        printf("ERROR: Failed to send message to server.\n");
        return false;
    }

    return true;
}





/*
* FUNCTION:
* DESCRIPTION: This function recieves the message from the server and displays it in the incoming messages window
* 			 The function will return true if the message is successfully received, and false otherwise
*                 ************************************************************************************************************ TO DO ********** This function needs to be finished and tested
*/
bool receiveMessage(char* clientIP, char* userID) 
{
    // Receive a message from the server 
    char* buffer = (char*)calloc(MAX_BUFFER, sizeof(char));
    read(socketID, buffer, MAX_BUFFER);
    // Messages will be sent from the server in 40 character parcel chunks to stitch together 
    // Display the received message in the Incoming Messages  Window

    // If the message is received, return true
    // If the message is not received, return false

    // if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed
    // Each message being received within a client needs to contain the sending client’s IP address - compare to ensure right client message 
    // Each message being received within a client needs to contain the sending client’s user-name (up to 5 characters) - compare to ensure right client message

    return true;
}
