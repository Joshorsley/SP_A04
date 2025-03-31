/* FILE :       programFunctions.h
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the function prototypes for programFunctions.c. These functions handle the system set up and connecting to the server. 
 */

#include <stdbool.h>
#include <ncurses.h>
#include "main.h"

#ifndef PROGRAM_FUNCTIONS_H
#define PROGRAM_FUNCTIONS_H

bool parseArgs(int argc, char* argv[], char* userID, char* serverName);
bool isAddress(const char* serverName);
bool resolveServerName(ClientInfo *clientInfo);
int connectToServer(ClientInfo *clientInfo);

#endif // PROGRAM_FUNCTIONS_H
