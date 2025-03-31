/*
* FILE : signals.c
* PROJECT : Can We Talk? System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-20
* UPDATED : 2025-03-31
* DESCRIPTION : This file holds the function for handling the SIGINT shutdown message. 
* FUNCTIONS:
* 		handleSignal - The handler function for the SIGINT signal, which will trigger the server to shutdown gracefully 
*/

#include "signals.h"

/*
* FUNCTION:     handleSignal
* DESCRIPTION:  Signal handler  SIGINT (Ctrl+C) to initiate a graceful server shutdown before all clients have disconnected
* PARAMETERS:   int sig - The signal number received
* RETURNS:      void
*/
void handleSignal(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived shutdown signal. Shutting down server...\n");
        serverRunning = 0;

        close(serverSocket);
    }
}
