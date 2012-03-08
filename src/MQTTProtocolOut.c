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

/**
 * @file
 * \brief Functions dealing with the MQTT protocol exchanges
 *
 * Some other related functions are in the MQTTProtocolClient module
 */

#include <stdlib.h>

#include "MQTTProtocolOut.h"
#include "StackTrace.h"
#include "Heap.h"

extern MQTTProtocol state;
extern ClientStates* bstate;


/**
 * Separates an address:port into two separate values
 * @param ip_address the input string
 * @param port the returned port integer
 * @return the address string
 */
char* MQTTProtocol_addressPort(char* ip_address, int* port)
{
	static char buf[INET6_ADDRSTRLEN+1];
	char* pos = strrchr(ip_address, ':'); /* reverse find to allow for ':' in IPv6 addresses */
	int len;

	FUNC_ENTRY;
	if (ip_address[0] == '[')
	{  /* ip v6 */
		if (pos < strrchr(ip_address, ']'))
			pos = NULL;  /* means it was an IPv6 separator, not for host:port */
	}

	if (pos)
	{
		int len = pos - ip_address;
		*port = atoi(pos+1);
		strncpy(buf, ip_address, len);
		buf[len] = '\0';
		pos = buf;
	}
	else
	{
		*port = DEFAULT_PORT;
	  pos = ip_address;
	}

	len = strlen(buf);
	if (buf[len - 1] == ']')
		buf[len - 1] = '\0';

	FUNC_EXIT;
	return pos;
}


/**
 * MQTT outgoing connect processing for a client
 * @param ip_address the TCP address:port to connect to
 * @param clientID the MQTT client id to use
 * @param cleansession MQTT cleansession flag
 * @param keepalive MQTT keepalive timeout in seconds
 * @param willMessage pointer to the will message to be used, if any
 * @param username MQTT 3.1 username, or NULL
 * @param password MQTT 3.1 password, or NULL
 * @return the new client structure
 */
int MQTTProtocol_connect(char* ip_address, Clients* aClient)
{
	int rc, port;
	char* addr;

	FUNC_ENTRY;
	aClient->good = 1;
	time(&(aClient->lastContact));

	addr = MQTTProtocol_addressPort(ip_address, &port);
	rc = Socket_new(addr, port, &(aClient->socket));
	if (rc == EINPROGRESS || rc == EWOULDBLOCK)
		aClient->connect_state = 1; /* TCP connect called */
	else if (rc == 0)
	{
		if ((rc = MQTTPacket_send_connect(aClient)) == 0)
			aClient->connect_state = 2; /* TCP connect completed, in which case send the MQTT connect packet */
		else
			aClient->connect_state = 0;
	}

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Process an incoming pingresp packet for a socket
 * @param pack pointer to the publish packet
 * @param sock the socket on which the packet was received
 * @return completion code
 */
int MQTTProtocol_handlePingresps(void* pack, int sock)
{
	Clients* client = NULL;
	int rc = TCPSOCKET_COMPLETE;

	FUNC_ENTRY;
	client = (Clients*)(ListFindItem(bstate->clients, &sock, clientSocketCompare)->content);
	Log(LOG_PROTOCOL, 21, NULL, sock, client->clientID);
	client->ping_outstanding = 0;
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * MQTT outgoing subscribe processing for a client
 * @param client the client structure
 * @param topics list of topics
 * @param qoss corresponding list of QoSs
 * @return completion code
 */
int MQTTProtocol_subscribe(Clients* client, List* topics, List* qoss)
{
	int rc = 0;

	FUNC_ENTRY;
	/* we should stack this up for retry processing too */
	rc = MQTTPacket_send_subscribe(topics, qoss, MQTTProtocol_assignMsgId(client), 0, client->socket, client->clientID);
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Process an incoming suback packet for a socket
 * @param pack pointer to the publish packet
 * @param sock the socket on which the packet was received
 * @return completion code
 */
int MQTTProtocol_handleSubacks(void* pack, int sock)
{
	Suback* suback = (Suback*)pack;
	Clients* client = NULL;
	int rc = TCPSOCKET_COMPLETE;

	FUNC_ENTRY;
	client = (Clients*)(ListFindItem(bstate->clients, &sock, clientSocketCompare)->content);
	Log(LOG_PROTOCOL, 23, NULL, sock, client->clientID, suback->msgId);
	MQTTPacket_freeSuback(suback);
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * MQTT outgoing unsubscribe processing for a client
 * @param client the client structure
 * @param topics list of topics
 * @return completion code
 */
int MQTTProtocol_unsubscribe(Clients* client, List* topics)
{
	int rc = 0;

	FUNC_ENTRY;
	/* we should stack this up for retry processing too? */
	rc = MQTTPacket_send_unsubscribe(topics, MQTTProtocol_assignMsgId(client), 0, client->socket, client->clientID);
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Process an incoming unsuback packet for a socket
 * @param pack pointer to the publish packet
 * @param sock the socket on which the packet was received
 * @return completion code
 */
int MQTTProtocol_handleUnsubacks(void* pack, int sock)
{
	Unsuback* unsuback = (Unsuback*)pack;
	Clients* client = NULL;
	int rc = TCPSOCKET_COMPLETE;

	FUNC_ENTRY;
	client = (Clients*)(ListFindItem(bstate->clients, &sock, clientSocketCompare)->content);
	Log(LOG_PROTOCOL, 24, NULL, sock, client->clientID, unsuback->msgId);
	free(unsuback);
	FUNC_EXIT_RC(rc);
	return rc;
}

