/*
 * UDPConectionManage.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */
#include "UDPConectionManage.h"







void * udp_init_channels(void* arg){
	Station* stationList = (Station*)arg;
	pthread_t thread_arr[number_of_stations];
	int i,s;
	//open new threads as the number of stations
	for (i=0; i<number_of_stations; i++){
		struct ThreadArg *t_Arg=(struct ThreadArg*) malloc(sizeof(struct ThreadArg));
		if (t_Arg ==NULL){
			perror("malloc() failed");
			exit(EXIT_FAILURE);
		}
		t_Arg->stationNum=i;
		s=pthread_create(thread_arr[i],NULL,newStationTread,t_Arg);
		if (s!=0){
			perror("pthread_create() error");
			exit(EXIT_FAILURE);
		}

	}//for

	while (1){

	}//Run forever
	return 0;
}//udp_init_channels



void* newStationTread (void* arg){
	//Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
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
	multiaddr.sin_addr.s_addr = htonl(list_of_stations[stationNum].multicast_address);

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
	//try to open the song

	int numOfBytes=1400;
	int byteRead;
	char buff[2000];

	//for measure time between each transmit
	long elapsed;
	struct timeval start, end;
	struct timespec wait;
	long interval_nanoSec=87500000;
	//Define the head of the list of songs as the current playing
	list_of_stations[stationNum].current_playing_song=list_of_stations[stationNum].list_of_songs;

	do{
		if(!(currentSong=fopen(list_of_stations[stationNum].current_playing_song->song_filename,"r"))){
			perror("Can't open the file");
			close(sock);
			exit(EXIT_FAILURE);
		}
		list_of_stations[stationNum].list_of_songs->songfile=currentSong;

		while (currentSong!=EOF){
			gettimeofday(&start, NULL);
			byteRead=fread(buff,1,numOfBytes,currentSong);
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
			wait.tv_nsec=interval_nanoSec-elapsed*1000;
			nanosleep(&wait,NULL);
		}//while transmitting the song

		if (!fclose(currentSong)){
			perror("Can't close the file");
			close(sock);
			exit(EXIT_FAILURE);
		}
		//update the next song as current playing song
		list_of_stations[stationNum].current_playing_song=list_of_stations[stationNum].current_playing_song->nextsong;

	}
	while (1);
	return 0;
}
