/*******************************************************************************
 * Copyright (c) 2017, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "MQTTV5Packet.h"
#include "MQTTPacket.h"

#include <string.h>

/**
 * Writes an integer as 4 bytes to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param anInt the integer to write
 */
void writeInt4(char** pptr, int anInt)
{
  **pptr = (char)(anInt / 16777216);
  (*pptr)++;
  anInt %= 16777216;
  **pptr = (char)(anInt / 65536);
  (*pptr)++;
  anInt %= 65536;
	**pptr = (char)(anInt / 256);
	(*pptr)++;
	**pptr = (char)(anInt % 256);
	(*pptr)++;
}


/**
 * Calculates an integer from two bytes read from the input buffer
 * @param pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the integer value calculated
 */
int readInt4(char** pptr)
{
	char* ptr = *pptr;
	int value = 16777216*(*ptr) + 65536*(*(ptr+1)) + 256*(*(ptr+2)) + (*(ptr+3));
	*pptr += 4;
	return value;
}


void writeMQTTLenString(char** pptr, MQTTLenString lenstring)
{
  writeInt(pptr, lenstring.len);
  memcpy(*pptr, lenstring.data, lenstring.len);
  *pptr += lenstring.len;
}


int MQTTLenStringRead(MQTTLenString* lenstring, char** pptr, char* enddata)
{
	int len = 0;

	/* the first two bytes are the length of the string */
	if (enddata - (*pptr) > 1) /* enough length to read the integer? */
	{
		lenstring->len = readInt(pptr); /* increments pptr to point past length */
		if (&(*pptr)[lenstring->len] <= enddata)
		{
			lenstring->data = (char*)*pptr;
			*pptr += lenstring->len;
			len = 2 + lenstring->len;
		}
	}
	return len;
}

/*
if (prop->value.integer4 >= 0 && prop->value.integer4 <= 127)
  len = 1;
else if (prop->value.integer4 >= 128 && prop->value.integer4 <= 16383)
  len = 2;
else if (prop->value.integer4 >= 16384 && prop->value.integer4 < 2097151)
  len = 3;
else if (prop->value.integer4 >= 2097152 && prop->value.integer4 < 268435455)
  len = 4;
*/
int MQTTPacket_VBIlen(int rem_len)
{
	int rc = 0;

	if (rem_len < 128)
		rc = 1;
	else if (rem_len < 16384)
		rc = 2;
	else if (rem_len < 2097152)
		rc = 3;
	else
		rc = 4;
  return rc;
}


/**
 * Decodes the message length according to the MQTT algorithm
 * @param getcharfn pointer to function to read the next character from the data source
 * @param value the decoded length returned
 * @return the number of bytes read from the socket
 */
int MQTTPacket_VBIdecode(int (*getcharfn)(char*, int), int* value)
{
	char c;
	int multiplier = 1;
	int len = 0;
#define MAX_NO_OF_REMAINING_LENGTH_BYTES 4

	*value = 0;
	do
	{
		int rc = MQTTPACKET_READ_ERROR;

		if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
		{
			rc = MQTTPACKET_READ_ERROR;	/* bad data */
			goto exit;
		}
		rc = (*getcharfn)(&c, 1);
		if (rc != 1)
			goto exit;
		*value += (c & 127) * multiplier;
		multiplier *= 128;
	} while ((c & 128) != 0);
exit:
	return len;
}


static char* bufptr;

int bufchar(char* c, int count)
{
	int i;

	for (i = 0; i < count; ++i)
		*c = *bufptr++;
	return count;
}


int MQTTPacket_decodeBuf(char* buf, int* value)
{
	bufptr = buf;
	return MQTTPacket_VBIdecode(bufchar, value);
}
