/*
* FILE : client_utils.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION :
* FUNCTIONS:
*/
#include "client_utils.h"
#include "server_utils.h"

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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



// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
void logClientConnection(int index) {
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clients[index].address.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Handling client %d with IP %s \n", index, clientIP);
}

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
void announceUserJoined(int index) {
    char announcement[BUFFER_SIZE];
    snprintf(announcement, sizeof(announcement), "User %s has joined the chat", clients[index].username);
    broadcastMessage(announcement, index);
}

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
void handleClientDisconnect(int index) {
    // Announce that the user has left
    char announcement[BUFFER_SIZE];
    snprintf(announcement, sizeof(announcement), "User %s has left the chat", clients[index].username);
    broadcastMessage(announcement, index);
    
    disconnectClient(index);
}

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
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