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

#if !defined(MQTTPROTOCOLOUT_H)
#define MQTTPROTOCOLOUT_H

#include "LinkedList.h"
#include "MQTTPacket.h"
#include "Clients.h"
#include "Log.h"
#include "Messages.h"
#include "MQTTProtocol.h"
#include "MQTTProtocolClient.h"

#define DEFAULT_PORT 1883

void MQTTProtocol_reconnect(char* ip_address, Clients* client);
int MQTTProtocol_connect(char* ip_address, Clients* acClients);
int MQTTProtocol_handlePingresps(void* pack, int sock);
int MQTTProtocol_subscribe(Clients* client, List* topics, List* qoss);
int MQTTProtocol_handleSubacks(void* pack, int sock);
int MQTTProtocol_unsubscribe(Clients* client, List* topics);
int MQTTProtocol_handleUnsubacks(void* pack, int sock);

#endif
