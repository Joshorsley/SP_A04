#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "main.h"

// Function prototypes
void* clientHandler(void* arg);
void broadcastMessage(char *message, int senderIndex);
void formatAndSendMessage(int receiverSocket, char* senderIP, char* senderName, char* message, char direction);
void parcelMessage(int receiverSocket, char* senderIP, char* senderName, char* message, char direction);
int isUsernameTaken(char* username, int currentIndex);

#endif /* CLIENT_UTILS_H */