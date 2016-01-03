/*
 * main.c
 *
 *  Created on: Dec 30, 2015
 *      Author: hershco
 */


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "ConectionManage.h"

int main(int argc, char *argv[]){
	if (argc != 3){ //validate that we get all the necessary arguments
		puts("Incorrect number of arguments!\nExiting...\n");
		return -1;
	}
	Play_MultiCast_Stream(argv[1],argv[2]);






	return 0;
}


