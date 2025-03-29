/*
* FILE : main.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION :
* FUNCTIONS:
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
            //Detach thread -we don't need to join it later
            pthread_detach(thread);
            printf("Created thread for client %d\n", i);
        }

        printf("New client connected. Total clients: %d\n", clientCount);
    }

    cleanupAndExit();

    return 0;
}