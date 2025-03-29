#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

// Constants
#define PORT 8000               // may need to change as my computer is using port 8000
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

// Global variables declarations (defined in main.c)
extern clientInfo clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;
extern int clientCount;
extern int serverSocket;
extern int serverRunning;

#endif /* MAIN_H */