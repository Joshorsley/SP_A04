/*
* FILE : server_utils.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION :
* FUNCTIONS:
*/
#include "server_utils.h"
#include "main.h"

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
int createServerSocket(int port) {
    int serverSocket;
    struct sockaddr_in serverAddr;
    
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(serverSocket);
        return -1;
    }

    // Set up the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind the socket 
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        return -1;
    }

    // Listen for connections
    if (listen(serverSocket, MAX_PENDING_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(serverSocket);
        return -1;
    }
    
    return serverSocket;
}

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
void cleanupAndExit(void) {
    printf("Cleaning up resources...\n");

    // Close all client connections
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            close(clients[i].socket);
            clients[i].active = 0;
        }
    }
    clientCount = 0;
    pthread_mutex_unlock(&clients_mutex);

    pthread_mutex_destroy(&clients_mutex);

    // Close the server socket
    if(serverSocket >= 0) {
        close(serverSocket);
    }

    printf("Server shutdown complete\n");
}

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
void checkForServerShutdown(void) {
    if (clientCount == 0) {
        printf("ALL clients disconnected. Shutting down server...\n");
        serverRunning = 0;

        int tempSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (tempSocket >= 0) {
            struct sockaddr_in tempAddr;
            memset(&tempAddr, 0, sizeof(tempAddr));
            tempAddr.sin_family = AF_INET;
            tempAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            tempAddr.sin_port = htons(PORT);

            connect(tempSocket, (struct sockaddr*)&tempAddr, sizeof(tempAddr));
            close(tempSocket);
        }
    }
}