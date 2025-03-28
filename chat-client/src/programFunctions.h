/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */



bool programStart(int argc, char* argv[], bool serverOrIPFlag)
bool parseArgs(int argc, char* argv[], bool serverOrIPFlag);
bool resolveServerName();
bool createSocket();
bool connectToServer();
bool IPCheck(char* serverName);
bool getClientIP(char* clientIP);
void programEnd();
