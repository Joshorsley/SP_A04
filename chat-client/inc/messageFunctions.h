/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */



#ifndef MESSAGE_FUNCTIONS_H
#define MESSAGE_FUNCTIONS_H

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

bool hiMessage(ClientInfo* clientInfo);
void byeMessage(ClientInfo* clientInfo);
void receiveMessages(ClientInfo* info);
//bool createMessage(char* message, char* clientIP, bool programEndFlag, char* userID, char* timestamp, int socketID);
//bool sendMessage(char* message, char* clientIP, bool programEndFlag, char* userID, char* timestamp, int socketID);
//bool receiveMessage(char* clientIP, char* userID);


#endif // MESSAGE_FUNCTIONS_H

