/* FILE :       
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */



#include "programFunctions.h"
#include "UIFunctions.h"
#include "messageFunctions.h"



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






















