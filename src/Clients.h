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

#if !defined(CLIENTS_H)
#define CLIENTS_H

#include <time.h>
#include "LinkedList.h"
#include "MQTTClientPersistence.h"
/*BE
include "LinkedList"
BE*/

/*BE
def PUBLICATIONS
{
   n32 ptr STRING open "topic"
   n32 ptr DATA "payload"
   n32 dec "payloadlen"
   n32 dec "refcount"
}
BE*/
/**
 * Stored publication data to minimize copying
 */
typedef struct
{
	char *topic;
	int topiclen;
	char* payload;
	int payloadlen;
	int refcount;
} Publications;

/*BE
// This should get moved to MQTTProtocol, but the includes don't quite work yet
map MESSAGE_TYPES
{
   "PUBREC" 5
   "PUBREL" .
   "PUBCOMP" .
}


def MESSAGES
{
   n32 dec "qos"
   n32 map bool "retain"
   n32 dec "msgid"
   n32 ptr PUBLICATIONS "publish"
   n32 time "lastTouch"
   n8 map MESSAGE_TYPES "nextMessageType"
   n32 dec "len"
}
defList(MESSAGES)
BE*/
/**
 * Client publication message data
 */
typedef struct
{
	int qos;
	int retain;
	int msgid;
	Publications *publish;
	time_t lastTouch;		/**> used for retry and expiry */
	char nextMessageType;	/**> PUBREC, PUBREL, PUBCOMP */
	int len;				/**> length of the whole structure+data */
} Messages;


/*BE
def WILLMESSAGES
{
   n32 ptr STRING open "topic"
   n32 ptr DATA open "msg"
   n32 dec "retained"
   n32 dec "qos"
}
BE*/

/**
 * Client will message data
 */
typedef struct
{
	char *topic;
	char *msg;
	int retained;
	int qos;
} willMessages;

/*BE
map CLIENT_BITS
{
	"cleansession" 1 : .
	"connected" 2 : .
	"good" 4 : .
	"ping_outstanding" 8 : .
}
def CLIENTS
{
	n32 ptr STRING open "clientID"
	n32 ptr STRING open "username"
	n32 ptr STRING open "password"
	n32 map CLIENT_BITS "bits"
	at 4 n8 bits 7:6 dec "connect_state"
	at 8
	n32 dec "socket"
	n32 dec "msgID"
	n32 dec "keepAliveInterval"
	n32 dec "maxInflightMessages"
	n32 ptr BRIDGECONNECTIONS "bridge_context"
	n32 time "lastContact"
	n32 ptr WILLMESSAGES "will"
	n32 ptr MESSAGESList open "inboundMsgs"
	n32 ptr MESSAGESList open "outboundMsgs"
	n32 ptr MESSAGESList open "messageQueue"
	n32 dec "discardedMsgs"
}

defList(CLIENTS)

BE*/

/**
 * Data related to one client
 */
typedef struct
{
	char* clientID;					/**< the string id of the client */
	char* username;					/**< MQTT v3.1 user name */
	char* password;					/**< MQTT v3.1 password */
	unsigned int cleansession : 1;	/**< MQTT clean session flag */
	unsigned int connected : 1;		/**< whether it is currently connected */
	unsigned int good : 1; 			/**< if we have an error on the socket we turn this off */
	unsigned int ping_outstanding : 1;
	unsigned int connect_state : 2;
	int socket;
	int msgID;
	int keepAliveInterval;
	int retryInterval;
	int maxInflightMessages;
	time_t lastContact;
	willMessages* will;
	List* inboundMsgs;
	List* outboundMsgs;				/**< in flight */
	List* messageQueue;
	void* phandle;  /* the persistence handle */
	MQTTClient_persistence* persistence; /* a persistence implementation */
	int connectOptionsVersion;
} Clients;

int clientIDCompare(void* a, void* b);
int clientSocketCompare(void* a, void* b);

/**
 * Configuration data related to all clients
 */
typedef struct
{
	char* version;
	List* clients;
} ClientStates;

#endif
