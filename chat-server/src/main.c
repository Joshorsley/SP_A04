#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

// Constants
#define PORT 8000           // this is just the defaul I chose, it can change
#define BACKLOG 10          // not sure if this is really how I should name this...
#define MAX_NAME_LENGTH 6   //Cant remember the number 
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024    // buffer for message size - this is not yet set to the correct size
                            // need to figure out the correct buffer size for requirements

// #this will be the entry point for the chat-server
// must contain about 11 threads, 1 for main program, 10 for chat clients
// must take two command line arguments:
    // chat-client -user<userID> -server<server name>

void didTheSocketSetUp(int socketPassed);
void didTheSocketBind(struct sockaddr_in server_addr, int listenSocket);
void socketListeningCheck(int listenSocket);
void* clientHandler(void* clientSocketPtr);
void handleClient(int clientSocket);

typedef struct {
    int socket;
    char username[MAX_NAME_LENGTH];
    struct sockaddr_in address;
} clientsInfo;

int main(void){                                         // pretty sure we need to add some args to this...
    // create socket
    int listenSocket;
    int clientCount = 0;                                // move this to a struct???
    struct sockaddr_in server_addr; 
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    didTheSocketSetUp(listenSocket);
    memset(&server_addr, 0, sizeof(server_addr));       // clears the struct
    server_addr.sin_family = AF_INET;                   // Address family IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;           // Bind to all available interfaces
    server_addr.sin_port = htons(PORT);                 // Port to bind to

    // bind socket to specific IP and port -- use bind()
    didTheSocketBind(server_addr, listenSocket); 

    socketListeningCheck(listenSocket);
    printf("Listening for connections...\n");
    
    // begin listening loop for client connections
    while(1){                                        // this needs to be better implemented, but I am using it for now
        
        // lock the mutex before checking how many clients there are
            // ADD MUTEX!!!
            // LOCK MUTEX
        // check if the server has reached max clients

        // if no - accept incoming client
        if(clientCount >= MAX_CLIENTS){
            printf("Maximum number of clients are currently connected. Cannot accept new connections.\n");
            sleep(1);
            continue;
        }

        int clientSocket = accept(listenSocket, NULL, NULL); 
        if(clientSocket == -1){
            perror("Accepting client connection failed...\n");
            // UNLOCK MUTEX -- does this conflict with the unlock further down?
            continue; 
        }

        clientCount++;
        printf("New client has connected. Total clients: %d\n", clientCount);       // for debugging

        //UNLOCK MUTEX

        // create a thread for the new client
        pthread_t clientThread; 
        if(pthread_create(&clientThread, NULL, clientHandler, (void*)&clientSocket) != 0){
            perror("Thread creation failed...\n");
        }
        //kill the thread before exiting the looping
        pthread_detach(clientThread); 
    }

    // needs to manage a client list as clients connect
    // needs to access a message queue to process client messages
    // somewhere in this mess a mutex/semaphore is initialized to manage and protect data structure

    close(listenSocket);
    return 0;                                       // lets us know if it closed correctly by returning a 0
}

void didTheSocketSetUp(int listenSocket){
    if (listenSocket == -1){
        perror("The socket is not working...\n");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
}

void didTheSocketBind(struct sockaddr_in server_addr, int listenSocket){
    if(bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("Socket didn't bind correctly...\n");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to %d\n", PORT);               // used for debug purposes
}

void socketListeningCheck(int listenSocket){
    if(listen(listenSocket, 10) == -1){
        perror("The listener is not working...\n");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    return;
}

// this should run in its own thread to handle each client
void* clientHandler(void* clientSocketPtr) {
    int clientSocket = *((int*)clientSocketPtr); 

    // this function loops listening to the client
    handleClient(clientSocket);

    // unlock the mutex here after the handleClient function has finished and client disconnects

    close(clientSocket);

    return NULL;
}

// this is preticated on the fact that the client doesn't stay connected long term,
// it will need to be reworked to handle a constantly connected client
void handleClient(int clientSocket){
    char buffer[BUFFER_SIZE];
    int bytesRead; 

    while((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0){
        buffer[bytesRead] = '\0';
        printf("Received from client: %s\n", buffer);       // for debugging

        // echo the message to the client(s)
        send(clientSocket, buffer, bytesRead, 0);
    }

    if(bytesRead == 0){
        printf("Client has disconnected.\n");
    } else{
        perror("Receive failed.\n");
    }

    return;
}