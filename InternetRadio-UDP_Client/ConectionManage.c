/*
 * ConectionManage.c
 *
 *  Created on: Dec 30, 2015
 *      Author: hershco
 */
#include "ConectionManage.h"


int Play_MultiCast_Stream(char *IP_ADDR , char* IP_Port){
	int sock,select_toread;
	struct sockaddr_in multiaddr;
	struct ip_mreq mreq;
	fd_set socks;
	FD_ZERO(&socks);
	multiaddr.sin_family = AF_INET;
	multiaddr.sin_port = htons(atoi(IP_Port));
	multiaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0){
		perror("failed to open socket");
		exit(1);
	}
	bind(sock,(struct sockaddr *)&multiaddr,sizeof(multiaddr));

	mreq.imr_multiaddr.s_addr = inet_addr(IP_ADDR);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
	FD_SET(sock,&socks);
	FD_SET(0,&socks);
	//inet_pton(AF_INET,IP_ADDR,&multiaddr.sin_addr);
	while(1){
		select_toread = select(sock +1, &socks,NULL,NULL,NULL); //wait for input from stdin or netwirk
		if(select_toread < 0){
			perror("there was problem in select function");
			exit(EXIT_FAILURE);
		}
		else if(select_toread > 0){
			if(read_socks(sock,&socks,&multiaddr) == 1){
				setsockopt(sock,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq));
				close(sock);
				exit(EXIT_SUCCESS);
			}
		}
	}
	return 0;
}

int read_socks(int  sock, fd_set *socks ,struct sockaddr_in* addr){
	int quit_flag = 0 ;
	if(FD_ISSET(0,socks))
		quit_flag = handle_user_input();
	if(FD_ISSET(sock,socks)){
		handle_multicast_recive(sock,addr);
	}
	return quit_flag;
}

int handle_user_input(){
	char user_input;
	user_input = getchar();
	if (user_input == 'q'){
		user_input = getchar();
		if (user_input == '\n'){
			return 1;
		}
		else{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int handle_multicast_recive(int sock ,struct sockaddr_in* addr)
{
	char databuf[1024];
	int datalen = sizeof(databuf);
	int addrlen = sizeof(*addr);
	recvfrom(sock,databuf,datalen,0,addr,&addrlen);
	puts(databuf);
	return 0;
}
