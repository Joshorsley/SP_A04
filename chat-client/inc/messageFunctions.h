/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */




void getTimestamp();
bool hiMessage(char message, char* clentIP, bool programEndflag, char userID, char timestamp, char clientIP, int socketID);
void byeMessage(char message, char* clentIP, bool programEndflag, char userID, char timestamp, char clientIP, int socketID);
bool createMessage(char message, char* clentIP, bool programEndflag, char userID, char timestamp, char clientIP, int socketID);
bool recieveMessage(char* clientIP, char* userID);




