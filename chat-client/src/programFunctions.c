/* FILE :       messageFunctions.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the functions related to creating, sending and recieving messages.
 */


#include <netdb.h>
#include "programFunctions.h"
#include "UIFunctions.h"
#include "messageFunctions.h"
#include "main.h"



#define PORT 8000 // port number for the server


//GOOD
// Function to parse the arguments passed to the program
bool parseArgs(int argc, char* argv[], char* userID, char* serverName)
{
	bool userExists = false;
	bool serverExists = false;

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
			strncpy(userID, argv[i] + ARG1_SKIP, MAX_USER_ID - 1); // Skip the "-user" part of the argument
			userID[MAX_USER_ID - 1] = '\0';
			userExists = true;

		}
		else if (strncmp(argv[i], "-server", ARG2_SKIP) == 0)	// Check Server Arg for name or IP address 
		{
			strncpy(serverName, argv[i] + ARG2_SKIP, MAX_SERVER_NAME - 1); // Skip the "-server" part of the argument
			serverName[MAX_SERVER_NAME - 1] = '\0';
			serverExists = true;
		}
		else
		{
			printf("\tERROR : Invalid argument: %s\n", argv[i]);					// *******TODO******* change usage message to display in the input ncurses window
			printf("\t\tUsage: chat-client –user<userID> –server<server name>\n");
			return false;
		}
	}

	if (userExists && serverExists)
	{
		return true;
	}


	return false;

}


//MEH - might need the ip validation check
// Validates the server arg to check if it's an IP address or a server name
// bool IPCheck(const char* serverName)
// {
// 	// Check : Is it a name or IP address?
// 	int periodCount = 0;
// 	for (int j = 0; serverName[j] != '\0'; j++)
// 	{
// 		if (serverName[j] == '.')
// 		{
// 			periodCount++;
// 		}
// 	}

// 	// If exactly 3 periods, assume it's an IP address
// 	if (periodCount == IP_DOT_COUNT)
// 	{
// 		serverOrIPFlag = false;  // It's an IP address
// 	}
// 	else
// 	{
// 		serverOrIPFlag = true;   // It's a server name
// 	}

// 	// Check if the IP address is valid                                          **********TODO**********  If time additional checks for valid IP address (check correct digits present and within valid range)



// 	return true;
// }
//REWRITE OF ABOVE
bool isAddress(const char* serverName)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, serverName, &(sa.sin_addr));
	return result != 0;
}


/*
* FUNCTION:		resolveServerName
* DESCRIPTION:	This function resolves the server name to an IP address
* PARAMETERS:	None
* RETURNS:		boolean value, true if the server name is successfully resolved, false otherwise
*/
bool resolveServerName(ClientInfo *clientInfo)
{
    struct hostent *host_entry;
    host_entry = gethostbyname(clientInfo->serverName);
    if (host_entry == NULL)
    {
        printf("ERROR: Failed to get host by name.\n");
        return false;
    }
    
    // Convert the IP address to a string and store it in the serverAddress variable
    strcpy(clientInfo->serverAddress, inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
    return true;
}



// REWROTE AND COMBINED THE TWO ABOVE FUNCTIONS TO NOW CREATE SOCKET AND CONNECT
//REWRITE OF ABOVE
int connectToServer(ClientInfo *clientInfo) 
{
    int socketID;
    struct sockaddr_in server_addr;

    // Create socket
    socketID = socket(AF_INET, SOCK_STREAM, 0); // create socket
    if (socketID < 0) 
    {
        perror("Error: could not create socket");
        return -1;
    }

    // Zero out the structure to prevent garbage values
    memset(&server_addr, 0, sizeof(server_addr));

    // Set the server address family to IPv4
    server_addr.sin_family = AF_INET;

    // Check if serverName is an IP address or hostname
    if (isAddress(clientInfo->serverName)) 
    {
        // If it's an IP address, copy it directly to serverAddress
        strncpy(clientInfo->serverAddress, clientInfo->serverName, MAX_IP - 1);
        clientInfo->serverAddress[MAX_IP - 1] = '\0';
    } 
    else 
    {
        // Resolve the server name to an IP address
        if (!resolveServerName(clientInfo)) 
        {
            printf("Error: Failed to resolve server name: %s\n", clientInfo->serverName);
            close(socketID);
            return -1;
        }
    }

    // Set the server IP address
    server_addr.sin_addr.s_addr = inet_addr(clientInfo->serverAddress);

    // Set up the port number
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(socketID, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error: Connect failed");
        close(socketID);
        return -1;
    }

    return socketID;
}

/*
* FUNCTION:		getClientIP
* DESCRIPTION:	This function gets the client's IP address and stores it in the clientIP variable
* PARAMETERS:	char* clientIP - the client's IP address
* RETURNS:		boolean value, true if the client's IP address is successfully retrieved, false otherwise
*/
// bool getClientIP(char* clientIP)
// {
// 	char hostname[256];
// 	struct hostent* host_entry;				// pointer to an entry in the hosts database

// 	// Get the system hostname
// 	if (gethostname(hostname, sizeof(hostname)) == -1)
// 	{
// 		printf("ERROR: Failed to get hostname.\n");
// 		return false;
// 	}

// 	// Get the host information from the hostname, store it in the host_entry struct
// 	host_entry = gethostbyname(hostname);
// 	if (host_entry == NULL)
// 	{
// 		printf("ERROR: Failed to get host by name.\n");
// 		return false;
// 	}

//     return true;
// }





/*
* FUNCTION:
* DESCRIPTION: This function recieves the message from the server and displays it in the incoming messages window
* 			 The function will return true if the message is successfully received, and false otherwise
*                 ************************************************************************************************************ TO DO ********** This function needs to be finished and tested
*/
// bool receiveMessage(char* clientIP, char* userID)
// {
//     // Receive a message from the server 
//     char* buffer = (char*)calloc(MAX_BUFFER, sizeof(char));
//     read(socketID, buffer, MAX_BUFFER);
//     // Messages will be sent from the server in 40 character parcel chunks to stitch together 
//     // Display the received message in the Incoming Messages  Window

//     // If the message is received, return true
//     // If the message is not received, return false

//     // if a message is received when the user is typing another message, it MUST not interrupt the message being currently composed
//     // Each message being received within a client needs to contain the sending client’s IP address - compare to ensure right client message 
//     // Each message being received within a client needs to contain the sending client’s user-name (up to 5 characters) - compare to ensure right client message

//     return true;
// }
