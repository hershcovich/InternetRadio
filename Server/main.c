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

int terminate();
int print_data();
int handle_user_input();

int main(int argc,char** argv){
	pthread_t UDP_Default_Thread,TCP_Default_Thread;
	int i,user_input;
	if(argc < 4){
		puts("usage: radio_server <TCP PORT> <MLTICAST ADDRESS> <MULTICAST PORT> (<FILE1>) (<FILE2>) ...\n");
		return 0;
	}
	number_of_stations = argc - 4;
	if (number_of_stations > 0){
		list_of_stations = (Station*)malloc(sizeof(Station)*number_of_stations);
		for(i = 4 ; i < argc ;i++){
			list_of_stations[i-4].chanel_number = i-4;
			inet_aton(argv[2],(list_of_stations[i-4].multicast_address));
			list_of_stations[i-4].list_of_songs = (songlist*)malloc(sizeof(songlist));
			list_of_stations[i-4].list_of_songs->song_filename=argv[i];
			list_of_stations[i-4].list_of_songs->song_filenamr_length = strlen(list_of_stations[i-4].list_of_songs->song_filename);
		}
		//init udp multicast server
		pthread_create(&UDP_Default_Thread,NULL,udp_init_channels,list_of_stations);
	}
	//init tcp server
	pthread_create(&TCP_Default_Thread,NULL,tcp_radio_welcome,list_of_stations);
	while(1){
		if((user_input = handle_user_input()) == -1){
			terminate();
		}
		else if(user_input == 1)
			print_data();
	}
}

int handle_user_input(){
	int user_request = -2;
	int c;
	char input[8];
	if(!fgets(input,sizeof(input),stdin)){
		perror("error receiving user input");
	}
	if((input[strlen(input)-1]) != '\n'){
		while((c = getchar()) != '\n' && c != '\0');
	}
	if(strcmp(input,"q\n") == 0){
		return -1;
	}
	else if(strcmp(input,"n\n") == 0){
			return 1;
		}
	else
		printf("please enter n for list of stations and clients,q for exit");
	return user_request;
}
