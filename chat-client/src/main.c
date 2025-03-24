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
#include "ncurses-samples/src/ncurses-01.c"
#include "ncurses-samples/src/ncurses-02.c"
#include "ncurses-samples/src/ncurses-03.c"

#define ARG_COUNT 3
#define MAX_USER_ID 5
#define MAX_MESSAGE 80
#define MAX_SERVER_NAME 255
#define MAX_MESSAGE 255


 // Function Prototypes (to split into new files) 

	// startProgram.h
bool programStart(int argc, char* argv[])
bool parseArgs(int argc, char* argv[]);
bool getServerName();
bool createSocket();
bool connectToServer();
// windowFunctions.h
bool incomingThreadCreate(); 
bool outgoingThreadCreate(); 
// messageFunctions.h
bool createMessage();
bool recieveMessage();
bool displayMessage();








/*

Notes : 
 -  in the chat-client, do not need to worry about dealing with / handling the delete, backspace or arrow keys
 -  no debugging messages being printed to the screen in your final client

 - Incoming Message Window: 
		- Should display each of the messages sent through the serverby this client or others (excluding >>hi<< or >>bye<< messages)
		- should be written in a specific format ( see function notes for more details)
		-  should be able to show the history of at most the last 10 “lines” from the messages sent and received. 
		- HINT: The fact that there are starting and stopping positions for fields in the message output should indicate the potential solution for you 

 - Outgoing Message Window 
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
	int socketID;						// Socket ID
	WINDOW* input_win, * output_win;    // ncurses windows for input and output
	char message[MAX_MESSAGE];           // Buffer for user messages



	printf("Welcome to the Chat - Client Terminal\n");

	if (argc != ARG_COUNT)
	{
		printf("\tERROR : Incorrect number of arguments.\n");
		printf("\t\tUsage: chat-client –user<userID> –server<server name>\n");
		return 0;
	}

	// Start the program with initial functions 
	programStart(); 



	// Loop for User Input 
	While(true)
	{
		// Clear the input window


		// Read user input from the Input Window (up to 80 characters)

		// Display the message in the Output Window.



		// If the user presses enter: 
		if ()
		{
			// Send message to the server using send().
			// Display the message in the Output Window.
		}


		// If the message is ">>bye<<",
		if ()
		{
			// Close the connection to the server
			close(socketID);

			// Clean the ncurses windows


			// Exit the program
			printf("Chat - Client is now closing, Goodbye!\n");
			return 0;
		}
	}

}




// ========================================================================================
// functions to be moved to programStart.h
// ========================================================================================


// This function will start the program with initial functions, including the intitial ncurses setup and ">>hi<<" message to the server with initial data 
bool programStart(int argc, char* argv[])
{


	// Parse Arguments into variables for the program 
	if (!parseArgs(argc, argv))
	{
		printf("ERROR: Failed to parse arguments.\n");
		return -1;
	}



	// Resolve the server name to an IP address
	if (!getServerName())
	{

		printf("ERROR: Failed to resolve server name.\n");
		return -1;
	}


	// Get User ID from user 
		//	make sure to handle duplicate user IDs if the server notes the user ID is taken during the 'hello' exchange 


	// Create socket
	if (!createSocket())
	{
		printf("ERROR: Failed to create socket.\n");
		return 0;
	}

	// Connect to the server
	if (!connectToServer())
	{
		printf("ERROR: Failed to connect to server.\n");
		return 0;
	}

	// Initialize the ncurses library


	// Create input window for typing messages 
	// Create output window for displaying chat history 



	// Create a thread to handle incoming messages
	incomingThreadCreate();
	outgoingThreadCreate(); 

	// Send initial message to the server ( Say >>Hi<< to the server) 
				//	(include : messageType (in this case "start" | username | content of message)
				// 
				//	send the message to the server using send() 
				
	// Display the intiial message in the Output Window.



}



// Function to parse the arguments passed to the program
bool parseArgs(int argc, char* argv[])
{
	/*
	************* TO DO *************
	To be clear then about the chat-client application’s -server command-line argument – 
	you need to be able to handle and support the server’s (true) name as well as the server’s IP Address. 
			For example:
					chat-client –userSean –serverubuntu or
					chat-client –userSean –server192.168.244.128
	
	*/



	// Loop to parse the arguments passed to the program
	for (int i = 1; i < argc; i++)
	{
		// Check if the argument is a user ID and withing the max length
		if (strncmp(argv[i], "-user", 5) == 0)
		{
			strncpy(userID, argv[i] + 5, MAX_USER_ID);
			userID[MAX_USER_ID] = '\0';
		}
		else if ()	// Check Server Arg for name or IP address 
		{
	
		}
		else
		{
			return false;
		}
	}
	return true;

}




// Should actually be able to fold this into parseArgs in the section that parses server details ***************** POSSIBLE DESIGN CHANGE NOTE ***********
bool getServerName()
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







// ========================================================================================
// functions to be moved to windowFunctions.h
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








// ========================================================================================
// functions to be moved to messageFunctions.h
// ========================================================================================



bool createMessage()
{
	// Create a message to send to the server
		// If the message is created, return true
		// If the message is not created, return false

	//the user should be allowed to enter a message of up to 80 characters.
		//When the user hits the 80 character input boundary – the UI must stop accepting characters for input with no other notification to the user.

	// if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed

	// 3 types of messages that can be sent: 
		//">>hi<<" = initial message to server and only for the server, contains client IP and client’s userID, where the server will establish the connection and ensure the userID is unique, if not the user will have to send a new hi message. 
		//"message" = regular message type being sent to other clients(server will package into chunks and send to other clients)
		//">>bye<<" = message to server and only for the server, to close connection and exit program



	// Message Format: 
	
		// XXX.XXX.XXX.XXX_[AAAAA]_>>_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa_(HH:MM:SS)
		// --- IP ADDR --- -USER ---------------MESSAGE------------------ --TIME--

		// Should display the IP address of the message’s source (i.e. the IP address of the client sending it)
				//• in positions 1 through 15 of the output line
				
		//• Positions 16, 24, 27 and 68 must be spaces
		
		//• Should show the name of the person sending the message(max 5 characters) enclosed in square brackets(“[“ and “]”)
				//• in positions 17 through 23 (including the brackets)
				
		//• Should show the directionality of the message(i.e. >> for outgoing, << for incoming)
				//• in positions 25 and 26
				//• you should see >> on your client if you sent the message
				//• you should see << on your client if the message came from another client
				
		//• Should show the actual message(again max 40 characters)
				//• in positions 28 to 67
				
		//• Should show the a 24 - hour clock received timestamp on the message enclosed in round brackets(“(“ and “)”)
				//• in positions 69 to 78 (including the brackets)
				//• this should reflect the time that the client received the message from the server





	return true;
}




bool recieveMessage()
{
	// Receive a message from the server using recv()
	// Messages will be sent from the server in 40 character parcel chunks to stitch together 
	// Display the recieved message in the Incoming Messages  Window
	
	// If the message is received, return true
	// If the message is not received, return false

	// if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed
	// Each message being received within a client needs to contain the sending client’s IP address. 
	//Each message being received within a client needs to contain the sending client’s user-name (up to 5 characters)

	return true;
}



bool displayMessage()
{
	// Display the message in the Output Window
	// If the message is displayed, return true
	// If the message is not displayed, return false
	return true;
}
