/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */



#include "programFunctions.h"
#include "messageFunctions.h"
#include "UIFunctions.h"


void getTimestamp()
{
	time_t currentTime;
	time(&currentTime);
	struct tm* timeInfo = localtime(&currentTime);
	strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeInfo);

	return;
}



bool hiMessage(char* clientIP)
{
	// Send initial message to the server (Say >>Hi<< to the server)
		// send the message to the server using send()												**********TODO**********  add the IP address of the client to the beginningmessage



	// Get the current time stamp
	getTimestamp();


	// Format message with necessary information
	snprintf(message, sizeof(message),
		"%.15s_[%.5s]_>>_%s_(%.8s)",		// message formatting string 
		clientIP,							// Positions 1-15: IP address
		userID,								// Positions 17-23: [userID] in square brackets and 5 chars max
		">>Hi<< Server, lets connect.",     // Positions 28-67: message with 
		timestamp);							// Positions 69-78: (HH:MM:SS)

	if (send(socketID, message, strlen(message), 0) == -1)
	{
		printf("ERROR: >>Hi<< Message to Server failed to send");
		return false;
	}

	// DO NOT DISPLAY THIS MESSAGE IN THE OUTPUT WINDOW ************************************DELETE BEFORE SUBMISSION

	return true;

}

void byeMessage(char* clentIP)
{
	// Send a message to the server to close the connection and exit the program
		// send the message to the server using send()												**********TODO**********  add the IP address of the client to the beginningmessage
	char byeMessage[MAX_SNT_MESSAGE];


	// Get the current time stamp
	getTimestamp();

	// Format message with necessary information
	snprintf(message, sizeof(message),
		"%.15s_[%.5s]_>>_%s_(%.8s)",		// message formatting string 
		clientIP,							// Positions 1-15: IP address
		userID,								// Positions 17-23: [userID] in square brackets and 5 chars max
		">>Bye<< Server, closing connection.",     // Positions 28-67: message with 
		timestamp);							// Positions 69-78: time in brackets (HH:MM:SS)

	if (send(socketID, message, strlen(message), 0) == -1)
	{
		printf("ERROR: >>Bye<< Message to Server failed to send"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}

	// DO NOT DISPLAY THIS MESSAGE IN THE OUTPUT WINDOW ************************************DELETE BEFORE SUBMISSION

	// Begin the process to close the connection and exit the program
	programEnd();

}


bool createMessage(char clientIP)
{
	// Create a message to send to the server and display in the Output Window

	//the user should be allowed to enter a message of up to 80 characters.
		//When the user hits the 80 character input boundary – the UI must stop accepting characters for input with no other notification to the user.

	// if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed




	// Get the current time stamp
	getTimestamp();


	// Get user input (up to 80 characters)
	char messageContent[MAX_SNT_MESSAGE + 1] = { 0 };
	wgetnstr(input_win, messageContent, MAX_SNT_MESSAGE); // **********************************TODO**********************************  check if this is the correct function to use for ncurses input

	// Skip if message area was empty 
	if (strlen(messageContent) == 0)
	{
		return false;
	}

	// if user enters the >>bye<< message, send the shutdown message to the server and close the connection
	if (stcmp(messageContent, ">>bye<<") == 0)
	{
		if (!byeMessage(clientIP))
		{
			return false;
		}

		return true;
	}



	// Format message with necessary information
	snprintf(message, sizeof(message),
		"%.15s_[%.5s]_>>_%.80s_(%.8s)",	// message formatting string 
		clientIP,							// Positions 1-15: IP address
		userID,								// Positions 17-23: [userID] in square brackets and 5 chars max
		message,							// Positions 28-67: 80 char message
		timestamp);							// Positions 69-78: (HH:MM:SS)


	if (send(socketID, message, strlen(message), 0) == -1)
	{
		return false;
	}

	// Display message in output window ***************************************TODO************  ncurses

	// Clear input window for next message
	/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/


	return true;
}




bool recieveMessage(char* clientIP, char* userID)
{
	// Receive a message from the server using recv()
	// Messages will be sent from the server in 40 character parcel chunks to stitch together 
	// Display the recieved message in the Incoming Messages  Window

	// If the message is received, return true
	// If the message is not received, return false

	// if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed
	// Each message being received within a client needs to contain the sending client’s IP address. - compare to ensure right client message 
	//Each message being received within a client needs to contain the sending client’s user-name (up to 5 characters) - compare to ensure right client message

	return true;
}






