/* FILE :       main.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    The scooby gang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file hold the entry point for the chat - client program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>



int main(int argc, char* argv[])
{

	printf("Welcome to the Chat - Client Terminal\n");

	if (argc != 3)
	{
		printf("ERROR : Incorrect number of arguments. 3 Required\n");
		printf("Usage: __________________________________________________ \n"); // TODO: Add usage instructions ****************************
		return 0;
	}

	// Parse Arguments into variables for the program 
	parseArgs(); 



	// Resolve the server name to an IP address

	// Create socket
	socket(); 

	// Connect to the server
	connect(); 

	// Initialize the ncurses library
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	// Create input window for typing messages 
	// Create output window for displaying chat history 


	// Loop for User Input 
	While(true)
	{
		// Read user input from the Input Window (up to 80 characters)
	
		// If the user presses enter: 
			// Send message to the server using send().
			// Display the message in the Output Window.
	

		// If the message is ">>bye<<",
			// Close the connection to the server
			// Clean the ncurses windows
			// Exit the program
			printf("Chat - Client is now closing, Goodbye!\n");
			return 0;
	}

}






// Function to parse the arguments passed to the program
bool parseArgs()
{

	/*
	o	Accept two command-line switches:
				–user<userID>: Specify the user’s ID (up to 5 characters).
				–server<server name>: Specify the server’s name or IP address.
	*/


	return true;
}



