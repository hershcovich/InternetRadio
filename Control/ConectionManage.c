/*
 * ConectionManage.c
 *
 *  Created on: Jan 4, 2016
 *      Author: hershco
 */
#include "ConectionManage.h"
/**
 * This function open new connection to the radio_server
 * @param IP the IP of the server
 * @param Port the port of the server
 * @return
 */
int open_tcp_concection(char* IP,char* Port){
	/*set the server address and parms*/
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP);
	server_addr.sin_port = htons(atoi(Port));
	/*Create a Socket*/
	if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
		perror("cannot open socket"); //if problem occurred
		exit(EXIT_FAILURE);
	}//if((sock..
	/*connect to the socket*/
	if (connect(sock, (struct  sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		perror("error connecting");//if problem occurred
		close(sock);
		exit(EXIT_FAILURE);
	}//if(connect
	return 0;
}

/**
 * this function send the hello handshake of the radio
 * @return
 */
int open_radio_sc_conection(){
	RadioCommand hello;
	char tosend[sizeof(hello.commandType)+sizeof(hello.stationNumber)];
	uint8_t* comtype_tosend = ((uint8_t*)&tosend[0]);
	uint16_t* reserved_tosend = ((uint16_t*)&tosend[1]);
	/* set new hello message*/
	int tosend_length =sizeof(tosend);
	hello.commandType = 0;
	hello.stationNumber =0;
	*comtype_tosend = hello.commandType;
	*reserved_tosend = hello.stationNumber;
	/*send the first part of the message(the Type field)*/
	if(send(sock,tosend,tosend_length,0) == -1){
		perror("error sending hello message");
		close(sock);
		exit(EXIT_FAILURE);
	}//if(send..
	return 0;
}

/**
 * this function sends the ask_song message
 * @param channel = channel to ask
 * @return
 */
int send_ask_song(uint16_t channel){
	RadioCommand ask;
	char tosend[sizeof(ask.commandType)+sizeof(ask.stationNumber)];
	uint8_t* comtype_tosend = ((uint8_t*)&tosend[0]);
	uint16_t* stationNumber_tosend = ((uint16_t*)&tosend[1]);
	/* set new ask message*/
	int tosend_length =sizeof(tosend);
	ask.commandType = 1;
	ask.stationNumber =htons(channel);
	*comtype_tosend = ask.commandType;
	*stationNumber_tosend = ask.stationNumber;
	/*send the message*/
	if(send(sock,tosend,tosend_length,0) == -1){
		perror("error sending hello message");
		close(sock);
		exit(EXIT_FAILURE);
	}//if(send..
	return 0;
}
/**
 * function to handle the user input
 * @return the user choice to do, -1 for exiting 0-.. for ask channel number.
 */
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

	else if(strcmp(input,"\n") ==0){
		user_request =-2;
	}
	else if(input[0] < 48 || input[0] > 57){
		user_request =-2;
	}
	else{
		user_request= atoi(input);
	}
	if(user_request <0 || user_request >= MAX_NUM_OF_STAITONS){
		printf("Please select a channel between 0 to %d or press q to exit\n",MAX_NUM_OF_STAITONS -1);
		user_request = -2;
	}
	return user_request;
}

/**
 * This function read message from the socket
 * @return 1 for success , 0 if socket is close.
 */
int recive_msg(){
	const int bufSiz = 1024;
	int numBytes;
	uint16_t numStations, portNum;
	char buf[bufSiz],stringToPrint[256];
	struct in_addr multicastGroup;

	if ((numBytes=recv(sock,buf,bufSiz,0))<0){
		perror("error receiving message");
		close(sock);
		exit(EXIT_FAILURE);
	}//if
	if (numBytes==0) return 0;
	if (status==HELLO){
		if	(buf[0]==0 && numBytes==9){
			memcpy(&numStations,buf+1,2);
			numStations=ntohs(numStations);
			memcpy(&multicastGroup,buf+3,4);
			memcpy(&portNum,buf+7,2);
			portNum=ntohs(portNum);
			multicastGroup.s_addr = ntohl(multicastGroup.s_addr);
			printf("MC group is: %s \n" ,inet_ntoa(multicastGroup));
			printf("There are %d stations\n",numStations);
			printf("The m.c port to listen to is %d\n",portNum);
			number_of_stations =numStations;
		}// if buf
		else if (buf[0]==2){
			memcpy(&stringToPrint,buf+2,buf[1]);
			stringToPrint[(int)buf[1]]=0;//add sign end of string
			printf("INVALID_COMMAND_REPLY: %s\nExiting..\n",stringToPrint);
			close(sock);
			exit(EXIT_FAILURE);
		}
		else {
			perror("error incorrect replay type message");
			close(sock);
			exit(EXIT_FAILURE);
		}//else
	}//if(status
	else{
		if(buf[0]==1 && numBytes==2+buf[1]){//check if the songnameSize is correct
			memcpy(&stringToPrint,buf+2,buf[1]);
			stringToPrint[(int)buf[1]]=0;//add sign end of string
			printf("Now Playing: %s\n",stringToPrint);
		}
		else if (buf[0]==2){
			memcpy(&stringToPrint,buf+2,buf[1]);
			stringToPrint[(int)buf[1]]=0;//add sign end of string
			printf("INVALID_COMMAND_REPLY: %s\nExiting..\n",stringToPrint);
			close(sock);
			exit(EXIT_FAILURE);
		}
		else
			return -1;
	}
	return 1;

}//recive_msg


/**
 * the main routine of the program
 * Running by states machine
 *
 * @param IP the IP of the server
 * @param Port the port of the server
 * @return never return
 */

int state_machine(char* IP,char* Port){
	fd_set socks; //set of file descriptors for check
	int select_answer; //when ruining select for checking the answer
	int result; //when calling to function temporary for holding the result
	struct timeval timeout; //time out for the select
	/* the state machine*/
	while(1){
		switch(status){
			case CONNECT:
			{
				FD_ZERO(&socks); //reset the file descriptor
				open_tcp_concection(IP,Port); //connect to the server
				/*set sock and stdin to the fd set*/
				FD_SET(sock, &socks);
				FD_SET(0,&socks);
				status = HELLO; //continue to hello
				break;
			}//connect
			case HELLO:
			{
				open_radio_sc_conection(); //send and hello msg
				timeout.tv_sec = TIMEOUT; //set the time out
				timeout.tv_usec = 0;
				select_answer = select(sock+1,&socks,NULL,NULL,&timeout);
				if(select_answer < 0){ //sleep on select and in case of failure:
					perror("select");
					close(sock);
					exit(EXIT_FAILURE);
				}//if((select_answer..
				else if (select_answer ==0){ //timeout occurred
					printf("Timeout while waiting for the server welcome message\n");
					printf("Exiting...\n");
					exit(EXIT_FAILURE);
				}//else if(select_answer..
				else{ //check which fd wake you
					if (FD_ISSET(0,&socks)){
						result = handle_user_input(); //collect user input
						if(result == -1){ //if the user ask to quit
							status = DISCONNECT;
						}//if(result..
					}//if(FD_ISSET(0..
					if(FD_ISSET(sock,&socks)){ //Receive msg for sock
						if((result = recive_msg()) < 0){ //if the msg type wasn't good
							status = DISCONNECT;
						}//if((result..
						else //in case the msg was welcome
							status = CONNECTED;
					}//if(FD_ISSET(sock..
				}//else
				break;
			}//hello
			case CONNECTED:
			{
				/*set the fd set*/
				FD_ZERO(&socks);
				FD_SET(sock, &socks);
				FD_SET(0,&socks);
				/*wait for event*/
				select_answer = select(sock+1,&socks,NULL,NULL,NULL);
				if(select_answer < 0){ //wait for user input or for socket event
					perror("select");
					close(sock);
					exit(EXIT_FAILURE);
				}//if((select_answer
				else{
					if(FD_ISSET(0,&socks)){ //if there's a user input
						result = handle_user_input();
						if(result == -1){ //if the user ask to quit
							status = DISCONNECT;
						}//if(result==-1
						else if (result >= 0){ //if the user ask for son info
							status = ASK_SONG;
						}//else if(result >=0..
					}//if(FD_ISSET(0..
					else if(FD_ISSET(sock,&socks)){
						if((result = recive_msg()) < 0){
							status = DISCONNECT;
						}//if((result = ..
						else if(result == 0){
							puts("the Server close the connection");
							status = DISCONNECT;
						}//else if(result
						else{
							puts("Message received without asking for it");
							status = DISCONNECT ;
						}//else..
					}//if(FD_ISSET..
				}//else
				break;
			}//connected
			case ASK_SONG:
			{
				send_ask_song(result);
				timeout.tv_sec = TIMEOUT; //wait for answer from the server
				timeout.tv_usec = 0;
				FD_ZERO(&socks);
				FD_SET(sock, &socks);
				FD_SET(0,&socks);
				if((select_answer = select(sock+1,&socks,NULL,NULL,&timeout)) < 0){
					perror("select");
					close(sock);
					exit(EXIT_FAILURE);
				}//if(select_answer..
				else if (select_answer ==0){ //in case of timeout
					printf("Timeout while waiting for the server announce message\n");
					printf("Exiting...\n");
					close(sock);
					exit(EXIT_FAILURE);
				}//else if(select_answer..
				else{
					if (FD_ISSET(0,&socks)){ //if there's a user input while waiting
						result = handle_user_input();
						if(result == -1){
							status = DISCONNECT;
						}//if(result==-1
					}//if(FD_ISSET(0..
					if(FD_ISSET(sock,&socks)){ //if there's a socket event
						if((result = recive_msg()) < 0){ //handle msg if there's a problem Disconnect and exit
							status = DISCONNECT;
						}//if(result = recive_msg..
						else if(result == 0){ //if the socket closed
							puts("the Server close the connection");
							status = DISCONNECT;
						}//else if(result..
						else
							status = CONNECTED;
					}//if(FD_SET(sock..
				}//else
				break;
			}//ASK_SONG
			case DISCONNECT:
			{
				close(sock); //close the open socket
				printf("Thanks for using radio_controller..bye\n");
				exit(EXIT_SUCCESS); //exit :)
			}//DISCONNECT
		}//switch
	}//while
		return 0; //will never get here :)
}//state_machine
