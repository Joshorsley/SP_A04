/*
* FILE : main.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION : Main server implementation for the "Can We Talk?" chat system. Handles socket creation, multiple client connections, and thread management. 
*              	  Server Initialization: 
*              	  	 - The server takes the port number given by the command line and opens a server socket for clients to connect to 
*			 - the SIGINT handler is set up for graceful shutdown later in the program 
*			 - The client list array is initialized
*			 - The listening loop is started to begin adding connecting clients 
*
*              	  Connection Initialization:
*     			 - Client sends the chosen userID as first message
*     			 - Server will validate the userID to ensure it is unique to the rest of the current clients on the list
*     			 - Server adds client to active clients list if it is unique
*     			 - each client gets a dedicated handler thread
*   
*  		   Regular Message Exchange (handled in the client_utils functions file in each client thread):
*     			 - Messages are recieved from the client in up to 80 char message sizes
*     			 - The server parses the message into 40 char packets
*     			 - The parsed messages are broadcast to all connected clients, including the client that sent the message
*   
*  		   Client Termination:
*     			 - When the Client sends ">>bye<<" message, the Server knows it needs to disconnect that client 
*     			 - Server removes the client from list of active users
*     			 - Connection with the client is closed
*     			 - Other clients are notified that the user has disconnected
*
*     		   Server Shutdown: 
*     		   	- When serverTunning becomes 0 or SIGINT is used, then the server begins the shutdown process
*     		   	- Any remaining client connections are closed if present 
*     		   	- Threads are terminated 
*     		   	- Server Socket is closed 
*     		   	- The mutex is destroyed 
*			- Program ends 
*
*		DATA STRUCTURE: 
*
*			ClientInfo structure
*				- contains the identification for the client socket, the client's IP address and port number, a boolean flag noting an active connection, and the user ID
*				- A new struct is created when a new client connects, and used to maintain that client's information
*				- each client's struct is stored on a global array called 'clients' that is maintained by the server as users connect and disconnect.
*				- When a client disconnects, their information struct is discared and removed from the global array 
*
*		FUNCTIONS: 
*    			 int main(int argc, char* argv[]) - Entry point that initializes the server, creates a listening socket, and handles
*                                       		     incoming client connections. 
*/

#include "server_utils.h"
#include "client_utils.h"
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

// Global variables
clientInfo clients[MAX_CLIENTS] = {0}; 
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int clientCount = 0;
int serverSocket;
int serverRunning = 1;

int main(int argc, char* argv[]) {
    int port = PORT;

    if (argc > 1) {
        port = atoi(argv[1]);

        if (port <= 0 || port > 65535) {
            printf("Usage: %s [port]\n", argv[0]);
            printf("  port: Optional. Port number to listen on (1-65535). Default: %d\n", PORT);
            return EXIT_FAILURE;
        }
    }
    signal(SIGINT, handleSignal);

    // Create socket
    serverSocket = createServerSocket(port);
    if (serverSocket < 0) {
        return EXIT_FAILURE;
    }

    printf("Server started on port %d\n", port);
    printf("Listening for connections...\n");

    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].socket = -1;
    }

    // Main loop to accept Client connections
    while(serverRunning) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        // Accept a new connection
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        // Check if accept failed
        if (clientSocket < 0) {
            if (serverRunning) {
                perror("Accept failed");
            }
            continue;
        }
        
        printf("New connection attempt received\n");

        //Lock the mutex to safely access the clients array
        pthread_mutex_lock(&clients_mutex);

        if (clientCount >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. Connection rejected.\n");
            // Send rejection to client
            const char *msg = "Sorry, the chat server is full. Please try again later.\n";
            send(clientSocket, msg, strlen(msg), 0);
            close(clientSocket);
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }

        // Find an empty slot in the clients array
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                break;
            }
        }

        // Add the client to our array
        clients[i].socket = clientSocket;
        clients[i].address = clientAddr;
        clients[i].active = 1;
        clientCount++; // increment the client count

        //unlock the mutex
        pthread_mutex_unlock(&clients_mutex);

        // Create a thread to handle this client
        pthread_t thread;
        int *clientIndex = malloc(sizeof(int)); // Allocate memory for the client index
        if (clientIndex == NULL) {
            perror("Failed to allocate memory");
            pthread_mutex_lock(&clients_mutex);
            clients[i].active = 0;
            close(clients[i].socket);
            clientCount--;
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }

        *clientIndex = i; // Store the index value

        // Create a new thread to handle this client
        if (pthread_create(&thread, NULL, clientHandler, clientIndex) != 0) {
            perror("Failed to create thread");
            free(clientIndex);
            pthread_mutex_lock(&clients_mutex);
            clients[i].active = 0;
            close(clients[i].socket);
            clientCount--;
            pthread_mutex_unlock(&clients_mutex);
            continue;
        } else {
            
            pthread_detach(thread);
            printf("Created thread for client %d\n", i);
        }

        printf("New client connected. Total clients: %d\n", clientCount);
    }

    cleanupAndExit();

    return 0;
}
