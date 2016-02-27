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

int print_logo();

int main(int argc , char* argv[]){
	if (argc != 3){
		puts("usage: radio_control <server name> <server port>");
		return 0;
	}
	print_logo();
	state_machine(argv[1],argv[2]);
	return 0;
}


int print_logo(){
	system("clear");
	puts("______          _ _                         _             _");
	puts("| ___ \\        | (_)                       | |           | |");
	puts("| |_/ /__ _  __| |_  ___     ___ ___  _ __ | |_ _ __ ___ | |");
	puts("|    // _` |/ _` | |/ _ \\   / __/ _ \\| '_ \\| __| '__/ _ \\| |");
	puts("| |\\ \\ (_| | (_| | | (_) | | (_| (_) | | | | |_| | | (_) | |");
	puts("\\_| \\_\\__,_|\\__,_|_|\\___/   \\___\\___/|_| |_|\\__|_|  \\___/|_|");
	puts("By Gal Hershcovich & David Dabah");
	puts("Visit us on github: http://github.com/hershcovich/InternetRadio");
	return 0;
}
