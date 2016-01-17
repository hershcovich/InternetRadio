/*
 * TCPConectionManage.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#include "TCPConectionManage.h"

void *tcp_radio_welcome(void* arg){
	socketnode* welcome=NULL;
	socketnode* tmpsocket=NULL;
	int newsocket;
	unsigned int sizofnewsocket;
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
	if(bind(welcome->sock,(struct sockaddr*)&listen_addr,sizeof(listen_addr))<0){
		perror("error binding welcome socket");
		close(welcome->sock);
		free(welcome);
	}
	listen(welcome->sock,10);
	while(1){
		sizofnewsocket =sizeof(newsocket);
		newsocket = accept(welcome->sock,(struct sockaddr*)&newconnection,&sizofnewsocket);
		tmpsocket = (socketnode*)malloc(sizeof(socketnode));
		tmpsocket->nextsocket=socketlist;
		socketlist=tmpsocket;
		socketlist->sock=newsocket;
		socketlist->address=newconnection;
		pthread_create(&(socketlist->pth),NULL,tcp_radio_conection,socketlist);
	}
	return 0;
}

void *tcp_radio_conection(void* arg){
	socketnode* sonode = (socketnode*)arg;
	if(0 > hello_handshake(sonode)){
		close_socket_node(sonode);
		pthread_exit(NULL);
	}
	while(1){
		if(server_client_interaction(sonode)<0){
			printf("Error..closing socket %d\n",sonode->sock);
			close_socket_node(sonode);
			pthread_exit(NULL);
		}
	}
	return 0;
}

int hello_handshake(socketnode *sonode){
	fd_set fd;
	Welcome_msg welcome;
	InvalidCommand_msg invalid;
	int select_answer,numRecvBytes;
	struct timeval timeout;
	char buf[BUFFER_SIZE],sendbuffer[BUFFER_SIZE];
	timeout.tv_usec = 0;
	timeout.tv_sec = TIMEOUT;
	FD_ZERO(&fd);
	FD_SET((sonode->sock),&fd);
	select_answer=select((sonode->sock) + 1,&fd,NULL,NULL,&timeout);
	if(select < 0){
		perror("error while select");
		close(sonode->sock);
		free(sonode);
		exit(EXIT_FAILURE);
	}
	else if(select_answer == 0){
		//timeout while waiting to hello msg from the client
		printf("Error: Time out while waiting to welcome msg from client\nDetails:\nsocket:%d,ip:%s\nclosing socket...\n",(sonode->sock),inet_ntoa((sonode->address.sin_addr)));
		return -1;
	}
	else{
		if((numRecvBytes = recv(sonode->sock,&buf,sizeof(buf),0)) < 0){
			perror("error receiving hello msg");
			close(sonode->sock);
			free(sonode);
			exit(EXIT_FAILURE);
		}
		else if(numRecvBytes ==0 ){
			printf("client close the connection..\n");
			return -1;
		}
		else{
			if(buf[0] == 0){
				if(buf[1] == 0 && buf[2] == 0){
					printf("hello message received from %s\n",inet_ntoa(sonode->address.sin_addr));
					welcome.replayType = 0;
					welcome.numStations = htons(number_of_stations);
					welcome.portNumber =htons(Multicast_Port);
					welcome.multicastGroup = htonl((list_of_stations[0].multicast_address));
					sendbuffer[0] =welcome.replayType;
					sendbuffer[1] =welcome.numStations;
					sendbuffer[3] =welcome.multicastGroup;
					sendbuffer[7]= welcome.portNumber;
					sendbuffer[9] ='\0';
					send((sonode->sock),sendbuffer,sizeof(welcome),0);
					printf("welcome message sent to: %s\n",inet_ntoa(sonode->address.sin_addr));
				}
			}
			else if(buf[0] == 1){
				printf("error! Received an asksong message before hello! socket: %d client IP: %s\n",sonode->sock,inet_ntoa(sonode->address.sin_addr));
				invalid.replyType =2;
				invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received asksong message before hello message!"));
				strcpy(invalid.replyString,"Invalid Condition! server received asksong message before hello message!");
				invalid.replatStringSize = strlen(invalid.replyString);
				sendbuffer[0] = invalid.replyType;
				sendbuffer[1] = invalid.replatStringSize;
				strcpy(&sendbuffer[2], invalid.replyString);
				send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
				printf("error message sent to the client\n");
				return -1;
			}
			else
			{
				printf("Received an unknown message type from %s\n",inet_ntoa(sonode->address.sin_addr));
				invalid.replyType =2;
				invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received unknown message type!"));
				strcpy(invalid.replyString,"Invalid Condition! server received unknown message type!");
				invalid.replatStringSize = strlen(invalid.replyString);
				sendbuffer[0] = invalid.replyType;
				sendbuffer[1] = invalid.replatStringSize;
				strcpy(&sendbuffer[2], invalid.replyString);
				send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
				printf("error message sent to the client\n");
				return -1;
			}
			return 0;
		}

	}

}

int close_socket_node(socketnode *sonode){
	socketnode* search = socketlist,*tmp;
	if (sonode == search){
		close(sonode->sock);
		socketlist = socketlist->nextsocket;
		free(sonode);
		return 0;
	}
	while(search->nextsocket != sonode){
		search = search->nextsocket;
	}
	tmp = search->nextsocket;
	search->nextsocket = search->nextsocket->nextsocket;
	close((tmp->sock));
	free(tmp);
	return 0;
}

int server_client_interaction(socketnode *sonode){
	Announce_msg announce;
	InvalidCommand_msg invalid;
	int numRecvBytes;
	char buf[BUFFER_SIZE],sendbuffer[BUFFER_SIZE];
	if((numRecvBytes = recv(sonode->sock,&buf,sizeof(buf),0)) < 0){
		perror("error receiving hello msg");
		close(sonode->sock);
		free(sonode);
		exit(EXIT_FAILURE);
	}
	else if(numRecvBytes ==0 ){
		printf("client close the connection..\n");
		return -1;
	}
	else{
		if(buf[0] == 1){
			if((uint16_t)buf[1] >= 0 && (uint16_t)buf[1] < number_of_stations){
				printf("Received askSong message from %s, Station number: %hu\n",inet_ntoa(sonode->address.sin_addr),ntohs((uint16_t)buf[1]));
				announce.replyType = 1;
				announce.songname = what_song_is_played((uint16_t)buf[1]);
				announce.songnameSize=strlen(announce.songname);
				sendbuffer[0] =announce.replyType;
				sendbuffer[1] =announce.songnameSize;
				strcpy(&sendbuffer[2],announce.songname);
				send((sonode->sock),sendbuffer,announce.songnameSize + 2,0);
				printf("announce message sent to: %s\n",inet_ntoa(sonode->address.sin_addr));
				return 0;
			}
			else{
				printf("Received an asksong with non-exisit station number %hu\n from %s",ntohs((uint16_t)buf[1]),inet_ntoa(sonode->address.sin_addr));
				invalid.replyType =2;
				invalid.replyString = (char*)malloc(sizeof("Invalid Condition! Station  XXXXX doesn't exist!"));
				sprintf(invalid.replyString,"Invalid Condition! Station %hu doesn't exist!",ntohs((uint16_t)buf[1]));
				invalid.replatStringSize = strlen(invalid.replyString);
				sendbuffer[0] = invalid.replyType;
				sendbuffer[1] = invalid.replatStringSize;
				strcpy(&sendbuffer[2], invalid.replyString);
				send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
				printf("error message sent to the client\n");
				return -1;
			}
		}
		else if(buf[0] == 0){
			printf("error! Received duplicate hello message socket: %d client IP: %s\n",sonode->sock,inet_ntoa(sonode->address.sin_addr));
			invalid.replyType =2;
			invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received duplicate hello message!"));
			strcpy(invalid.replyString,"Invalid Condition! server received duplicate hello message!");
			invalid.replatStringSize = strlen(invalid.replyString);
			sendbuffer[0] = invalid.replyType;
			sendbuffer[1] = invalid.replatStringSize;
			strcpy(&sendbuffer[2], invalid.replyString);
			send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
			printf("error message sent to the client\n");
			return -1;
		}
		else
		{
			printf("Received an unknown message type from %s\n",inet_ntoa(sonode->address.sin_addr));
			invalid.replyType =2;
			invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received unknown message type!"));
			strcpy(invalid.replyString,"Invalid Condition! server received unknown message type!");
			invalid.replatStringSize = strlen(invalid.replyString);
			sendbuffer[0] = invalid.replyType;
			sendbuffer[1] = invalid.replatStringSize;
			strcpy(&sendbuffer[2], invalid.replyString);
			free(invalid.replyString);
			send((sonode->sock),sendbuffer,invalid.replatStringSize+ 2,0);
			printf("error message sent to the client\n");
			return -1;
		}
		return 0;
	}
}

char* what_song_is_played(int Station){
	if(Station >= number_of_stations){
		return NULL;
	}
	else
		return (list_of_stations[Station].current_playing_song->song_filename);

}
