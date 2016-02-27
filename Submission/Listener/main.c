/*
 * main.c
 *
 *  Created on: Dec 30, 2015
 *      Author: hershco
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "ConectionManage.h"
#include <signal.h>

extern volatile int sock_for_sigint;
extern FILE* play;
void signal_handler(int signo);
FILE* send_output_to_play();
int print_logo();

int main(int argc, char *argv[])
{
	signal(SIGINT, signal_handler);
	if (argc != 3 && argc != 4){ //validate that we get all the necessary arguments
		puts("Usage: radio_listener <Multicast addreess> <Port> (<-p>)\nUse -p flag for screen printing instead of playing with play");
		return -1;
	}
	print_logo();
	play = NULL;
	if (argc == 3) // check if there's a -p flag
		play = send_output_to_play();
	else if((argc == 4 &&  (0!=strcmp(argv[3],"-p")))){ //incase of unkown flag as argument
		puts("Usage: radio_listener <Multicast addreess> <Port> (<-p>)\nUse -p flag for screen printing instead of playing with play");
		return -1;
	}
	Play_MultiCast_Stream(argv[1],argv[2]); //do what you have to do and play the multicast radio ;)
	return 0;
}


/** set ctrl+c to exit the progrem corectly*/
void signal_handler(int signo)
{
	if(!close(sock_for_sigint)){ //close the socket
		if(play){ //if using play to play the file close it
			pclose(play);
		}
		system("clear");
		printf("socket close successfully!\n");
		puts("Thanks for using InternetRadio_Listener...\n ");
		exit(EXIT_SUCCESS);
	}
	else{
		perror("fail to close Socket");
		exit(EXIT_FAILURE);
	}
}

/** use Play to play the received music
 *
 * @return the pointer to play process or null if play isnw installed
 */
FILE* send_output_to_play()
{
	FILE* play;
	if (access("/usr/bin/play", X_OK) == 0) { //check if play is installed
	    play = (FILE*)popen("/usr/bin/play -t mp3 -","w"); //open play process
	    return play;
	}
	else {
	    return NULL;
	}
}

int print_logo(){
	system("clear");
	puts("______          _ _         _     _     _");
	puts("| ___ \\        | (_)       | |   (_)   | |");
	puts("| |_/ /__ _  __| |_  ___   | |    _ ___| |_ ___ _ __   ___ _ __");
	puts("|    // _` |/ _` | |/ _ \\  | |   | / __| __/ _ \\ '_ \\ / _ \\ '__|");
	puts("| |\\ \\ (_| | (_| | | (_) | | |___| \\__ \\ ||  __/ | | |  __/ |");
	puts("\\_| \\_\\__,_|\\__,_|_|\\___/  \\_____/_|___/\\__\\___|_| |_|\\___|_|");
	puts("\nBy:\nGal Hershcovich & David Dabah\nUse ctrl+c to exit..\nVisit our project GitHub at: Http://github.com/hershcovich/InternetRadio\n");
	return 0;
}
