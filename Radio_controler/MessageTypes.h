/*
 * MessageTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: hershco
 */

#ifndef MESSAGETYPES_H_
#define MESSAGETYPES_H_

#include <ctype.h>

typedef struct RadioCommand{
	uint8_t commandType;
	uint16_t stationNumber;
} RadioCommand;


#endif /* MESSAGETYPES_H_ */
