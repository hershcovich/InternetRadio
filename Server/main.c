/*
 * main.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */
#include "GlobalStructs.h"
#include "TCPConectionManage.h"
#include "UDPConectionManage.h"
#include <netinet/in.h>

Station* list_of_stations;
int number_of_stations;
int Multicast_Port;
uint16_t TCP_Port;
struct in_addr multicast_address;
pthread_t UDP_Default_Thread,TCP_Default_Thread; //threads for tcp and udp severs


int terminate();
int print_data();
int handle_user_input();
int print_logo();

int main(int argc,char** argv){

	int i,user_input;
	if(argc < 4){ //check for reciving arguments
		puts("usage: radio_server <TCP PORT> <MLTICAST ADDRESS> <MULTICAST PORT> (<FILE1>) (<FILE2>) ...\n");
		return 0;
	}
	print_logo();
	TCP_Port = atoi(argv[1]); //save the recived TCP server port
	Multicast_Port = atoi(argv[3]); //save the recived multicast Port
	inet_aton(argv[2],&multicast_address); //save the recived multicast ip address
	number_of_stations = argc - 4; //calculate the number of stations
	/* Creating UDP stations*/
	if (number_of_stations > 0){
		list_of_stations = (Station*)malloc(sizeof(Station)*number_of_stations);

		for(i = 4 ; i < argc ;i++){
			list_of_stations[i-4].chanel_number = i-4;
			inet_aton(argv[2],&(list_of_stations[i-4].multicast_address.sin_addr));
			list_of_stations[i-4].multicast_address.sin_addr.s_addr =list_of_stations[i-4].multicast_address.sin_addr.s_addr + htonl(i-4);
			list_of_stations[i-4].list_of_songs = (songlist*)malloc(sizeof(songlist));
			list_of_stations[i-4].list_of_songs->song_filename=argv[i];
			list_of_stations[i-4].list_of_songs->song_filenamr_length = strlen(list_of_stations[i-4].list_of_songs->song_filename);
			list_of_stations[i-4].list_of_songs->nextsong =NULL;
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
	else if(strcmp(input,"p\n") == 0){
			return 1;
		}
	else
		printf("please enter p for list of stations and clients,q for exit\n");
	return user_request;
}

int terminate(){
	puts("EXITING..");
	close_TCP_server();
	close_udp_server();
	exit(0);
}

int print_data(){
	print_logo();
	print_TCP_data();
	print_UDP_data();
	return 0;
}

int print_logo(){
	system("clear");
	puts(ANSI_COLOR_RED "______          _ _         _____");
	puts("| ___ \\        | (_)       /  ___|");
	puts("| |_/ /__ _  __| |_  ___   \\ `--.  ___ _ ____   _____ _ __");
	puts("|    // _` |/ _` | |/ _ \\   `--. \\/ _ \\ '__\\ \\ / / _ \\ '__|");
	puts("| |\\ \\ (_| | (_| | | (_) | /\\__/ /  __/ |   \\ V /  __/ |");
	puts("\\_| \\_\\__,_|\\__,_|_|\\___/  \\____/ \\___|_|    \\_/ \\___|_|" ANSI_COLOR_RESET);
	puts("by Gal Hershcovich & David Dabah");
	puts("Don't forget to visit us on github");
	puts("http://github.com/hershcovich/InternetRadio");
	return 0;
}
