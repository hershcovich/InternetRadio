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
#include <signal.h>
#include "GlobalStructs.h"
#include "MessageTypes.h"

#define BUFFER_SIZE 4096
#define TIMEOUT 1 //timeout in sec

extern Station* list_of_stations;
extern int number_of_stations;
extern int Multicast_Port;
extern uint16_t TCP_Port;
extern struct in_addr multicast_address;
extern pthread_t UDP_Default_Thread,TCP_Default_Thread;

typedef struct socketnode{
	int sock;
	struct socketnode* nextsocket;
	struct sockaddr_in address;
	pthread_t  pth;
}socketnode;

socketnode* socketlist;
socketnode* welcomesocket; // a node to for the default welcome socket
socketnode* tmpsocket; //a socket for new conections

void* tcp_radio_welcome(void* arg);
void *tcp_radio_conection(void* arg);
char* what_song_is_played(uint16_t Station);
int hello_handshake(socketnode *sonode);
int close_socket_node(socketnode *sonode);
int server_client_interaction(socketnode *sonode);
int close_TCP_server();
int print_TCP_data();


#endif /* TCPCONECTIONMANAGE_H_ */
