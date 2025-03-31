/* FILE :       programFunctions.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :	Kalina cathcart, Josh Horsley, Jon Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the functions related to managing the program and the connection to the server. 
 */


#include <netdb.h>
#include "programFunctions.h"
#include "UIFunctions.h"
#include "messageFunctions.h"
#include "main.h"

#define PORT 8000 

/*
 * FUNCTION : 	 parseArgs 
 * DESCRIPTION : This function takes the arguments passed from the command line and places them into their related variables. 
 * 		 The user ID is the first argument, which will be the name given to the server to identify this particular client connection. 
 * 		 The server name or IP address is the second argument. Either or is stored inthe serverName variable for further processing. 
 * PARAMETERS :  argc and argv = variables carrying the command line arguments and number of 
 * 		 userID = holds the name chosen by the user to be used to identify this client's connection to the server. 
 * 		 serverName = holds the server name or IP address given from the command line for further processing. 
 * RETURNS : 	 True if both userID and serverName arguments are present, false if one or both are missing or could not be parsed. 
 */
bool parseArgs(int argc, char* argv[], char* userID, char* serverName)
{
	bool userExists = false;
	bool serverExists = false;

	if (argc != ARG_COUNT)
	{
		return false;
	}

	
	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-user", ARG1_SKIP) == 0)
		{
			strncpy(userID, argv[i] + ARG1_SKIP, MAX_USER_ID - 1);
			userID[MAX_USER_ID - 1] = '\0';
			userExists = true;
		}
		else if (strncmp(argv[i], "-server", ARG2_SKIP) == 0)
		{
			strncpy(serverName, argv[i] + ARG2_SKIP, MAX_SERVER_NAME - 1); 
			serverName[MAX_SERVER_NAME - 1] = '\0';
			serverExists = true;
		}
		else
		{
			return false;
		}
	}

	if (userExists && serverExists)
	{
		return true;
	}

	return false;
}


/*
 * FUNCTION :	 isAddress 
 * DESCRIPTION : This function will check if the address is a IP address.
 * PARAMETERS :  serverName - to pass the value to be checked 
 * RETURNS : 	 True if the string is a valid IP address, false otherwise. 
 *
 */
bool isAddress(const char* serverName)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, serverName, &(sa.sin_addr));
	return result != 0;
}


/*
* FUNCTION:	resolveServerName
* DESCRIPTION:	This function resolves the server name to an IP address. 
* 		It will convert the found IP address to a string and store it in the serverAddress variable
* PARAMETERS:	client info - struct holding the servername value within 
* RETURNS:	Truerue if the server name is successfully resolved, false otherwise
*/
bool resolveServerName(ClientInfo *clientInfo)
{
    struct hostent *host_entry;
    host_entry = gethostbyname(clientInfo->serverName);
    if (host_entry == NULL)
    {
        return false;
    }
    
    strcpy(clientInfo->serverAddress, inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
    return true;
}

/*
* FUNCTION:     connectToServer
* DESCRIPTION:  Establishes a TCP connection through sockets to the specified server using the server address variable.
* 		isAddress is called here to determine if the server address variable is a server name or IP address. 
* 		If it is a server name, resolveServerName is called to find the related IP address.  
* PARAMETERS:   ClientInfo *clientInfo - pointer to ClientInfo structure with connection details
* RETURNS:      int - socket file descriptor if successful, -1 on error
*/
int connectToServer(ClientInfo *clientInfo) 
{
    int socketID;
    struct sockaddr_in server_addr;

    socketID = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketID < 0) 
    {
        perror("Error: could not create socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;

    if (isAddress(clientInfo->serverName)) 
    {
        strncpy(clientInfo->serverAddress, clientInfo->serverName, MAX_IP - 1);
        clientInfo->serverAddress[MAX_IP - 1] = '\0';
    } 
    else 
    {
        if (!resolveServerName(clientInfo)) 
        {
            close(socketID);
            return -1;
        }
    }

    server_addr.sin_addr.s_addr = inet_addr(clientInfo->serverAddress);

    server_addr.sin_port = htons(PORT);

    if (connect(socketID, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        close(socketID);
        return -1;
    }

    return socketID;
}
