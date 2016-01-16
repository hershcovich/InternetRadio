/*
 * main.c
 *
 *  Created on: Jan 4, 2016
 *      Author: hershco
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ConectionManage.h"

int main(int argc , char* argv[]){
	if (argc != 3){
		puts("usage: radio_control <server name> <server port>");
		return 0;
	}
	state_machine(argv[1],argv[2]);
	return 0;
}