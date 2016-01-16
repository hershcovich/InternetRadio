/*
 * TCPConectionManage.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#include "TCPConectionManage.h"

int tcp_radio_welcome(void* arg){
	socketnode* welcome=NULL;
	socketnode* socketlist=NULL;
	socketnode* tmpsocket=NULL;
	int newsocket,sizofnewsocket;
	struct sockaddr_in listen_addr,newconnection;
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr=INADDR_ANY;
	listen_addr.sin_port=htons(TCP_Port);


	if(!(welcome = (socketnode*)malloc(sizeof(socketnode)))){
		perror("memory allocation error");
		exit(EXIT_FAILURE);
	}
	if((welcome->sock=socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("error creating welcome socket");
		free(welcome);
		exit(EXIT_FAILURE);
	}
	if(bind(welcome->sock,(struct socaddr *)&listen_addr,sizeof(listen_addr))<0){
		perror("error binding welcome socket");
		close(welcome->sock);
		free(welcome);
	}
	listen(welcome->sock,10);
	while(1){
		sizofnewsocket =sizeof(newsocket);
		newsocket = accept(welcome->sock,(struct socaddr *)&newconnection,&sizofnewsocket);
		tmpsocket = (socketnode*)malloc(sizeof(socketnode));
		tmpsocket->nextsocket=socketlist;
		socketlist=tmpsocket;
		socketlist->sock=newsocket;
		socketlist->address=newconnection;
		pthread_create(&(socketlist->pth),NULL,&tcp_radio_conection,socketlist);
	}
	return 0;
}


void tcp_radio_conection(void* arg){
	return 0;
}


