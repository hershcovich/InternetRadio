/*
 * UDPConectionManage.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */
#include "UDPConectionManage.h"







void * udp_init_channels(void* arg){
	Station* stationList = (Station*)arg;
	int i,s;
	//open new threads as the number of stations
	for (i=0; i<number_of_stations; i++){
		struct ThreadArg *t_Arg=(struct ThreadArg*) malloc(sizeof(struct ThreadArg));
		if (t_Arg ==NULL){
			perror("malloc() failed");
			exit(EXIT_FAILURE);
		}
		t_Arg->stationNum=i;
		s=pthread_create(&(stationList[i].ptr),NULL,newStationTread,t_Arg);
		if (s!=0){
			perror("pthread_create() error");
			exit(EXIT_FAILURE);
		}

	}//for

//	while (1){
//
//	}//Run forever
	return 0;
}//udp_init_channels



void* newStationTread (void* arg){
	//Extract the station num from arg
	int stationNum=((struct ThreadArg*)arg)->stationNum;
	free(arg);

	int sock;
	FILE *currentSong;

	//setup sochadd fields
	struct sockaddr_in multiaddr; // struct to open the socket
	memset((char*) &multiaddr,0,sizeof(multiaddr));
	multiaddr.sin_family = AF_INET;
	multiaddr.sin_port = htons(Multicast_Port);
	multiaddr.sin_addr.s_addr = (list_of_stations[stationNum].multicast_address.sin_addr.s_addr);

	//try to create new socket
	if((sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0){ //In case of failure...
			perror("failed to open socket");
			exit(EXIT_FAILURE);
	} //if(sock ..
	/*Set TTL of multicast packet */
	u_char mcTTL = 64;
	if(setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL,&mcTTL,sizeof(mcTTL)) <0){ //in case of failure...
		perror("failed to SetSockopt multicast");
		close(sock);
		exit(EXIT_FAILURE);
	}

	list_of_stations[stationNum].socket_number=sock;


	int numOfBytes=1024;
	int byteRead;
	char buff[2000];

	//for measure time between each transmit
	long elapsed;
	struct timeval start, end;
	struct timeval wait;
	long interval_uSec=62500;
	//Define the head of the list of songs as the current playing
	list_of_stations[stationNum].current_playing_song=list_of_stations[stationNum].list_of_songs;

	do{
		//try to open the song
		if(!(currentSong=fopen(list_of_stations[stationNum].current_playing_song->song_filename,"r"))){
			perror("Can't open the file");
			close(sock);
			exit(EXIT_FAILURE);
		}
		list_of_stations[stationNum].list_of_songs->songfile=currentSong;

		while ((byteRead=fread(buff,1,numOfBytes,currentSong)) != 0){
			gettimeofday(&start, NULL);

			ssize_t numBytes=sendto(sock,buff,byteRead,0,(struct sockaddr *)&multiaddr,sizeof(multiaddr));
			if (numBytes==-1){
				perror("sendto() ERROR");
				fclose(currentSong);
				close(sock);
				exit(EXIT_FAILURE);
			}
			gettimeofday(&end, NULL);
			elapsed = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
			wait.tv_sec=0;
			wait.tv_usec=interval_uSec-elapsed;
			usleep(wait.tv_usec);
		}//while transmitting the song

		if (fclose(currentSong) == EOF){
			perror("Can't close the file");
			close(sock);
			exit(EXIT_FAILURE);
		}
		//update the next song as current playing song
		if(list_of_stations[stationNum].current_playing_song->nextsong)
			list_of_stations[stationNum].current_playing_song=list_of_stations[stationNum].current_playing_song->nextsong;
		else
			list_of_stations[stationNum].current_playing_song =list_of_stations[stationNum].list_of_songs;

	}while (1);

	return 0;
}

int close_udp_server(){
	int i;
	songlist* temp;
	for(i = 0 ; i<number_of_stations ; i++){
		pthread_cancel(list_of_stations[i].ptr);
		pthread_join(list_of_stations[i].ptr,NULL);
		close(list_of_stations[i].socket_number);
		fclose(list_of_stations[i].current_playing_song->songfile);
		while(list_of_stations[i].list_of_songs){
			temp = list_of_stations[i].list_of_songs;
			list_of_stations[i].list_of_songs = list_of_stations[i].list_of_songs->nextsong;
			free(temp);
		}
	}
	free(list_of_stations);
	pthread_cancel(UDP_Default_Thread);
	pthread_join(UDP_Default_Thread,NULL);
	return 0;
}

int print_UDP_data(){
	int i;
	printf(ANSI_COLOR_GREEN "List of Stations:\n" ANSI_COLOR_RESET);
	for(i = 0 ; i <  number_of_stations ; i++){
		printf("Channel %d:\nMulticast address: %s\nAt Port: %d\nCurrent Playing Song:%s\n",i,inet_ntoa(list_of_stations[i].multicast_address.sin_addr),Multicast_Port,list_of_stations[i].current_playing_song->song_filename);
	}
	return 0;
}
