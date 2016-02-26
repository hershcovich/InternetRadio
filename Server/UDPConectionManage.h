/*
 * UDPConectionManage.h
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#ifndef UDPCONECTIONMANAGE_H_
#define UDPCONECTIONMANAGE_H_
#include "GlobalStructs.h"
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

extern Station* list_of_stations;
extern int number_of_stations;
extern int Multicast_Port;
extern uint16_t TCP_Port;
extern struct in_addr multicast_address;
extern pthread_t UDP_Default_Thread,TCP_Default_Thread;

//Structure of arguments to pass to the thread
struct ThreadArg{
	int stationNum;
};

void* newStationTread (void* arg);

void *udp_init_channels(void* arg);
int close_udp_server();
int print_UDP_data();

#endif /* UDPCONECTIONMANAGE_H_ */
