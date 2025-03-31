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



#define PORT 8000 


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
        return false;
    }
    
    // Convert the IP address to a string and store it in the serverAddress variable
    strcpy(clientInfo->serverAddress, inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
    return true;
}


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
        //perror("Error: Connect failed");
        close(socketID);
        return -1;
    }

    return socketID;
}