//this is a file for me so i can tell what the fuck is going on

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8000               // may need to change as my computer is using port 8000, dont know if that affects the virtual machine
#define MAX_PENDING_CONNECTIONS 10
#define MAX_NAME_LENGTH 6
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MESSAGE_SIZE 80
#define CHUNK_SIZE 40

// Client information structure
typedef struct {
    int socket;
    char username[MAX_NAME_LENGTH];
    struct sockaddr_in address;     // Client's network address information
    int active;                     // Flag to show client slot is active
} clientInfo;

// Global variables, I know, I know but it is hinted at in the documentation, needs to be accessible by all threads
clientInfo clients[MAX_CLIENTS]; 
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int clientCount = 0;
int serverSocket;
int serverRunning = 1;

// Function prototypes
void* clientHandler(void* arg);


int main(void){
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Erro creating socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address
    int opt =1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family =AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket 
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket,MAX_PENDING_CONNECTIONS) < 0) {
        perror("listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server started on port %d\n", PORT);
    printf("Listening for connections...\n");

    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < MAX_CLIENTS; i++){
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
        for (i= 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                break;
            }
        }

        // Add the client to our array
        clients[i].socket = clientSocket;
        clients[i].address = clientAddr;
        clients[i].active =1;
        clientCount ++; // increment the client count

        //unlock the mutex
        pthread_mutex_unlock(&clients_mutex);

        //TODO: Create a thread to handle this client
        // Create a thread to handle this client
        pthread_t thread;
        int *clientIndex = malloc(sizeof(int)); // Allocate memory for the client index
        if (clientIndex == NULL) {
            perror("Failed to allocate memory");
            pthread_mutex_lock(&clients_mutex);
            clients[i].active = 0;
            close(clients[i].socket);
            clientCount --;
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
            clientCount --;
            pthread_mutex_unlock(&clients_mutex);
            continue;
        } else {
            //Detach thread -we don't need to join it later
            pthread_detach(thread);
            printf("Created thread for client %d\n", i);
        }

        printf("New client connected. Total clients: %d\n", clientCount);
    }

    

    //TODO: Cleanup and exit
    close(serverSocket);
    return 0;

}

void* clientHandler(void* arg) {
    //Extract the client index and free the memory
    int index = *((int*)arg);
    free(arg);

    // Get the client socket and prepare a buffer
    int clientSocket = clients[index].socket;
    char buffer[BUFFER_SIZE];
    int bytesRead;

    // Get the client's IP address as a string
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clients[index].address.sin_addr), clientIP, INET_ADDRSTRLEN);

    printf("Handling client %d with IP \n", index, clientIP);

    // First message from client should be the username
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        printf("Client disconnected before sending username\n");

        // Lock the mutex before updating shared data
        pthread_mutex_lock(&clients_mutex);

        // Clean up and mark slot as inactive
        close(clients[index].socket);
        clients[index].active = 0;
        clientCount --;

        pthread_mutex_unlock(&clients_mutex);

        return NULL;

    }
    
    //NULL -terminate the received data
    buffer[bytesRead] = '\0';

    // Store the username 
    pthread_mutex_lock(&clients_mutex);
    strncpy(clients[index].username, buffer, MAX_NAME_LENGTH -1);
    clients[index].username[MAX_NAME_LENGTH -1] = '\0';
    pthread_mutex_unlock(&clients_mutex);

    printf("Client %d registered with username: %s\n", index, clients[index].username);

    //TODO: Announce that a new user has joined
    char announcement[BUFFER_SIZE];
    snprintf(announcement, sizeof(announcement), "User %s has joined the chat", clients[index].username);
    broadcastMessage(announcement, index);

    // Main message loop - receive and process messages
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) -1, 0)) > 0) {
        // NULL- terminate the received data
        buffer[bytesRead]= '\0';

        printf("Received from %s: %S\n", clients[index].username, buffer);

        // Check if client wants to disconnect
        if (strcmp(buffer, ">>bye<<") == 0) {
            printf("Client %s is disconnecting\n", clients[index].username);
            break;
        }

        // TODO: Process and broadcast the message
        broadcastMessage(buffer, index);
    }

    if (bytesRead == 0) {
        printf("Client %s closed connection\n", clients[index].username);
    }else if (bytesRead < 0) {
        printf("Error receiving from client %s\n", clients[index].username);
    }

    // TODO: Announce that the user has left
    snprintf(announcement, sizeof(announcement), "User %s has left the chat", clients[index].username);
    broadcastMessage(announcement, index);

    // Lock the mutex before updating shared data
    pthread_mutex_lock(&clients_mutex);

    // Clean up and mark slot as inactive
    close(clients[index].socket);
    clients[index].active = 0;
    clientCount --;

    pthread_mutex_unlock(&clients_mutex);

    printf("Client %d cleaned up, total clients: %d\n", index, clientCount);

    return NULL;
}

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
            char direction = ( i == senderIndex) ? '>' : '<';

            //TODO: Call function to format and send message
            // For now just echo the raw message
            send(clients[i].socket, message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}