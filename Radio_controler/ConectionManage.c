/*
 * ConectionManage.c
 *
 *  Created on: Jan 4, 2016
 *      Author: hershco
 */
#include "ConectionManage.h"

int open_tcp_concection(char* IP,char* Port){
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP);
	server_addr.sin_port = htons(atoi(Port));
	if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		perror("cannot open socket");
		exit(EXIT_FAILURE);
	}
	if (connect(sock, (struct  sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		perror("error connecting");
		close(sock);
		exit(EXIT_FAILURE);
	}
	return 0;
}

int open_radio_sc_conection(){
	RadioCommand hello;
	int commandtype_length,reserved_length;
	commandtype_length = sizeof(hello.commandType);
	reserved_length =sizeof(hello.stationNumber);
	hello.commandType = 0;
	if(send(sock,hello.commandType,&commandtype_length,0)== -1){
		perror("error sending hello message");
		close(sock);
		exit(EXIT_FAILURE);
	}
	if(send(sock,hello.stationNumber,&reserved_length,0) == -1){
		perror("error sending hello message");
		close(sock);
		exit(EXIT_FAILURE);
	}
	return 0;
}

int send_ask_song(uint16_t channel){
	RadioCommand ask;
	int commandtype_length,reserved_length;
	commandtype_length = sizeof(ask.commandType);
	reserved_length =sizeof(ask.stationNumber);
	ask.commandType = 1;
	ask.stationNumber = channel;
	if(send(sock,ask.commandType,&commandtype_length,0)== -1){
		perror("error sending ask message");
		close(sock);
		exit(EXIT_FAILURE);
	}
	if(send(sock,ask.stationNumber,&reserved_length,0) == -1){
		perror("error sending ask message");
		close(sock);
		exit(EXIT_FAILURE);
	}
	return 0;
}


int state_machine(char* IP,char* Port){
	fd_set socks;
	int select_answer;
	int result;
	struct timeval timeout;
	switch(status){
		case CONNECT:
		{
			FD_ZERO(&readset);
			open_tcp_concection(IP,Port);
			FD_SET(sock, &socks);
			FD_SET(0,&socks);
			status = HELLO;
			break;
		}
		case HELLO:
		{
			int open_radio_sc_conection();
			timeout.tv_usec = TIMEOUT;
			if((select_answer = select(sock+1,&socks,NULL,NULL,&timeout)) < 0){
				perror("select");
				close(sock);
				exit(EXIT_FAILURE);
			}
			else if (select_answer ==0){
				printf("Timeout while waiting for the server welcome message");
				printf("Exiting...");
			}
			else{
				if (FD_ISET(0,&socks)){
					result = handle_user_input();
					if(result == 0){
						status = DISCONNECT;
					}
				}
				if(FD_ISET(sock,&socks)){
					if((result = recive_msg()) < 0){
						status = DISCONNECT;
					}
				}
			}
			break;
		}
		case CONNECTED:
		{

		}
		case FAIL:
		{

		}
		case DISCONNECT:
		{

		}
	}
}
