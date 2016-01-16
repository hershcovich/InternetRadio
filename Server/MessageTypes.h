/*
 * MessageTypes.h
 *
 *  Created on: Jan 14, 2016
 *      Author: hershco
 */

#ifndef MESSAGETYPES_H_
#define MESSAGETYPES_H_
#include <inttypes.h>

typedef struct Welcome_msg{
	uint8_t replayType =0;
	uint16_t numStations;
	uint32_t multicastGroup;
	uint16_t portNumber;
} Welcome_msg;

typedef struct Announce_msg{
	uint8_t replyType =1;
	uint8_t songnameSize;
	char* songname;
} Announce_msg;

typedef struct InvalidCommand_msg{
	uint8_t replyType =2;
	uint8_t replatStringSize;
	char* replyString;
};

#endif /* MESSAGETYPES_H_ */
