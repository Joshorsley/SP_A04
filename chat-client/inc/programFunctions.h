/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */



bool programStart(int argc, char* argv[], bool serverOrIPFlag, int socketID);
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag, char userID, char serverName);
bool resolveServerName(bool serverOrIPFlag, char serverName);
bool createSocket(int socketID);
bool connectToServer(int socketID);
bool IPCheck(char* serverName);
bool getClientIP(char* clientIP);
void programEnd(int socketID, WINDOW* inWin, WINDOW* outWin);
