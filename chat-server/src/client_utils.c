/*
* FILE : client_utils.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION : This file holds the functions related to handling the incoming and outgoing messages between clients and the server. 
*		
*
*  		
* FUNCTIONS:
*
*   clientHandler        - Main thread function for handling client connections
*   broadcastMessage     - Broadcasts a message to all connected clients
*   logClientConnection  - Logs new client connection information
*   registerUsername     - Registers and validates client usernames
*   announceUserJoined   - Announces new user to all clients
*   processClientMessages- Main loop for receiving and processing client messages
*   handleClientDisconnect- Handles client disconnection cleanup
*   disconnectClient     - Cleans up client resources and updates server state
*   formatAndSendMessage - Formats messages with metadata and timestamps
*   parcelMessage        - Breaks large messages into chunks for transmission
*   isUsernameTaken      - Checks if a username is already in use
*
*/

#include "client_utils.h"
#include "server_utils.h"



/*
* FUNCTION:     clientHandler
* DESCRIPTION:  Main thread function that manages the lifecycle of a client connection.
*               It will take the client index from the passed arg pointer and logs the connection to the client.
*               It checks and registers the username if valid, if not it will disconnect. 
*               If the user does connect, a notification of the user's connection will broadcast to the other clients. 
*               This function then will process incoming messges until the client disconnects. 
*               When the client disconnects it will trigger the function to to handle the disconnection cleanly and end. 
* PARAMETERS:   void* arg - Pointer to client's index in the global clients array
* RETURNS:      NULL
*/
void* clientHandler(void* arg) {

    int index = *((int*)arg);
    free(arg);

    int clientSocket = clients[index].socket;
    
    logClientConnection(index);

    if(!registerUsername(index)) {
        return NULL;
    }

    announceUserJoined(index);

    processClientMessages(index);

    handleClientDisconnect(index);

    return NULL;
}



/*
* FUNCTION:     broadcastMessage
* DESCRIPTION:  Sends a message to all connected clients with proper formatting.
* 		If the message is being returned to the sender, it will be prefixed with >>, otherwise other clients will get the prefix of <<.
* 		Each message will include the sender's IP and and username within. 
*               The function uses mutex to ensure thread-safe access to clients array.
* PARAMETERS:   char *message - The message content to broadcast
*               int senderIndex - Index of the sending client in clients array
* RETURNS:      void
*/
void broadcastMessage(char *message, int senderIndex) {
    pthread_mutex_lock(&clients_mutex);

    // Get the senders IP address as a string
    char senderIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clients[senderIndex].address.sin_addr), senderIP, INET_ADDRSTRLEN);

    printf("Broadcasting message from %s: %s\n", clients[senderIndex].username, message);

    // Send to all connected clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            // For the sender, use '>>' for others use '<<'
            char direction = (i == senderIndex) ? '>' : '<';

            parcelMessage(clients[i].socket, senderIP, clients[senderIndex].username, message, direction);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}




/*
* FUNCTION:     logClientConnection
* DESCRIPTION:  Logs client connection information to server console.
* PARAMETERS:   int index - Client index in global clients array
* RETURNS:      void
*/
void logClientConnection(int index) {
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clients[index].address.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Handling client %d with IP %s \n", index, clientIP);
}




/*
* FUNCTION:     registerUsername
* DESCRIPTION:  Registers and validates a client's username.
*               Ensures the first message from client is only the username.
*               If the username is empty, or already taken, the connection is not registered and disconnected. 
*               If the username is too long, it will be cut to length. 
* PARAMETERS:   int index - Client index in global clients array
* RETURNS:      True if registration successful, false if invalid/disconnected
*/
bool registerUsername(int index) {
    int clientSocket = clients[index].socket;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    
    // First message from client should be the username
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        printf("Client disconnected before sending username\n");
        disconnectClient(index);
        return false;
    }
    
    //NULL-terminate the received data
    buffer[bytesRead] = '\0';
    
    if (isUsernameTaken(buffer, index)) {
        printf("Client %d attempted to use taken username: %s\n", index, buffer);
        
        // Send rejection message to client
        const char *msg = "Sorry, that username is already taken. Please reconnect with a different username.\n";
        send(clientSocket, msg, strlen(msg), 0);
        disconnectClient(index);
        return false;
    }
    
    // Store the username 
    pthread_mutex_lock(&clients_mutex);
    strncpy(clients[index].username, buffer, MAX_NAME_LENGTH - 1);
    clients[index].username[MAX_NAME_LENGTH - 1] = '\0';
    pthread_mutex_unlock(&clients_mutex);
    
    printf("Client %d registered with username: %s\n", index, clients[index].username);
    return true;
}



/*
* FUNCTION:     announceUserJoined
* DESCRIPTION:  Broadcasts notification to all clients that a new user has joined the chat.
* PARAMETERS:   int index - Client index of new user
* RETURNS:      void
*/
void announceUserJoined(int index) {
    char announcement[BUFFER_SIZE];
    snprintf(announcement, sizeof(announcement), "User %s has joined the chat", clients[index].username);
    broadcastMessage(announcement, index);
}





/*
* FUNCTION:     processClientMessages
* DESCRIPTION:  Main message processing loop for a client connection.
* 		This function will continually recieve messages from the client socket until disconnection is signalled with a >>bye<< message from the client.
* 		If nothing is read, the disconnection will be announced. 
* 		Any valid message received is broadcasted to all clients. 
* PARAMETERS:   int index - Client index to process messages for
* RETURNS:      void
*/
void processClientMessages(int index) {
    int clientSocket = clients[index].socket;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    
    // Main message loop - receive and process messages
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        // NULL-terminate the received data
        buffer[bytesRead] = '\0';
        printf("Received from %s: %s\n", clients[index].username, buffer);
        
        // Check if client wants to disconnect
        if (strcmp(buffer, ">>bye<<") == 0) {
            printf("Client %s is disconnecting\n", clients[index].username);
            break;
        }
        
        // Process and broadcast the message
        broadcastMessage(buffer, index);
    }
    
    if (bytesRead == 0) {
        printf("Client %s closed connection\n", clients[index].username);
    } else if (bytesRead < 0) {
        printf("Error receiving from client %s\n", clients[index].username);
    }
}

/*
* FUNCTION:     handleClientDisconnect
* DESCRIPTION:  Handles the complete disconnection process for a client
*               It will announce the client's departure to other clients
*               It will then  clean up related resources and disconnect with disconnectClient()
* PARAMETERS:   int index - Index of disconnecting client in global clients array
* RETURNS:      void
*/
void handleClientDisconnect(int index) {
    // Announce that the user has left
    char announcement[BUFFER_SIZE];
    snprintf(announcement, sizeof(announcement), "User %s has left the chat", clients[index].username);
    broadcastMessage(announcement, index);
    
    disconnectClient(index);
}



/*
* FUNCTION:     disconnectClient
* DESCRIPTION:  Performs actual client disconnection tasks. 
*               It will close the socket related to the client
*               It will then mark the client's old slot as inactive and decrement the client count
*               It then checks if server should shutdown with checkForServerShurdown()
*		Uses mutex lock to protect shared clients array and clientCount
* PARAMETERS:   int index - Index of client to disconnect
* RETURNS:      void
*/
void disconnectClient(int index) {
    // Lock the mutex before updating shared data
    pthread_mutex_lock(&clients_mutex);
    
    // Clean up and mark slot as inactive
    close(clients[index].socket);
    clients[index].active = 0;
    clientCount--;
    checkForServerShutdown();
    
    pthread_mutex_unlock(&clients_mutex);
    printf("Client %d cleaned up, total clients: %d\n", index, clientCount);
}

/*
* FUNCTION:     formatAndSendMessage
* DESCRIPTION:  Formats a message with metadata and timestamp according to requirements provided:
*               "IP_address [user] >> message (timestamp)"
* PARAMETERS:   int receiverSocket - Destination socket
*               char* senderIP - IP address of message sender
*               char* senderName - Username of sender
*               char* message - The message content
*               char direction - '>' for sender, '<' for others
* RETURNS:      void
*/
void formatAndSendMessage(int receiverSocket, char* senderIP, char* senderName, char* message, char direction) {
    char formattedMsg[BUFFER_SIZE];
    time_t now;
    struct tm *timeinfo;
    char timestamp[10];

    // Get current time
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);

    // Format according to requirements:
    // XXX.XXX.XXX.XXX_[AAAAA]_>>_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa_(HH:MM:SS)
    snprintf(formattedMsg, sizeof(formattedMsg),
            "%-15s [%-5s] %c%c %-40s (%s)\n",
            senderIP, senderName, direction, direction, message, timestamp);
    send(receiverSocket, formattedMsg, strlen(formattedMsg), 0);
}



/*
* FUNCTION:     parcelMessage
* DESCRIPTION:  Breaks large messages into chunks for network transmission.
* 		Chunking: 
* 			- breaks longer messages into CHUNK_SIZE (40 char) sized messages 
* 			- will break at space characters when possible
* 			- makes sure chunks are null-terminated 
* 			- calls formatAndSendMessage for each chunk
* PARAMETERS:   int receiverSocket - Destination socket
*               char* senderIP - Sender's IP address
*               char* senderName - Sender's username
*               char* message - Message content to send
*               char direction - Message direction indicator ('>' or '<')
* RETURNS:      void
*/
void parcelMessage(int receiverSocket, char* senderIP, char* senderName, char* message, char direction) {
    int messageLen = strlen(message);
    int pos = 0;

    // Break the message into chunks of CHUNK_SIZE characters
    while (pos < messageLen) {
        char chunk[CHUNK_SIZE + 1];
        int chunkSize = 0;

        // Try to break at a space if possible
        int breakPoint = pos + CHUNK_SIZE;
        
        // If we're beyond the message length, just use the remaining characters
        if (breakPoint >= messageLen) {
            breakPoint = messageLen;
        } else {
            // Look for a space to break at
            int j = breakPoint;
            while (j > pos && message[j] != ' ') {
                j--;
            }
            // If we found a space, break there
            if (j > pos) {
                breakPoint = j + 1; // Include the space in the first chunk
            }
        }

        // Copy the chunk
        chunkSize = breakPoint - pos;
        if (chunkSize > CHUNK_SIZE) {
            chunkSize = CHUNK_SIZE; // Ensure we don't exceed CHUNK_SIZE
        }
        strncpy(chunk, message + pos, chunkSize);
        chunk[chunkSize] = '\0';

        // Format and send this chunk
        formatAndSendMessage(receiverSocket, senderIP, senderName, chunk, direction);

        // Move to the next position
        pos += chunkSize;
    }
}



/*
* FUNCTION:     isUsernameTaken
* DESCRIPTION:  Checks if a username is already in use by another active client. 
* 		Uses a mutex lock to protect the shared clients array during checks
* PARAMETERS:   char* username - Username to check
*               int currentIndex - Index of client being checked (excluded from check)
* RETURNS:      int - 1 if username is taken, 0 if available
*/
int isUsernameTaken(char* username, int currentIndex) {
    pthread_mutex_lock(&clients_mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // Skip checking our own slot
        if (i == currentIndex) continue;
        
        if (clients[i].active && strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 1; // Username is taken
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
    return 0; // Username is available
}
