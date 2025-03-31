/* FILE :       main.h
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    Kalina Cathcart, Josh Horsley, John Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file hold the global constants, and client info structure  for the chat - client program.
 */


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

#define PORT 8000 
#define ARG_COUNT 3
#define MAX_USER_ID 6
#define MAX_SERVER_NAME 255 
#define ARG1_SKIP 5 
#define ARG2_SKIP 7 
#define IP_DOT_COUNT 3 
#define MAX_SNT_MESSAGE 80 
#define MAX_RECVD_MESSAGE 40
#define MAX_BUFFER 1024 
#define MAX_TIMESTAMP 10 
#define MAX_IP 16 
#define BUFFER_SIZE 256

typedef struct 
{
    char userID[MAX_USER_ID];
    int socketID;
    char serverName[MAX_SERVER_NAME];
    char serverAddress[MAX_IP];
    bool status;
    WINDOW* inWin;			
	WINDOW* outWin;
} ClientInfo;


#endif /* MAIN_H */
