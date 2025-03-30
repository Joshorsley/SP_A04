/* FILE :       programFunctions.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the functions that handle beginning the program and initializing the connection with the server. 
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

#define PORT 8000 		// port number for the server
#define ARG_COUNT 3		// expected arg count
#define MAX_USER_ID 5		// max user ID length
#define MAX_SERVER_NAME 255 	// max server name length
#define ARG1_SKIP 5 		// skip the first 5 characters of the argument
#define ARG2_SKIP 7 		// skip the first 7 characters of the argument
#define IP_DOT_COUNT 3 		// number of periods in an IP address
#define MAX_IP 16 		// max IP address size



/*
* FUNCTION:         programStart
* DESCRIPTION:      This function starts the program by doing the following: 
                        1) parses the  userID and serverAddress or serverName variables from the arguments given in the command line 
                        2) If a name was given for the server instead of an IP address, the program resolves the server name to an IP address
                        3) creates a socket
		       	4) connects to the server using the socket and found or given 1P address for the server
                        5) initializes the ncurses library
			6) Uses Ncurses to initialize input and output windows                                                                          **********TODO************  Update when ncurses and thread functionality complete in case of changes 
			7) Creates 2 threads, one to handle incoming messages, one to handle outgoing messages
                        8) gets the client IP address to add to the messages
                        9) sends the initial >>hi<< message to the server
		    When the functionality is complete, it will return true if all steps are successful, false otherwise. 
                    It will go back to main to continue to the chat loop if successful, or end the program if unsuccessful.
*/
bool programStart(int argc, char* argv[], bool serverOrIPFlag, int socketID, char* serverName, char* userID, char* clientIP, char* serverAddress, char* timestamp, char* message, bool programEndFlag) 
{
    // Parse Arguments into variables for the program 
    if (!parseArgs(argc, argv, serverOrIPFlag, userID, serverName, serverAddress)) 
    {
        printf("ERROR: Failed to parse arguments.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
        return false;
    }

    // Check: If the server or IP flag is true, then it is a server name and needs to be resolved to find the address
    if (*serverOrIPFlag == true) 
    {
        // Resolve the server name to an IP address
        if (!resolveServerName(serverOrIPFlag, serverName, serverAddress)) 
        {
            printf("ERROR: Failed to resolve server name.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
            return false;
        }
    }

    // Create socket with the found or given server IP address
    if (!createSocket(socketID)) 
    {
        printf("ERROR: Failed to create socket.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
        return false;
    }

    // Connect to the server
    if (!connectToServer(socketID, serverAddress)) 
    {
        printf("ERROR: Failed to connect to server.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
        return false;
    }

    // Initialize the ncurses library 
    /*
 __
.--()°'./
'|, . ,'        NCurses Be needed here
 !_-(_\

    */

    // Create input window for typing messages 
    /*
 __
.--()°'./
'|, . ,'        NCurses Be needed here
 !_-(_\

    */

    // Create output window for displaying messages
    /*
 __
.--()°'./
'|, . ,'        NCurses Be needed here
 !_-(_\

    */

    // Create a thread to handle incoming messages
    //
    //
    //

    // Get the client IP address 
    if (!getClientIP(clientIP)) 
    {
        printf("ERROR: Failed to get client IP address.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
        return false;
    }

    // Send initial message to the server (Say >>Hi<< to the server) 
    if (!hiMessage(message, clientIP, programEndFlag, userID, timestamp, socketID)) 
    {
        printf("ERROR: Failed to send >>Hi<< message to server.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
        return false;
    }

    // Display the initial message in the Output Window.

    return true;
}





/*
* FUNCTION:             parseArgs
* DESCRIPTION:          This function parses the arguments passed to the program and stores the user ID and the server name or server address in their variables.
						It also sets the serverOrIPFlag to true if the server name was given, and false if the server address was given.
						It returns true if the arguments are successfully parsed, and false otherwise.
*/
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag, char* userID, char* serverName, char* serverAddress) 
{
    if (argc != ARG_COUNT) 
    {
        printf("\tERROR : Incorrect number of arguments.\n");                    // *******TODO******* change usage message to display in the input ncurses window
        printf("\t\tUsage: chat-client –user<userID> –server<server name>\n");
        return false;
    }

    // Loop to parse the arguments passed to the program
    for (int i = 1; i < argc; i++) 
    {
        // Check if the argument is a user ID and within the max length
        if (strncmp(argv[i], "-user", ARG1_SKIP) == 0) 
        {
            char* userArg = argv[i] + ARG1_SKIP; // Skip the "-user" part of the argument
            strncpy(userID, userArg, MAX_USER_ID);
            userID[MAX_USER_ID] = '\0';
        }
        else if (strncmp(argv[i], "-server", ARG2_SKIP) == 0)  // Check Server Arg for name or IP address 
        {   
            char* serverArg = argv[i] + ARG2_SKIP; // Skip the "-server" part of the argument 
            strncpy(serverName, serverArg, MAX_SERVER_NAME);
            serverName[MAX_SERVER_NAME] = '\0';

            // Validate the server arg to check if it's an IP address or a server name, set flag accordingly 
            // If it is an IP address, then set the server address to the variable for the server address
            if (IPCheck(serverName, serverOrIPFlag)) 
            {
                strncpy(serverAddress, serverName, MAX_IP);
                serverAddress[MAX_IP] = '\0';
                *serverOrIPFlag = false;
            }
            else 
            {
                *serverOrIPFlag = true;
            }
        }
        else 
        {
            return false;
        }
    }
    return true;
}




/*
* FUNCTION:             IPCheck
* DESCRIPTION:      This function validates the server arg to check if it's an IP address or a server name.
* 
*/
bool IPCheck(char* serverName, bool* serverOrIPFlag) 
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
        *serverOrIPFlag = false;  // It's an IP address
    }
    else 
    {
        *serverOrIPFlag = true;   // It's a server name
    }

    // Check if the IP address is valid                                                   **********TODO**********  If time additional checks for valid IP address (check correct digits present and within valid ranges)

    return true;
}



/*
* FUNCTION:             resolveServerName
* DESCRIPTION:  This function resolves the server name to an IP address
* PARAMETERS:   None
* RETURNS:              boolean value, true if the server name is successfully resolved, false otherwise
*/

bool resolveServerName(bool* serverOrIPFlag, char* serverName, char* serverAddress) 
{
    struct hostent* host_entry;
    struct in_addr** addr_list;

    // If IPCheck set to false, then it's already an IP address and this function is not needed
    if (!(*serverOrIPFlag)) 
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


/*
 * FUNCTION : createSocket
 * DESCRIPTION : creates socket for program to use 
 */
bool createSocket(int socketID) 
{
    *socketID = socket(AF_INET, SOCK_STREAM, 0); // create socket

    if (*socketID == -1) 
    {
        printf("ERROR: Failed to create socket.\n");
        return false;
    }

    return true;
}





/*
* FUNCTION:             connectToServer
* DESCRIPTION:      This function connects the client to the server using the socket and server address.
* 
*/
bool connectToServer(int socketID, char* serverAddress) 
{
    struct sockaddr_in serv_addr;

    // Configure the server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);  

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, serverAddress, &serv_addr.sin_addr) <= 0) 
    {
        printf("ERROR: Invalid address / Address not supported\n");
        close(socketID);
        return false;
    }

    // Connect to server
    if (connect(socketID, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("ERROR: Failed to connect to server\n");
        close(socketID);
        return false;
    }

    return true;
}



/*
* FUNCTION:             getClientIP
* DESCRIPTION:  This function gets the client's IP address and stores it in the clientIP variable
* PARAMETERS:   char* clientIP - the client's IP address
* RETURNS:              boolean value, true if the client's IP address is successfully retrieved, false otherwise
*/

bool getClientIP(char* clientIP) 
{
    char hostname[256];
    struct hostent* host_entry;             // pointer to an entry in the hosts database

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



/*
 * FUNCTION:    programEnd
 * DESCRIPTION: ends the program gracefully, ensuring all components are closed properly 
 *
 */

void programEnd(int socketID, WINDOW* inWin, WINDOW* outWin) 
{
    // Close the connection to the server
    close(socketID);

    // Clean the ncurses windows
    endProg(inWin, outWin);

    // Exit the program
    printf("Chat - Client is now closing, Goodbye!\n");
    printf("========================================\n");
}



