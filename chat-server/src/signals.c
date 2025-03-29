#include "signals.h"

void handleSignal(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived shutdown signal. Shutting down server...\n");
        serverRunning = 0;

        close(serverSocket);
    }
}