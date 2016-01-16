/*
 * TCPConectionManage.h
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#ifndef TCPCONECTIONMANAGE_H_
#define TCPCONECTIONMANAGE_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <string.h>
#include <pthread.h>
#include "GlobalStructs.h"

extern Station* list_of_stations;
extern int number_of_stations;
extern int Multicast_Port;
extern int TCP_Port;


typedef struct socketnode{
	int sock;
	struct socketnode* nextsocket;
	struct sockaddr_in address;
	pthread_t  pth;
}socketnode;

int tcp_radio_welcome(void* arg);
void tcp_radio_conection(void* arg);
char* what_song_is_played();



#endif /* TCPCONECTIONMANAGE_H_ */
