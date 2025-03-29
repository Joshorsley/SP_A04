#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "main.h"

// Function prototypes
int createServerSocket(int port);
void cleanupAndExit(void);
void checkForServerShutdown(void);

#endif /* SERVER_UTILS_H */