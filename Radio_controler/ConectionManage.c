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
	/* set new hello message*/
	int commandtype_length,reserved_length;
	commandtype_length = sizeof(hello.commandType);
	reserved_length =sizeof(hello.stationNumber);
	hello.commandType = 0;
	/*send the first part of the message(the Type field)*/
	if(send(sock,&(hello.commandType),commandtype_length,0)== -1){
		perror("error sending hello message");
		close(sock);
		exit(EXIT_FAILURE);
	}//if(send..
	/*send the second part of the message (the reserved field)*/
	if(send(sock,&(hello.stationNumber),reserved_length,0) == -1){
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
	/*length parameters for send*/
	int commandtype_length,reserved_length;
	commandtype_length = sizeof(ask.commandType);
	reserved_length =sizeof(ask.stationNumber);
	/*set the message parms */
	ask.commandType = 1;
	ask.stationNumber = htons(channel);
	/*send first part of the message*/
	if(send(sock,&(ask.commandType),commandtype_length,0)== -1){
		perror("error sending ask message");
		close(sock);
		exit(EXIT_FAILURE);
	}//if(send..
	/*send second part*/
	if(send(sock,&(ask.stationNumber),reserved_length,0) == -1){
		perror("error sending ask message");
		close(sock);
		exit(EXIT_FAILURE);
	}//if send..
	return 0;
}
/**
 * function to handle the user input
 * @return the user choice to do, -1 for exiting 0-.. for ask channel number.
 */
int handle_user_input(){
	int user_request = -2;
	char input[6];
	char cleaning;
	if(!fgets(input,sizeof(input),stdin)){
		perror("error receiving user input");
	}
	while((cleaning = getchar()) != '\n' && cleaning != '\0');
	if(strcmp(input,"q\n") == 0){
		user_request = -1;
	}
	else{
		user_request= atoi(input);
		if(user_request <0 || user_request >= number_of_stations){
			printf("Please select a channel between 0 to %n",&(number_of_stations));
			user_request = -2;
		}
	}
	return user_request;
}


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
	if (status==HELLO)
		if	(buf[0]==0 && numBytes==9){
			memcpy(&numStations,buf+1,2);
			numStations=ntohs(numStations);
			memcpy(&multicastGroup,buf+3,4);
			memcpy(&portNum,buf+7,2);
			portNum=ntohs(portNum);
			printf("MC group is: " ,inet_ntoa(multicastGroup));
			printf("There are %d stations",numStations);
			printf("The m.c port to listen to is %d",portNum);
		}// if buf
		else {
			perror("error incorrect replay type message");
			close(sock);
			exit(EXIT_FAILURE);
		}//if buf[0]==0
	else{
		if(buf[0]==1 && numBytes==2+buf[0]){//check if the songnameSize is correct
			memcpy(&stringToPrint,buf+1,buf[1]);
			stringToPrint[buf[1]]=0;//add sign end of string
			printf("Now Playing: %s",stringToPrint);
		}
		if (buf[1]==2){
			memcpy(&stringToPrint,buf+1,buf[1]);
			stringToPrint[buf[1]]=0;//add sign end of string
			printf("INVALID_COMMAND_REPLY: %s",stringToPrint);
			perror("error incorrect replay type message");
			close(sock);
			exit(EXIT_FAILURE);
		}
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
				FD_ZERO(&socks); //reset the file discriptor
				open_tcp_concection(IP,Port); //connect to the server
				/*set sock and stdin to the fd set*/
				FD_SET(sock, &socks);
				FD_SET(0,&socks);
				status = HELLO; //continue to hello
				break;
			}//connect
			case HELLO:
			{
				int open_radio_sc_conection(); //send and hello msg
				timeout.tv_usec = TIMEOUT; //set the time out
				if((select_answer = select(sock+1,&socks,NULL,NULL,&timeout)) < 0){ //sleep on select and in cse of failure:
					perror("select");
					close(sock);
					exit(EXIT_FAILURE);
				}//if((select_answer..
				else if (select_answer ==0){ //timeout occurred
					printf("Timeout while waiting for the server welcome message");
					printf("Exiting...");
				}//else if(select_answer..
				else{ //check which fd wake you
					if (FD_ISSET(0,&socks)){
						result = handle_user_input(); //collect user input
						if(result == -1){ //if the user ask to quit
							status = DISCONNECT;
						}//if(result..
					}//if(FD_ISSET(0..
					if(FD_ISSET(sock,&socks)){ //recive msg for sock
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
				if((select_answer = select(sock+1,&socks,NULL,NULL,NULL)) < 0){ //wait for user input or for socket event
					perror("select");
					close(sock);
					exit(EXIT_FAILURE);
				}//if((select_answer
				else{
					if (FD_ISSET(0,&socks)){ //if therews a user input
						result = handle_user_input();
						if(result == -1){ //if the user ask to quit
							status = DISCONNECT;
						}//if(result==-1
						else if (result >= 0){ //if the user ask for son info
							send_ask_song(result);
							timeout.tv_usec = TIMEOUT; //wait for answer from the server
							if((select_answer = select(sock+1,&socks,NULL,NULL,&timeout)) < 0){
								perror("select");
								close(sock);
								exit(EXIT_FAILURE);
							}//if(select_answer..
							else if (select_answer ==0){ //in case of timeout
								printf("Timeout while waiting for the server welcome message");
								printf("Exiting...");
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
								}//if(FD_SET(sock..
							}//else
						}//else if(result >=0..
					}//if(FD_ISSET(0..
					if(FD_ISSET(sock,&socks)){
						if((result = recive_msg()) < 0){
							status = DISCONNECT;
						}//if((result = ..
					}//if(FD_ISSET..
				}//else
				break;
			}//connected
			case DISCONNECT:
			{
				close(sock); //close the open socket
				exit(EXIT_SUCCESS); //exit :)
			}//DISCONNECT
		}
	}
		return 0; //will never get here :)
}
