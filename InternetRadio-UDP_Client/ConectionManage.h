/*
 * ConectionManage.h
 *
 *  Created on: Dec 30, 2015
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


int Play_MultiCast_Stream(char *IP_ADDR , char* IP_Port);
int read_socks(int  sock, fd_set *socks ,struct sockaddr_in* addr);
int handle_user_input();
int handle_multicast_recive(int sock ,struct sockaddr_in* addr);
#endif /* CONECTIONMANAGE_H_ */
