/*
 * ConectionManage.h
 *
 *  Created on: Jan 4, 2016
 *      Author: hershco
 */

#ifndef CONECTIONMANAGE_H_
#define CONECTIONMANAGE_H_

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
#include "MessageTypes.h"

int open_tcp_concection(char* IP,char* Port);
int open_radio_sc_conection();
int send_ask_song(int socket,int channel);
int print_invalid_command(char* to_print);
int print_Announce(char* to_print);
int state_machine(char* IP,char* Port);
int sock;
typedef enum state {CONNECT,HELLO,CONNECTED,FAIL,DISCONNECT} state ;

state status;

#endif /* CONECTIONMANAGE_H_ */
