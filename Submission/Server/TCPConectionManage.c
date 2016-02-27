/*
 * TCPConectionManage.c
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#include "TCPConectionManage.h"
/**
 * TCP main function
 * Creates a welcome socket and wait for new connection
 * open new thread for each connection
 */
void *tcp_radio_welcome(void* arg){
	int newsocket;
	unsigned int sizofnewsocket;
	struct sockaddr_in listen_addr,newconnection;
	welcomesocket = NULL; // a node to for the default welcome socket
	tmpsocket  = NULL; //a socket for new conections
	//socketlist = NULL;
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr=INADDR_ANY;
	listen_addr.sin_port=htons(TCP_Port);

	/*allocate anew socketnode for the welcome*/
	if(!(welcomesocket = (socketnode*)malloc(sizeof(socketnode)))){
		perror("memory allocation error");
		exit(EXIT_FAILURE);
	}
	/*create new welcome socket*/
	if((welcomesocket->sock=socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("error creating welcome socket");
		free(welcomesocket);
		exit(EXIT_FAILURE);
	}
	/*bind the socket*/
	if(bind(welcomesocket->sock,(struct sockaddr*)&listen_addr,sizeof(listen_addr))<0){
		perror("error binding welcome socket");
		close(welcomesocket->sock);
		free(welcomesocket);
		exit(EXIT_FAILURE);
	}
	printf("tcp server listen at port %d\n",TCP_Port);
	/*listen to the socket*/
	if(listen(welcomesocket->sock,10) < 0 ){
		perror("error listen()");
		close(welcomesocket->sock);
		free(welcomesocket);
		exit(EXIT_FAILURE);
	}
	/*start the new clients routine*/
	while(1){
		sizofnewsocket = sizeof(struct sockaddr_in);
		newsocket = accept(welcomesocket->sock,(struct sockaddr*)&newconnection,&sizofnewsocket); //wait for new connections
		if(newsocket < 0){//in case of error
			perror("error accept()");
			close(welcomesocket->sock);
			free(welcomesocket);
			exit(EXIT_FAILURE);
		}
		/* allocate new socketnode for thee new connection*/
		if(!(tmpsocket = (socketnode*)malloc(sizeof(socketnode)))){
			perror("error malloc()");
			exit(0);
		}
		/* add the new socket node to the list of sockets*/
		tmpsocket->nextsocket=socketlist;
		socketlist=tmpsocket;
		socketlist->sock=newsocket;
		socketlist->address=newconnection;
		/*create a new thread for the new socket*/
		if(pthread_create(&(socketlist->pth),NULL,tcp_radio_conection,socketlist)){
			perror("error create thread)"); //in case of error
			close(welcomesocket->sock);
			free(welcomesocket);
			exit(EXIT_FAILURE);
		}
	}//while
	return 0;
}
/**
 * the main thread function
 * function for each client connection
 * @param arg the socketnode for this connection.
 */
void *tcp_radio_conection(void* arg){
	socketnode* sonode = (socketnode*)arg;
	int ans;
	printf("New client connection expecting hello\n");
	if(0 >= (ans = hello_handshake(sonode))){ //enter client server handshake routine
		close_socket_node(sonode); //in case of error or client close the connection
		pthread_detach(pthread_self());
		return 0;
	}
	while(1){ //interact with the client
		if((ans = server_client_interaction(sonode))<=0){
			if (ans == -1)
				printf("Error..closing socket %d\n",sonode->sock);
			close_socket_node(sonode); //in case of error or client close the connection
			pthread_detach(pthread_self());
			return 0;
		}
	}
	return 0;
}
/**
 * this function manage the hello-handshake process for each client
 * @param sonode the socket node of this connection
 * @return
 */
int hello_handshake(socketnode *sonode){
	fd_set fd; // file descriptor set for select function for the timmer
	Welcome_msg welcome;
	InvalidCommand_msg invalid;
	int select_answer,numRecvBytes;
	struct timeval timeout; //struct for the timmer
	char buf[BUFFER_SIZE],sendbuffer[BUFFER_SIZE];
	timeout.tv_usec = 0;
	timeout.tv_sec = TIMEOUT; //set the timmou value
	/*using select for masuring timeout between client connection and reciving hello msg*/
	FD_ZERO(&fd);
	FD_SET((sonode->sock),&fd);
	select_answer=select((sonode->sock) + 1,&fd,NULL,NULL,&timeout);
	if(select_answer < 0){
		//error in the select process
		perror("error while select");
		close(sonode->sock);
		free(sonode);
		exit(EXIT_FAILURE);
	}
	else if(select_answer == 0){
		//timeout while waiting to hello msg from the client
		printf("Error: Time out while waiting to hello msg from client\nDetails:\nsocket:%d,ip:%s\nclosing socket...\n",(sonode->sock),inet_ntoa((sonode->address.sin_addr)));
		return -1;
	}
	else{
		//every thing cool, the client respond in time

		/* trying receive client msg*/
		if((numRecvBytes = recv(sonode->sock,&buf,sizeof(buf),0)) < 0){
			//error while reciving
			perror("error receiving hello msg");
			close(sonode->sock);
			free(sonode);
			exit(EXIT_FAILURE);
		}
		else if(numRecvBytes ==0 ){
			//client has close the connection
			printf("client close the connection..\n");
			return 0;
		}
		else{
			//receiving msg
			if(buf[0] == 0){
				if(buf[1] == 0 && buf[2] == 0){ //in case the msg was hello has expected
					printf("hello message received from %s\n",inet_ntoa(sonode->address.sin_addr));
					/* create a welcome replay and send it*/
					welcome.replayType = 0;
					welcome.numStations = htons(number_of_stations);
					welcome.portNumber =htons(Multicast_Port);
					welcome.multicastGroup = htonl((multicast_address.s_addr));
					memcpy(&sendbuffer[0],&welcome.replayType,1);
					memcpy(&sendbuffer[1],&welcome.numStations,2);
					memcpy(&sendbuffer[3],&welcome.multicastGroup,4);
					memcpy(&sendbuffer[7],&welcome.portNumber,2);
					sendbuffer[9] ='\0';
					send((sonode->sock),sendbuffer,9,0);
					printf("welcome message sent to: %s\n",inet_ntoa(sonode->address.sin_addr));
				}
			}
			else if(buf[0] == 1){
				//in case client send ask song msg before hello
				printf("error! Received an asksong message before hello! socket: %d client IP: %s\n",sonode->sock,inet_ntoa(sonode->address.sin_addr));
				//create an invalid msg reply and send it to the client
				invalid.replyType =2;
				if(!(invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received asksong message before hello message!")))){
					perror("error malloc()");
					exit(1);
				}
				strcpy(invalid.replyString,"Invalid Condition! server received asksong message before hello message!");
				invalid.replatStringSize = strlen(invalid.replyString);
				memcpy(&sendbuffer[0],&invalid.replyType,1);
				memcpy(&sendbuffer[1],&invalid.replatStringSize,1);
				strcpy(&sendbuffer[2], invalid.replyString);
				send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
				printf("error message sent to the client\n");
				free(invalid.replyString);
				return -1;
			}
			else
			{
				//in case of unknown msg
				printf("Received an unknown message type from %s\n",inet_ntoa(sonode->address.sin_addr));
				//create an invalid msg reply and send it to the client
				invalid.replyType =2;
				if(!(invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received unknown message type!")))){
					perror("error malloc ()");
					exit(1);
				}
				strcpy(invalid.replyString,"Invalid Condition! server received unknown message type!");
				invalid.replatStringSize = strlen(invalid.replyString);
				sendbuffer[0] = invalid.replyType;
				sendbuffer[1] = invalid.replatStringSize;
				strcpy(&sendbuffer[2], invalid.replyString);
				send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
				printf("error message sent to the client\n");
				free(invalid.replyString);
				return -1;
			}
			return 1;
		}

	}

}
/**
 * this function close socket node struct
 * close the socket related to it and release it's memory
 * @param sonode the socket node to close
 * @return 0
 */
int close_socket_node(socketnode *sonode){
	socketnode* search = socketlist,*tmp;
	/*search for the socket node in the socket node link list*/
	/*close it and release memory */
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

/**
 * manage client server interaction
 * @param sonode the conection related socket node
 * @return
 */

int server_client_interaction(socketnode *sonode){
	Announce_msg announce;
	InvalidCommand_msg invalid;
	int numRecvBytes;
	uint16_t req_channel;
	char buf[BUFFER_SIZE],sendbuffer[BUFFER_SIZE];
	//try to read for the buffer
	if((numRecvBytes = recv(sonode->sock,&buf,sizeof(buf),0)) < 0){ // in case of error
		perror("error receiving msg");
		close(sonode->sock);
		free(sonode);
		exit(EXIT_FAILURE);
	}
	else if(numRecvBytes ==0 ){ // in case client close the connection
		printf("client close the connection..\n");
		return 0;
	}
	else{ //in case of successes
		if(buf[0] == 1){
			req_channel = ntohs(*(uint16_t*)(buf + 1));
			if(req_channel >= 0 && req_channel < number_of_stations){
				//receive correct ask song message
				printf("Received askSong message from %s, Station number: %hu\n",inet_ntoa(sonode->address.sin_addr),req_channel);
				//set announce message and sent it to the client
				announce.replyType = 1;
				announce.songname = what_song_is_played(req_channel);
				announce.songnameSize=strlen(announce.songname);
				sendbuffer[0] =announce.replyType;
				sendbuffer[1] =announce.songnameSize;
				strcpy(&sendbuffer[2],announce.songname);
				send((sonode->sock),sendbuffer,announce.songnameSize + 2,0);
				printf("announce message sent to: %s\n",inet_ntoa(sonode->address.sin_addr));
				return 1;
			}
			else{
				//receive ask song for non exist channel
				printf("Received an asksong with non-exisit station number %hu from %s\n",req_channel,inet_ntoa(sonode->address.sin_addr));
				//set invalid message and send it to the client
				invalid.replyType =2;
				if(!(invalid.replyString = (char*)malloc(sizeof("Invalid Condition! Station  XXXXX doesn't exist!")))){ //xxxxx for maximum channel size
					perror("error malloc ()");
					exit(1);
				}
				sprintf(invalid.replyString,"Invalid Condition! Station %hu doesn't exist!",req_channel);
				invalid.replatStringSize = strlen(invalid.replyString);
				sendbuffer[0] = invalid.replyType;
				sendbuffer[1] = invalid.replatStringSize;
				strcpy(&sendbuffer[2], invalid.replyString);
				send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
				printf("error message sent to the client\n");
				free(invalid.replyString);
				return -1;
			}
		}
		else if(buf[0] == 0){ // in case of hello message after handshake
			printf("error! Received duplicate hello message socket: %d client IP: %s\n",sonode->sock,inet_ntoa(sonode->address.sin_addr));
			//set invalid message and send it to the client
			invalid.replyType =2;
			if(!(invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received duplicate hello message!")))){
				perror("error malloc ()");
				exit(1);
			}
			strcpy(invalid.replyString,"Invalid Condition! server received duplicate hello message!");
			invalid.replatStringSize = strlen(invalid.replyString);
			sendbuffer[0] = invalid.replyType;
			sendbuffer[1] = invalid.replatStringSize;
			strcpy(&sendbuffer[2], invalid.replyString);
			send((sonode->sock),sendbuffer,invalid.replatStringSize + 2,0);
			printf("error message sent to the client\n");
			free(invalid.replyString);
			return -1;
		}
		else
		{   //in case of unrecognized message
			printf("Received an unknown message type from %s\n",inet_ntoa(sonode->address.sin_addr));
			//set invalid message and send it to the client
			invalid.replyType =2;
			if(!(invalid.replyString = (char*)malloc(sizeof("Invalid Condition! server received unknown message type!")))){
				perror("error malloc ()");
				exit(1);
			}
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
		return 1;
	}
}
/**
 * return the name of the song that played in specific channel
 * @param Station the channel asked about
 * @return the name of the channel or NULL in case the channel dose'nt exist
 */
char* what_song_is_played(uint16_t Station){
	if(Station >= number_of_stations){
		return NULL;
	}
	else
		return (list_of_stations[Station].current_playing_song->song_filename);

}
/**
 * close the tcp server
 * release all the memory of all the socket nodes, close all the tcp related socket and close all the tcp related threads
 * @return 0
 */

int close_TCP_server(){
	socketnode* temp;
	while(socketlist){ // release the clients socketnode list
		pthread_cancel(socketlist->pth);
		pthread_join(socketlist->pth,NULL);
		close(socketlist->sock);
		temp = socketlist;
		socketlist =socketlist->nextsocket;
		free(temp);
	} //release tcp main thread
	pthread_cancel(TCP_Default_Thread);
	pthread_join(TCP_Default_Thread,NULL);
	if(welcomesocket){
		close(welcomesocket->sock);
		free(welcomesocket);
	} //release temp socket if in the middle of connecting to a new client
//	if(tmpsocket){
//		close(tmpsocket->sock);
//		free(tmpsocket);
//	}
	return 0;
}

int print_TCP_data(){
	socketnode* to_print;
	int client_number = 1;
	printf("TCP server Listening on port: %d\n",TCP_Port);
	printf(ANSI_COLOR_GREEN "Client List:\n" ANSI_COLOR_RESET);
	to_print = socketlist;
	if(!to_print)
		puts("there are no clients at this moment");
	else{
		while (to_print){
			printf("%d)%s\n",client_number,inet_ntoa(to_print->address.sin_addr));
			client_number++;
			to_print = to_print->nextsocket;
		}
	}
	return 0;

}
