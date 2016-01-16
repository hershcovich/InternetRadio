/*
 * main.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */
#include "GlobalStructs.h"
#include "TCPConectionManage.h"
#include "UDPConectionManage.h"

Station* list_of_stations;
int number_of_stations;
int Multicast_Port;
int TCP_Port;

int main(int argc,char* argv){
	pthread_t UDP_Default_Thread,TCP_Default_Thread;

	number_of_stations = argc - 4;
	list_of_stations = (Station*)malloc(sizeof(Station)*number_of_stations);
	//init udp multicast server
	pthread_create(&UDP_Default_Thread,NULL,udp_init_channels,list_of_stations);
	//init tcp server
	pthread_create(&TCP_Default_Thread,NULL,tcp_radio_welcome,list_of_stations);
	while(1){
		//get user input
	}

}
