/**
 * ConectionManage.c

 *
 *  Created on: Dec 30, 2015
 *      Author: hershco
 */

#include "ConectionManage.h"

/**
 * Play Multicast Stream - the Primary function of the program - getws an ip and port and play the multicast
 * @param IP_ADDR Multicast address
 * @param IP_Port port of th sream
 * @return 0 on success 1 otherwise
 */
int Play_MultiCast_Stream(char *IP_ADDR , char* IP_Port){
	int sock,select_toread;
	struct sockaddr_in multiaddr; // struct to open the socket
	struct ip_mreq mreq; //struct for the igmp request
	fd_set socks; //file descriptor set for the select function
	FD_ZERO(&socks);
	/* Set the multiaddr struct to bind aocket for the multicast stream */
	memset((char*) &multiaddr,0,sizeof(multiaddr));
	multiaddr.sin_family = AF_INET;
	multiaddr.sin_port = htons(atoi(IP_Port));
	multiaddr.sin_addr.s_addr = INADDR_ANY;
	/*try to open the socket*/
	if((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0){ //In case of failure...
		perror("failed to open socket");
		exit(1);
	} //if(sock ..
	/*try to bind the socket*/
	if(bind(sock,(struct sockaddr *)&multiaddr,sizeof(multiaddr))){//in case of failure...
		perror("Bind problem");
		close(sock);
		exit(EXIT_FAILURE);
	}//if(bind..
	/*generate multicast request*/
	mreq.imr_multiaddr.s_addr = inet_addr(IP_ADDR);
	mreq.imr_interface.s_addr = INADDR_ANY;
	/*Send the Igmp request*/
	if(setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) <0){ //in case of failure...
		perror("failed to SetSockopt multicast");
		close(sock);
		exit(EXIT_FAILURE);
	}
	/* fill the FD set with stdin and sock*/
	FD_SET(sock,&socks);
	FD_SET(0,&socks);
	/*the main loop*/
	while(1){
		select_toread = select(sock +1, &socks,NULL,NULL,NULL); //wait for input from stdin or network
		if(select_toread < 0){ //In case of failure
			perror("there was problem in select function");
			setsockopt(sock,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq));
			close(sock);
			exit(EXIT_FAILURE);
		}//if(select_toread < ..
		else if(select_toread > 0){ //there's something to read
			if(read_socks(sock,&socks,&multiaddr) == 1){  //go to read_socks , if the function return quit_flag 1 exit safely.
				setsockopt(sock,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq));
				close(sock);
				exit(EXIT_SUCCESS);
			}//if(read_socks..
		}//else if(select_toread..
	}//while(1)
	return 0; //will never get here ;)
}
/**
 * this function read and decide what to do from select fd notifications
 * @param sock the sock file discriptor
 * @param socks fd_set of socks
 * @param addr sockaddr_in struct for the multicast
 * @return 1 for exit , 0 else
 */
int read_socks(int  sock, fd_set *socks ,struct sockaddr_in* addr){
	int quit_flag = 0 ; //set the quit flag to 0
	if(FD_ISSET(0,socks)) //check if there was astdin event
		quit_flag = handle_user_input(); //handle stdin event
	if(FD_ISSET(sock,socks)){//chef if there was a network event
		handle_multicast_recive(sock,addr); //handle network event
	}
	return quit_flag;
}
/**
 * Handle stdin's events
 * @return 1 for exit , 0 else
 */
int handle_user_input(){
	char user_input;
	user_input = getchar(); //get user input
	if (user_input == 'q'){ // if there was a q folow by enter quit
		user_input = getchar();
		if (user_input == '\n'){
			return 1;
		}//if(user_input == '\n'...
		else{
			return 0;
		}//else
	}//if(user_inpt =='q'..
	else{
		return 0;
	}//else
}
/**
 * the function hanles network events from the multicast sock
 * @param sock multicast ssock to read
 * @param addr address of the bind sock
 * @return 0
 */
int handle_multicast_recive(int sock ,struct sockaddr_in* addr)
{
	char databuf[1024]; //bufer for geting the stream
	int datalen = sizeof(databuf); //the size of the buffer
	int addrlen = sizeof(*addr);
	int countbytes;
	countbytes = recvfrom(sock,databuf,datalen,0,(struct sockaddr *)addr,(socklen_t *)&addrlen); //recive bytes from the stream to databuf buffer
	if (countbytes <= 0) return 0;
	write(STDOUT_FILENO,databuf,countbytes); //print the stream to stdout
	return 0;
}
