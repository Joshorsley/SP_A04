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

}