/* FILE :       messageFunctions.h
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    Kalina Cathcart, Josh Horsley, John Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the function prototypes for messageFunctions.c. These functions handle the sending and receiving of messages between the client and server. 
 */

#ifndef MESSAGE_FUNCTIONS_H
#define MESSAGE_FUNCTIONS_H

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#define MAX_SNT_MESSAGE 80 
#define MAX_RECVD_MESSAGE 40 
#define MAX_BUFFER 1024 
#define MAX_TIMESTAMP 10 
#define MAX_IP 16 
#define MSG_ROW_START 1 

bool hiMessage(ClientInfo* clientInfo);
bool byeMessage(ClientInfo* clientInfo);
void receiveMessages(ClientInfo* info);

#endif // MESSAGE_FUNCTIONS_H

