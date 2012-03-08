/*******************************************************************************
 * Copyright (c) 2009, 2012 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(MQTTPACKETOUT_H)
#define MQTTPACKETOUT_H

#include "MQTTPacket.h"

int MQTTPacket_send_connect(Clients* client);
void* MQTTPacket_connack(unsigned char aHeader, char* data, int datalen);

int MQTTPacket_send_pingreq(int socket, char* clientID);

int MQTTPacket_send_subscribe(List* topics, List* qoss, int msgid, int dup, int socket, char* clientID);
void* MQTTPacket_suback(unsigned char aHeader, char* data, int datalen);

int MQTTPacket_send_unsubscribe(List* topics, int msgid, int dup, int socket, char* clientID);

#endif
