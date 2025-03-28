#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

// Constants
#define ERROR_CODE -1       // default error codes for the program
#define SUCCESS 0           // default success code for the program
#define SERVER_RUNNING 2    // define for the while loop to keep server running
#define PORT 8000           // this is just the defaul I chose, it can change
#define BACKLOG 10          // not sure if this is really how I should name this...
#define MAX_NAME_LENGTH 6   //Cant remember the number 
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024    // buffer for message size - this is not yet set to the correct size
                            // need to figure out the correct buffer size for requirements



void didTheSocketSetUp(int socketPassed);
void didTheSocketBind(struct sockaddr_in server_addr, int listenSocket);
void socketListeningCheck(int listenSocket);
int canServerAcceptClient(int clientCount);
void* clientHandler(void* clientSocketPtr);
void sendMessageToClients(const char* message, int senderSocket);

// !!IF WE ARE GOING TO USE THIS STRUCT, VARIABLES MUST BE ELIMINATED AND THIS STRUCT USED IN THEIR PLACE!!
typedef struct {
    int socket;
    char username[MAX_NAME_LENGTH];
    struct sockaddr_in address;
} clientsInfo;

pthread_mutex_t clientListMutex = PTHREAD_MUTEX_INITIALIZER;
clientsInfo clients[MAX_CLIENTS]; 
int clientCount = 0;

int main(void){                                         // pretty sure we need to add some args to this...
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
    didTheSocketBind(server_addr, listenSocket); 

    socketListeningCheck(listenSocket);
    printf("Listening for connections...\n");
    
    // can change the serverShutdownCode when last client disconnects
    int serverShutdownCode = SERVER_RUNNING;
    while(serverShutdownCode == SERVER_RUNNING){
        int responseCode = ERROR_CODE; 
        Pthread_mutex_lock(&clientListMutex); 

        // check if we server can accept new clients
        responseCode = canServerAcceptClient(clientCount);
        if(responseCode == SUCCESS){
            int clientSocket = accept(listenSocket, NULL, NULL); 
            // if already at max clients
            if(clientSocket == ERROR_CODE){
                perror("Accepting client connection failed...\n");
                pthread_mutex_unlock(&clientListMutex);
                continue; 
            }

            //add a new client to the client list
            clients[clientCount].socket = clientSocket; 
            clientCount++;
            printf("New client has connected. Total clients: %d\n", clientCount);       // for debugging

            //UNLOCK MUTEX
            pthread_mutex_unlock(&clientListMutex);

            // create a thread for the new client
            pthread_t clientThread; 
            if(pthread_create(&clientThread, NULL, clientHandler, (void*)&clientSocket) != 0){
                perror("Thread creation failed...\n");
            }
            pthread_detach(clientThread); 
        } else{
            pthread_mutex_unlock(&clientListMutex);
            sleep(1);
            continue; 
        }
        // code here to check client statuses -- if they have all sent the >>bye<< message, then change the serverShutdownCode to anything but 2!
    }
    
    // needs to manage a client list as clients connect
    // needs to access a message queue to process client messages
    // somewhere in this mess a mutex/semaphore is initialized to manage and protect data structure

    close(listenSocket);
    return SUCCESS;                                    // lets us know if it closed correctly by returning a 0
}

void didTheSocketSetUp(int listenSocket){
    if (listenSocket == ERROR_CODE){
        perror("The socket is not working...\n");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
}

void didTheSocketBind(struct sockaddr_in server_addr, int listenSocket){
    if(bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == ERROR_CODE){
        perror("Socket didn't bind correctly...\n");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to %d\n", PORT);               // used for debug purposes
}

void socketListeningCheck(int listenSocket){
    if(listen(listenSocket, 10) == ERROR_CODE){
        perror("The listener is not working...\n");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    return;
}

int canServerAcceptClient(int clientCount){
    int responseCode = SUCCESS;
    if(clientCount >= MAX_CLIENTS){
        printf("Maximum number of clients are currently connected.\n");
        responseCode = ERROR_CODE;
    }
    return responseCode;
}

// this should run in its own thread to handle each client
void* clientHandler(void* clientSocketPtr) {
    int clientSocket = *((int*)clientSocketPtr); 
    char buffer[BUFFER_SIZE];
    int bytesRead; 

    // receive the incoming messages
    while((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0){
        buffer[bytesRead] = '\0';
        printf("Message received is: %s\n", buffer);    // for debugging purposes
        // send message to all clients
        sendMessageToClients(buffer, clientSocket); 
    }

    // this logic should probably use the specific >>bye<< message instead of the bytesRead = 0
    if(bytesRead == 0){
        printf("Client has diconnected.\n");
    } else{
        perror("Receive failed.\n");
    }

    // remove the client once they disconnect
    pthread_mutex_lock(&clientListMutex);
    for(int i = 0; i < clientCount; i++){
        if(clients[i].socket == clientSocket){
            clients[i] = clients[clientCount - 1];
            clientCount--;
            break;          // prevent going through the rest of the list if you find the client early
        }
    }
    pthread_mutex_unlock(&clientListMutex);
    close(clientSocket);

    return NULL;
}

void sendMessageToClients(const char* message, int senderSocket){
    pthread_mutex_lock(&clientListMutex);

    for(int i = 0; i < clientCount; i++){
        if(clients[i].socket != senderSocket){
            send(clients[i].socket, message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&clientListMutex);
}