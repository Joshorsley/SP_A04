/*
* FILE : signals.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION :
* FUNCTIONS:
*/
#include "signals.h"

// FUNCTION :
// DESCRIPTION : 
// PARAMETERS :
// RETURNS :
void handleSignal(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived shutdown signal. Shutting down server...\n");
        serverRunning = 0;

        close(serverSocket);
    }
}