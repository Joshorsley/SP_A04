/* FILE :       
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */



#include "programFunctions.h"
#include "UIFunctions.h"
#include "messageFunctions.h"
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


#define PORT 8000 // port number for the server

 // This function will start the program with initial functions, including the intitial ncurses setup and ">>hi<<" message to the server with initial data 
bool programStart(int argc, char* argv[], bool serverOrIPFlag, int socketID, char serverName)
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
		if (!resolveServerName(serverOrIPFlag, serverName))
		{
			printf("ERROR: Failed to resolve server name.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
			return false;
		}
	}



	// Create socket
	if (!createSocket(socketID))
	{
		printf("ERROR: Failed to create socket.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}

	// Connect to the server
	if (!connectToServer(socketID))
	{
		printf("ERROR: Failed to connect to server.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
		return false;
	}

	// Initialize the ncurses library 
	/*
 __
.--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/

// Create input window for typing messages 
/*
 __
.--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/

// Create output window for displaying messages
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
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag, char userID, char serverName)
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
			// If it is an IP address, then set the server address to the variable for the server address
			if (IPCheck(serverName) == true)
			{
				strncpy(serverAddress, serverName, MAX_IP);
				serverAddress[MAX_IP] = '\0';
				serverOrIPFlag = false;
			}
			else
			{
				serverOrIPFlag = true;
			}

	

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


/*
* FUNCTION:		resolveServerName
* DESCRIPTION:	This function resolves the server name to an IP address
* PARAMETERS:	None
* RETURNS:		boolean value, true if the server name is successfully resolved, false otherwise
*/
bool resolveServerName(bool serverOrIPFlag, char serverName)
{

	struct hostent* host_entry;
	struct in_addr** addr_list;

	// If IPcheck set to false, then it's already an IP address and this function is not needed
	if (!serverOrIPFlag) 
	{
		return true;
	}

	// Try to resolve the hostname
	host_entry = gethostbyname(serverName);
	if (host_entry == NULL) 
	{
		printf("ERROR: host entry error when attempting to resolve serverName.\n");
		return false;
	}

	// Get the first IP address from the list
	addr_list = (struct in_addr**)host_entry->h_addr_list;
	if (addr_list[0] == NULL) 
	{
		printf("ERROR: No IP addresses found for hostname.\n");
		return false;
	}

	// Convert to string and store in serverAddress for use in the program 
	strncpy(serverAddress, inet_ntoa(*addr_list[0]), MAX_IP);
	serverAddress[MAX_IP - 1] = '\0'; 

	return true;
}



bool createSocket(int socketID)
{

	socketID = socket(AF_INET, SOCK_STREAM, 0); // create socket

	if (socketID == -1)
	{
		printf("ERROR: Failed to create socket.\n");
		return false;
	}

	// Set up the server address structure
	struct sockaddr_in serv_addr;

	// Zero out the structure preventing garbage values 
	memset(&serv_addr, 0, sizeof(serv_addr));

	// Set the server address family to IPv4
	serv_addr.sin_family = AF_INET;

	// set server IP address
	serv_addr.sin_addr.s_addr = inet_addr(serverAddress);

	// set up the port number
	serv_addr.sin_port = htons(PORT);	

	return true; 


}


bool connectToServer(int socketID)
{
	// Connect to the server using the socket struct created in createSocket()
	if (!connect(socketID, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
	{

		return false; 

	}

	return true;

}

/*
* FUNCTION:		getClientIP
* DESCRIPTION:	This function gets the client's IP address and stores it in the clientIP variable
* PARAMETERS:	char* clientIP - the client's IP address
* RETURNS:		boolean value, true if the client's IP address is successfully retrieved, false otherwise
*/
bool getClientIP(char* clientIP)
{
	char hostname[256];
	struct hostent* host_entry;				// pointer to an entry in the hosts database

	// Get the system hostname
	if (gethostname(hostname, sizeof(hostname)) == -1)
	{
		printf("ERROR: Failed to get hostname.\n");
		return false;
	}

	// Get the host information from the hostname, store it in the host_entry struct
	host_entry = gethostbyname(hostname);
	if (host_entry == NULL)
	{
		printf("ERROR: Failed to get host by name.\n");
		return false;
	}

	// get the IP address from the struct and store it in the clientIP variable for use in the message functions
	strcpy(clientIP, inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
	return true;
}



// *********************************************** TO DO ********************************** make sure everything closes properly 
void programEnd(int socketID)
{

	// Close the connection to the server
	close(socketID);

	// Clean the ncurses windows

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






















