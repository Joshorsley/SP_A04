#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

// Constants
#define PORT 8000       // this is just the defaul I chose, it can change

// #this will be the entry point for the chat-server
// must contain about 11 threads, 1 for main program, 10 for chat clients
// must take two command line arguments:
    // chat-client -user<userID> -server<server name>

void didTheSocketSetUp(int socketPassed);
void bindSocket(struct sockaddr_in server_addr, int listenSocket);

int main(void){
    // create socket
    int listenSocket;
    struct sockaddr_in server_addr; 
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    didTheSocketSetUp(listenSocket);
    memset(&server_addr, 0, sizeof(server_addr));       // clears the struct
    server_addr.sin_family = AF_INET;                   // Address family IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;           // Bind to all available interfaces
    server_addr.sin_port = htons(PORT);                 // Port to bind to

    // bind socket to specific IP and port -- use bind()
    bindSocket(server_addr, listenSocket); 

    // begin listening loop for client connections
    if (listen(listenSocket, 10) == -1){
        perror("The listener is not working... :(");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    prinft("Listening for connections...]n");       // this is not actually listening correctly yet, still needs more logic to connect clients

        // needs to manage a client list as clients connect
        // needs to access a message queue to process client messages

    // somewhere in this mess a mutex/semaphore is initialized to manage and protect data structure

    close(listenSocket);
    return 0;                                       // lets us know if it closed correctly by returning a 0
}

void didTheSocketSetUp(int listenSocket){
    if (listenSocket == -1){
        perror("The socket is not working...");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
}

void bindSocket(struct sockaddr_in server_addr, int listenSocket){
    if(bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("Socket didn't bind correctly...");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to %d\n", PORT);               // used for debug purposes
}