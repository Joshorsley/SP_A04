/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */


#include <stdbool.h>
#include <ncurses.h>


#ifndef PROGRAM_FUNCTIONS_H
#define PROGRAM_FUNCTIONS_H




bool programStart(int argc, char* argv[], bool serverOrIPFlag, int socketID, char* serverName, char* userID, char* clientIP, char* serverAddress, char* timestamp, char* message, bool programEndFlag);
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag, char* userID, char* serverName, char* serverAddress);
bool resolveServerName(bool* serverOrIPFlag, char* serverName, char* serverAddress);
bool createSocket(int* socketID);
bool connectToServer(int socketID, char* serverAddress);
bool IPCheck(char* serverName, bool* serverOrIPFlag);
bool getClientIP(char* clientIP);
void programEnd(int socketID, WINDOW* inWin, WINDOW* outWin);

#endif // PROGRAM_FUNCTIONS_H
