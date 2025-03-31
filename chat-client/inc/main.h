#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h> 
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8000 // port number for the server

#define ARG_COUNT 3		// expected arg count
#define MAX_USER_ID 5	// max user ID length
#define MAX_SERVER_NAME 255 // max server name length

#define ARG1_SKIP 5 // skip the first 5 characters of the argument
#define ARG2_SKIP 7 // skip the first 7 characters of the argument
#define IP_DOT_COUNT 3 // number of periods in an IP address

#define MAX_SNT_MESSAGE 80 // max message length for sent messages 
#define MAX_RECVD_MESSAGE 40 // max message size for sending/recieving messages
#define MAX_BUFFER 1024 // max buffer size									         **********TODO************ resize this to correct size
#define MAX_TIMESTAMP 10 // max timestamp size
#define MAX_IP 16 // max IP address size

typedef struct {
    char userID[MAX_USER_ID];
    int socketID;
    char serverName[MAX_SERVER_NAME];
    char serverAddress[MAX_IP];
    WINDOW *outWin;
    WINDOW *inWin;
    bool status;
} ClientInfo;


#endif /* MAIN_H */