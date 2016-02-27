/*
 * GlobalStructs.h
 *
 *  Created on: Jan 16, 2016
 *      Author: hershco
 */

#ifndef GLOBALSTRUCTS_H_
#define GLOBALSTRUCTS_H_
#include "stdio.h"
#include <pthread.h>
#include <netinet/in.h>
//Colors define
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct songlist{
	char * song_filename;
	int song_filenamr_length;
	struct songlist* nextsong;
	FILE * songfile;
}songlist;

typedef struct Station {
	int chanel_number;
	struct sockaddr_in multicast_address;
	int socket_number;
	songlist *list_of_songs;
	songlist *current_playing_song;
	pthread_t ptr;
}Station;

#endif /* GLOBALSTRUCTS_H_ */
