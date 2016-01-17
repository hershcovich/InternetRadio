/*
 * GlobalStructs.h
 *
 *  Created on: Jan 16, 2016
 *      Author: hershco
 */

#ifndef GLOBALSTRUCTS_H_
#define GLOBALSTRUCTS_H_

typedef struct songlist{
	char * song_filename;
	int song_filenamr_length;
	struct songlist* nextsong;
	FILE * songfile;
}songlist;

typedef struct Station {
	int chanel_number;
	int multicast_address;
	int socket_number;
	songlist *list_of_songs;
	songlist *current_playing_song;

}Station;

#endif /* GLOBALSTRUCTS_H_ */
