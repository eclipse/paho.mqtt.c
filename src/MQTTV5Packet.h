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

#if !defined(MQTTV5PACKET_H)
#define MQTTV5PACKET_H

#include "MQTTV5Properties.h"

enum errors
{
	MQTTPACKET_BUFFER_TOO_SHORT = -2,
	MQTTPACKET_READ_ERROR = -1,
	MQTTPACKET_READ_COMPLETE
};

void writeInt4(char** pptr, int anInt);
int readInt4(char** pptr);
void writeMQTTLenString(char** pptr, MQTTLenString lenstring);
int MQTTLenStringRead(MQTTLenString* lenstring, char** pptr, char* enddata);
int MQTTPacket_VBIlen(int rem_len);
int MQTTPacket_decodeBuf(char* buf, int* value);

#endif /* MQTTV5PACKET_H */
