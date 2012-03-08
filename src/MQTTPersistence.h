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

#include "Clients.h"

/** Stem of the key for a sent PUBLISH QoS1 or QoS2 */
#define PERSISTENCE_PUBLISH_SENT "s-"
/** Stem of the key for a sent PUBREL */
#define PERSISTENCE_PUBREL "sc-"
/** Stem of the key for a received PUBLISH QoS2 */
#define PERSISTENCE_PUBLISH_RECEIVED "r-"


int MQTTPersistence_create(MQTTClient_persistence** per, int type, void* pcontext);
int MQTTPersistence_initialize(Clients* c, char* serverURI);
int MQTTPersistence_close(Clients* c);
int MQTTPersistence_clear(Clients* c);
int MQTTPersistence_restore(Clients* c);
void* MQTTPersistence_restorePacket(char* buffer, int buflen);
void MQTTPersistence_insertInOrder(List* list, void* content, int size);
int MQTTPersistence_put(int socket, char* buf0, int buf0len, int count, 
								 char** buffers, int* buflens, int htype, int msgId, int scr);
int MQTTPersistence_remove(Clients* c, char* type, int qos, int msgId);
void MQTTPersistence_wrapMsgID(Clients *c);
