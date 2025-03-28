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


#include "ncurses-samples/src/ncurses-01.c" // ******TODO****** remove these lines and related files, they are for testing purposes only
#include "ncurses-samples/src/ncurses-02.c"
#include "ncurses-samples/src/ncurses-03.c"

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


 // Function Prototypes (to split into new files) 

	// programFunctions.h
bool programStart(int argc, char* argv[], bool serverOrIPFlag)
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag);
bool resolveServerName();
bool createSocket();
bool connectToServer();
bool IPCheck(char* serverName);
bool getClientIP(char* clientIP);
void programEnd();

// UIFunctions.h
bool incomingThreadCreate();
bool outgoingThreadCreate();
bool displayMessage();

// messageFunctions.h
void getTimestamp();
bool hiMessage(char* clientIP);
void byeMessage(char* clientIP);
bool createMessage(char* clientIP);
bool recieveMessage(char* clientIP);







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
	bool serverOrIPFlag = false;		// Flag to note if 3rd arg is server name (true) or an IP address (false)
	int socketID;						// Socket ID

	WINDOW* input_win;					// ncurses windows for input 
	WINDOW* output_win;					// ncurses windows for output

	char message[MAX_BUFFER];           // Buffer for user messages
	static char timestamp[MAX_TIMESTAMP];			// Timestamp for messages
	char clientIP[MAX_IP];					// Client IP address for messages 
	



	printf("========================================\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
	printf("Welcome to the Chat - Client Terminal\n");	 // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY



	// Start the program with initial functions 
	if (!programStart(argc, argv))
	{
		return -1;
	}

	

	// Loop for User Input 
	while(true)
	{
		// Clear the input window

		/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/

		// Get user input and create message with it  
		if (strlen(message) > 0)
		{
			if(!createMessage(clientIP))
			{
				printf("ERROR: Failed to create message.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
				
				return -1; 
			}

			// Display the message in the Output Window.
			/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/

		}


		
	
	}

}




// ========================================================================================
// functions to be moved to programStart.h
// ========================================================================================


// This function will start the program with initial functions, including the intitial ncurses setup and ">>hi<<" message to the server with initial data 
bool programStart(int argc, char* argv[], bool serverOrIPFlag)
{


	// Parse Arguments into variables for the program 
	if (!parseArgs(argc, argv, serverOrIPFlag))
	{
		printf("ERROR: Failed to parse arguments.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}



	// Resolve the server name to an IP address
	if (serverOrIPFlag == true)
	{
		// Resolve the server name to an IP address
		if (!resolveServerName())
		{
			printf("ERROR: Failed to resolve server name.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
			return false;
		}
	}



	// Create socket
	if (!createSocket())
	{
		printf("ERROR: Failed to create socket.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}

	// Connect to the server
	if (!connectToServer())
	{
		printf("ERROR: Failed to connect to server.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}

	// Initialize the ncurses library
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/

	// Create input window for typing messages 
	// Create output window for displaying chat history 
	/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/


	// Create a thread to handle incoming messages
	incomingThreadCreate();
	outgoingThreadCreate();

	// Get the client IP address 
	if (!getClientIP(clientIP))
	{


		printf("ERROR: Failed to get client IP address.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}


	// Send initial message to the server ( Say >>Hi<< to the server) 
	if (!hiMessage(clientIP))
	{

		printf("ERROR: Failed to send >>Hi<< message to server.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;

	}

	// Display the intiial message in the Output Window.


	return true;
}



// Function to parse the arguments passed to the program
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag)
{


	if (argc != ARG_COUNT)
	{
		printf("\tERROR : Incorrect number of arguments.\n");					// *******TODO******* change usage message to display in the input ncurses window
		printf("\t\tUsage: chat-client –user<userID> –server<server name>\n");
		return false;
	}

	// Loop to parse the arguments passed to the program
	for (int i = 1; i < argc; i++)
	{
		// Check if the argument is a user ID and withing the max length
		if (strncmp(argv[i], "-user", ARG1_SKIP) == 0)
		{
			char userArg = argv[i] + ARG1_SKIP; // Skip the "-user" part of the argument
			strncpy(userID, userArg, MAX_USER_ID);
			userID[MAX_USER_ID] = '\0';
		}
		else if (strncmp(argv[i], "-server", ARG2_SKIP) == 0)	// Check Server Arg for name or IP address 
		{

			char* serverArg = argv[i] + ARG2_SKIP; // Skip the "-server" part of the argument 
			strncpy(serverName, serverArg, MAX_SERVER_NAME);
			serverName[MAX_SERVER_NAME] = '\0';

			// Validate the server arg to check if it's an IP address or a server name, set flag accordingly 
			IPCheck(serverName);

		}
		else
		{
			return false;
		}
	}
	return true;

}



// Validates the server arg to check if it's an IP address or a server name
bool IPCheck(char* serverName)
{
	// Check : Is it a name or IP address?
	int periodCount = 0;
	for (int j = 0; serverName[j] != '\0'; j++)
	{
		if (serverName[j] == '.')
		{
			periodCount++;
		}
	}

	// If exactly 3 periods, assume it's an IP address
	if (periodCount == IP_DOT_COUNT)
	{
		serverOrIPFlag = false;  // It's an IP address
	}
	else
	{
		serverOrIPFlag = true;   // It's a server name
	}

	// Check if the IP address is valid                                          **********TODO**********  If time additional checks for valid IP address (check correct digits present and within valid range)


	return true;
}


// Use getServerName() to accept the server name and it's IP address
bool resolveServerName()
{

	// design your chat-client to accept both a server’s (true) name and also a server’s IP Address as this 'name' command - line argument
			// can verify the (true) name of our computer by looking in the etc/hosts file. 
			// Chances are that this name is tied / bound too the loopback IP address of 127.0.0.1
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


bool getClientIP(char* clientIP)
{
	// Get the client IP address using gethostname() and gethostbyname()
	// If the client IP address is found, return true
	// If the client IP address is not found, return false
	return true;
}



// *********************************************** TO DO ********************************** make sure everything closes properly 
void programEnd()
{

	// Close the connection to the server
	close(socketID);

	// Clean the ncurses windows
	delwin(input_win);
	delwin(output_win);
	endwin();
	/*
__
.--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/

// Exit the program
	printf("Chat - Client is now closing, Goodbye!\n");
	printf("========================================\n");
	// end program 



}



// ========================================================================================
// functions to be moved to UIFunctions.h
// ========================================================================================




bool incomingThreadCreate()
{
	// Create a thread to handle incoming message window
	// If the thread is created, return true
	// If the thread is not created, return false
	return true;
}

bool outgoingThreadCreate()
{
	// Create a thread to handle outgoing message window 
	// If the thread is created, return true
	// If the thread is not created, return false
	return true;
}



bool displayMessage()
{

	// Display the message in the Output Window
	// If the message is displayed, return true
	// If the message is not displayed, return false
	/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/
	return true;
}




// ========================================================================================
// functions to be moved to messageFunctions.h
// ========================================================================================




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






