/*
 * UDPConectionManage.h
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#ifndef UDPCONECTIONMANAGE_H_
#define UDPCONECTIONMANAGE_H_
#include "GlobalStructs.h"

extern Station* list_of_stations;
extern int number_of_stations;
extern int Multicast_Port;
extern int TCP_Port;

void *udp_init_channels(void* arg);

#endif /* UDPCONECTIONMANAGE_H_ */
