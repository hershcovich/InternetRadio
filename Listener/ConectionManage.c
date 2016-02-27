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
int Play_MultiCast_Stream(char *IP_ADDR , char* IP_Port)
{
	int sock;
	char databuf[2048]; //bufer for geting the stream
	int datalen = sizeof(databuf); //the size of the buffer
	int addrlen; //the length of the sockaddr_in
	int countbytes; //counter for received bytes
	struct sockaddr_in multiaddr; // struct to open the socket
	struct ip_mreq mreq; //struct for the igmp request
	fd_set socks; //file descriptor set for the select function
	addrlen = sizeof(multiaddr); //set address len to be the size of sockaddr_in struct
	FD_ZERO(&socks);
	/* Set the multiaddr struct to bind a socket for the multicast stream */
	memset((char*) &multiaddr,0,sizeof(multiaddr));
	multiaddr.sin_family = AF_INET;
	multiaddr.sin_port = htons(atoi(IP_Port));
	multiaddr.sin_addr.s_addr = INADDR_ANY;
	/*try to open the socket*/
	if((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0){ //In case of failure...
		perror("failed to open socket");
		if(play)pclose(play);
		exit(1);
	} //if(sock ..
	sock_for_sigint = sock;  // save sock as global variable for use with the Sig_handler;
	/*try to bind the socket*/
	if(bind(sock,(struct sockaddr *)&multiaddr,sizeof(multiaddr))){//in case of failure...
		perror("Bind problem");
		close(sock);
		if(play)pclose(play);
		exit(EXIT_FAILURE);
	}//if(bind..
	/*generate multicast request*/
	mreq.imr_multiaddr.s_addr = inet_addr(IP_ADDR);
	mreq.imr_interface.s_addr = INADDR_ANY;
	/*Send the Igmp request*/
	if(setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) <0){ //in case of failure...
		perror("failed to SetSockopt multicast");
		close(sock);
		if(play)pclose(play);
		exit(EXIT_FAILURE);
	}
	/*the main loop to receive data*/
	if(play){ // if the play application installed
		while(1){
			if((countbytes = recvfrom(sock,databuf,datalen,0,(struct sockaddr *)&multiaddr,(socklen_t *)&addrlen)) == -1){
								perror("Recive problem");
								close(sock);
								if(play)pclose(play);
								exit(EXIT_FAILURE);
							}
				fwrite(databuf,sizeof(databuf[0]),countbytes,play); //print the stream to play program
			}
	}
	else{ //if play application not installed or -p flag exist
		while(1){
				if((countbytes = recvfrom(sock,databuf,datalen,0,(struct sockaddr *)&multiaddr,(socklen_t *)&addrlen)) == -1){
					perror("Recive problem");
					close(sock);
					if(play)pclose(play);
					exit(EXIT_FAILURE);
				}
				write(STDOUT_FILENO,databuf,countbytes); //print the stream to stdout
			}
	}
	return 0; //will never get here ;)
}

