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

#if !defined(MQTTPACKET_H)
#define MQTTPACKET_H

#include "Socket.h"
#include "LinkedList.h"
#include "Clients.h"

/*BE
include "Socket"
include "LinkedList"
include "Clients"
BE*/

typedef unsigned int bool;
typedef void* (*pf)(unsigned char, char*, int);

#define BAD_MQTT_PACKET -4

enum msgTypes
{
	CONNECT = 1, CONNACK, PUBLISH, PUBACK, PUBREC, PUBREL,
	PUBCOMP, SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK,
	PINGREQ, PINGRESP, DISCONNECT
};


/**
 * Bitfields for the MQTT header byte.
 */
typedef union
{
	/*unsigned*/ char byte;	/**< the whole byte */
#if defined(REVERSED)
	struct
	{
		unsigned int type : 4;	/**< message type nibble */
		bool dup : 1;			/**< DUP flag bit */
		unsigned int qos : 2;	/**< QoS value, 0, 1 or 2 */
		bool retain : 1;		/**< retained flag bit */
	} bits;
#else
	struct
	{
		bool retain : 1;		/**< retained flag bit */
		unsigned int qos : 2;	/**< QoS value, 0, 1 or 2 */
		bool dup : 1;			/**< DUP flag bit */
		unsigned int type : 4;	/**< message type nibble */
	} bits;
#endif
} Header;


/**
 * Data for a connect packet.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
	union
	{
		unsigned char all;	/**< all connect flags */
#if defined(REVERSED)
		struct
		{
			bool username : 1;			/**< 3.1 user name */
			bool password : 1; 			/**< 3.1 password */
			bool willRetain : 1;		/**< will retain setting */
			unsigned int willQoS : 2;	/**< will QoS value */
			bool will : 1;			/**< will flag */
			bool cleanstart : 1;	/**< cleansession flag */
			int : 1;	/**< unused */
		} bits;
#else
		struct
		{
			int : 1;	/**< unused */
			bool cleanstart : 1;	/**< cleansession flag */
			bool will : 1;			/**< will flag */
			unsigned int willQoS : 2;	/**< will QoS value */
			bool willRetain : 1;		/**< will retain setting */
			bool password : 1; 			/**< 3.1 password */
			bool username : 1;			/**< 3.1 user name */
		} bits;
#endif
	} flags;	/**< connect flags byte */

	char *Protocol, /**< MQTT protocol name */
		*clientID,	/**< string client id */
        *willTopic,	/**< will topic */
        *willMsg;	/**< will payload */

	int keepAliveTimer;		/**< keepalive timeout value in seconds */
	unsigned char version;	/**< MQTT version number */
} Connect;


/**
 * Data for a connack packet.
 */
typedef struct
{
	Header header; /**< MQTT header byte */
	char rc; /**< connack return code */
} Connack;


/**
 * Data for a packet with header only.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
} MQTTPacket;


/**
 * Data for a subscribe packet.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
	int msgId;		/**< MQTT message id */
	List* topics;	/**< list of topic strings */
	List* qoss;		/**< list of corresponding QoSs */
	int noTopics;	/**< topic and qos count */
} Subscribe;


/**
 * Data for a suback packet.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
	int msgId;		/**< MQTT message id */
	List* qoss;		/**< list of granted QoSs */
} Suback;


/**
 * Data for an unsubscribe packet.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
	int msgId;		/**< MQTT message id */
	List* topics;	/**< list of topic strings */
	int noTopics;	/**< topic count */
} Unsubscribe;


/**
 * Data for a publish packet.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
	char* topic;	/**< topic string */
	int topiclen;
	int msgId;		/**< MQTT message id */
	char* payload;	/**< binary payload, length delimited */
	int payloadlen;	/**< payload length */
} Publish;


/**
 * Data for one of the ack packets.
 */
typedef struct
{
	Header header;	/**< MQTT header byte */
	int msgId;		/**< MQTT message id */
} Ack;

typedef Ack Puback;
typedef Ack Pubrec;
typedef Ack Pubrel;
typedef Ack Pubcomp;
typedef Ack Unsuback;

int MQTTPacket_encode(char* buf, int length);
int MQTTPacket_decode(int socket, int* value);
int readInt(char** pptr);
char* readUTF(char** pptr, char* enddata);
char readChar(char** pptr);
void writeChar(char** pptr, char c);
void writeInt(char** pptr, int anInt);
void writeUTF(char** pptr, char* string);

char* MQTTPacket_name(int ptype);

void* MQTTPacket_Factory(int socket, int* error);
int MQTTPacket_send(int socket, Header header, char* buffer, int buflen);
int MQTTPacket_sends(int socket, Header header, int count, char** buffers, int* buflens);

void* MQTTPacket_header_only(unsigned char aHeader, char* data, int datalen);
int MQTTPacket_send_disconnect(int socket, char* clientID);

void* MQTTPacket_publish(unsigned char aHeader, char* data, int datalen);
void MQTTPacket_freePublish(Publish* pack);
int MQTTPacket_send_publish(Publish* pack, int dup, int qos, int retained, int socket, char* clientID);
int MQTTPacket_send_puback(int msgid, int socket, char* clientID);
void* MQTTPacket_ack(unsigned char aHeader, char* data, int datalen);

void MQTTPacket_freeSuback(Suback* pack);
int MQTTPacket_send_pubrec(int msgid, int socket, char* clientID);
int MQTTPacket_send_pubrel(int msgid, int dup, int socket, char* clientID);
int MQTTPacket_send_pubcomp(int msgid, int socket, char* clientID);

void MQTTPacket_free_packet(MQTTPacket* pack);

#if !defined(NO_BRIDGE)
	#include "MQTTPacketOut.h"
#endif

#endif /* MQTTPACKET_H */
