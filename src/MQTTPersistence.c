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
 * \brief Functions that apply to persistence operations.
 *
 */

#include <stdio.h>
#include <string.h>

#include "MQTTPersistence.h"
#include "MQTTPersistenceDefault.h"
#include "MQTTProtocolClient.h"
#include "Heap.h"


/**
 * Creates a ::MQTTClient_persistence structure representing a persistence implementation.
 * @param persistence the ::MQTTClient_persistence structure.
 * @param type the type of the persistence implementation. See ::MQTTClient_create.
 * @param pcontext the context for this persistence implementation. See ::MQTTClient_create.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
#include "StackTrace.h"

int MQTTPersistence_create(MQTTClient_persistence** persistence, int type, void* pcontext)
{
	int rc = 0;
	MQTTClient_persistence* per = NULL;

	FUNC_ENTRY;
#if !defined(NO_PERSISTENCE)
	switch (type)
	{
		case MQTTCLIENT_PERSISTENCE_NONE :
			per = NULL;
			break;
		case MQTTCLIENT_PERSISTENCE_DEFAULT :
			per = malloc(sizeof(MQTTClient_persistence));
			if ( per != NULL )
			{
				if ( pcontext != NULL )
				{
					per->context = malloc(strlen(pcontext) + 1);
					strcpy(per->context, pcontext);
				}
				else
					per->context = ".";  /* working directory */
				/* file system functions */
				per->popen        = pstopen;
				per->pclose       = pstclose;
				per->pput         = pstput;
				per->pget         = pstget;
				per->premove      = pstremove;
				per->pkeys        = pstkeys;
				per->pclear       = pstclear;
				per->pcontainskey = pstcontainskey;
			}
			else
				rc = MQTTCLIENT_PERSISTENCE_ERROR;
			break;
		case MQTTCLIENT_PERSISTENCE_USER :
			per = (MQTTClient_persistence *)pcontext;
			if ( per == NULL || (per != NULL && (per->context == NULL || per->pclear == NULL ||
				per->pclose == NULL || per->pcontainskey == NULL || per->pget == NULL || per->pkeys == NULL ||
				per->popen == NULL || per->pput == NULL || per->premove == NULL)) )
				rc = MQTTCLIENT_PERSISTENCE_ERROR;
			break;
		default:
			rc = MQTTCLIENT_PERSISTENCE_ERROR;
			break;
	}
#endif

	*persistence = per;

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Open persistent store and restore any persisted messages.
 * @param client the client as ::Clients.
 * @param serverURI the URI of the remote end.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
int MQTTPersistence_initialize(Clients *c, char *serverURI)
{
	int rc = 0;

	FUNC_ENTRY;
	if ( c->persistence != NULL )
	{
		rc = c->persistence->popen(&(c->phandle), c->clientID, serverURI, c->persistence->context);
		if ( rc == 0 )
			rc = MQTTPersistence_restore(c);
	}

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Close persistent store.
 * @param client the client as ::Clients.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
int MQTTPersistence_close(Clients *c)
{
	int rc =0;

	FUNC_ENTRY;
	if (c->persistence != NULL)
	{
		rc = c->persistence->pclose(c->phandle);
		c->phandle = NULL;
#if !defined(NO_PERSISTENCE)
		if ( c->persistence->popen == pstopen )
			free(c->persistence);
#endif
		c->persistence = NULL;
	}

	FUNC_EXIT_RC(rc);
	return rc;
}

/**
 * Clears the persistent store.
 * @param client the client as ::Clients.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
int MQTTPersistence_clear(Clients *c)
{
	int rc = 0;

	FUNC_ENTRY;
	if (c->persistence != NULL)
		rc = c->persistence->pclear(c->phandle);

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Restores the persisted records to the outbound and inbound message queues of the
 * client.
 * @param client the client as ::Clients.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
int MQTTPersistence_restore(Clients *c)
{
	int rc = 0;
	char **msgkeys, *buffer;
	int nkeys, buflen;
	int i = 0;

	FUNC_ENTRY;
	if ( c->persistence != NULL &&
		(rc = c->persistence->pkeys(c->phandle, &msgkeys, &nkeys)) == 0 )
	{
		while( rc == 0 && i < nkeys)
		{
			if ( (rc = c->persistence->pget(c->phandle, msgkeys[i], &buffer, &buflen)) == 0 )
			{
				MQTTPacket* pack = MQTTPersistence_restorePacket(buffer, buflen);
				if ( pack != NULL )
				{
					if ( strstr(msgkeys[i],PERSISTENCE_PUBLISH_RECEIVED) != NULL )
					{
						Publish* publish = (Publish*)pack;
						Messages* msg = NULL;
						msg = MQTTProtocol_createMessage(publish, &msg, publish->header.bits.qos, publish->header.bits.retain);
						msg->nextMessageType = PUBREL;
						/* order does not matter for persisted received messages */
						ListAppend(c->inboundMsgs, msg, msg->len);
						publish->topic = NULL;
						MQTTPacket_freePublish(publish);
					}
					else if ( strstr(msgkeys[i],PERSISTENCE_PUBLISH_SENT) != NULL )
					{
						Publish* publish = (Publish*)pack;
						Messages* msg = NULL;
						char *key = malloc(MESSAGE_FILENAME_LENGTH + 1);
						sprintf(key, "%s%d", PERSISTENCE_PUBREL, publish->msgId);
						msg = MQTTProtocol_createMessage(publish, &msg, publish->header.bits.qos, publish->header.bits.retain);
						if ( c->persistence->pcontainskey(c->phandle, key) == 0 )
							/* PUBLISH Qo2 and PUBREL sent */
							msg->nextMessageType = PUBCOMP;
						/* else: PUBLISH QoS1, or PUBLISH QoS2 and PUBREL not sent */
						/* retry at the first opportunity */
						msg->lastTouch = 0;
						MQTTPersistence_insertInOrder(c->outboundMsgs, msg, msg->len);
						publish->topic = NULL;
						MQTTPacket_freePublish(publish);
						free(key);
					}
					else if ( strstr(msgkeys[i],PERSISTENCE_PUBREL) != NULL )
					{
						/* orphaned PUBRELs ? */
						Pubrel* pubrel = (Pubrel*)pack;
						char *key = malloc(MESSAGE_FILENAME_LENGTH + 1);
						sprintf(key, "%s%d", PERSISTENCE_PUBLISH_SENT, pubrel->msgId);
						if ( c->persistence->pcontainskey(c->phandle, key) != 0 )
							rc = c->persistence->premove(c->phandle, msgkeys[i]);
						free(pubrel);
						free(key);
					}
				}
				else  /* pack == NULL -> bad persisted record */
					rc = c->persistence->premove(c->phandle, msgkeys[i]);
			}
			if ( buffer != NULL )
				free(buffer);
			if ( msgkeys[i] != NULL )
				free(msgkeys[i]);
			i++;
		}
		if ( msgkeys != NULL )
			free(msgkeys);
	}

	MQTTPersistence_wrapMsgID(c);

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Returns a MQTT packet restored from persisted data.
 * @param buffer the persisted data.
 * @param buflen the number of bytes of the data buffer.
 */
void* MQTTPersistence_restorePacket(char* buffer, int buflen)
{
	void* pack = NULL;
	Header header;
	int fixed_header_length = 1, ptype, remaining_length = 0;
	char c;
	int multiplier = 1;
	extern pf new_packets[];

	FUNC_ENTRY;
	header.byte = buffer[0];
	/* decode the message length according to the MQTT algorithm */
	do
	{
		c = *(++buffer);
		remaining_length += (c & 127) * multiplier;
		multiplier *= 128;
		fixed_header_length++;
	} while ((c & 128) != 0);

	if ( (fixed_header_length + remaining_length) == buflen )
	{
		ptype = header.bits.type;
		if (ptype >= CONNECT && ptype <= DISCONNECT && new_packets[ptype] != NULL)
			pack = (*new_packets[ptype])(header.byte, ++buffer, remaining_length);
	}

	FUNC_EXIT;
	return pack;
}


/**
 * Inserts the specified message into the list, maintaining message ID order.
 * @param list the list to insert the message into.
 * @param content the message to add.
 * @param size size of the message.
 */
void MQTTPersistence_insertInOrder(List* list, void* content, int size)
{
	ListElement* index = NULL;
	ListElement* current = NULL;

	FUNC_ENTRY;
	while(ListNextElement(list, &current) != NULL && index == NULL)
	{
		if ( ((Messages*)content)->msgid < ((Messages*)current->content)->msgid )
			index = current;
	}

	ListInsert(list, content, size, index);
	FUNC_EXIT;
}


/**
 * Adds a record to the persistent store. This function must not be called for QoS0
 * messages.
 * @param socket the socket of the client.
 * @param buf0 fixed header.
 * @param buf0len length of the fixed header.
 * @param count number of buffers representing the variable header and/or the payload.
 * @param buffers the buffers representing the variable header and/or the payload.
 * @param buflens length of the buffers representing the variable header and/or the payload.
 * @param msgId the message ID.
 * @param scr 0 indicates message in the sending direction; 1 indicates message in the
 * receiving direction.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
int MQTTPersistence_put(int socket, char* buf0, int buf0len, int count,
								 char** buffers, int* buflens, int htype, int msgId, int scr )
{
	int rc = 0;
	extern ClientStates* bstate;
	int nbufs, i;
	int* lens = NULL;
	char** bufs = NULL;
	char *key;
	Clients* client = NULL;

	FUNC_ENTRY;
	client = (Clients*)(ListFindItem(bstate->clients, &socket, clientSocketCompare)->content);
	if (client->persistence != NULL)
	{
		key = malloc(MESSAGE_FILENAME_LENGTH + 1);
		nbufs = 1 + count;
		lens = (int *) malloc(nbufs * sizeof(int));
		bufs = (char **)malloc(nbufs * sizeof(char *));
		lens[0] = buf0len;
		bufs[0] = buf0;
		for (i = 0; i < count; i++)
		{
			lens[i+1] = buflens[i];
			bufs[i+1] = buffers[i];
		}

		// key
		if ( scr == 0 )
		{  /* sending */
			if (htype == PUBLISH)   /* PUBLISH QoS1 and QoS2*/
				sprintf(key, "%s%d", PERSISTENCE_PUBLISH_SENT, msgId);
			if (htype == PUBREL)  /* PUBREL */
				sprintf(key, "%s%d", PERSISTENCE_PUBREL, msgId);
		}
		if ( scr == 1 )  /* receiving PUBLISH QoS2 */
			sprintf(key, "%s%d", PERSISTENCE_PUBLISH_RECEIVED, msgId);

		rc = client->persistence->pput(client->phandle, key, nbufs, bufs, lens);

		free(key);
		free(lens);
		free(bufs);
	}

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Deletes a record from the persistent store.
 * @param client the client as ::Clients.
 * @param type the type of the persisted record: #PERSISTENCE_PUBLISH_SENT, #PERSISTENCE_PUBREL
 * or #PERSISTENCE_PUBLISH_RECEIVED.
 * @param qos the qos field of the message.
 * @param msgId the message ID.
 * @return 0 if success, #MQTTCLIENT_PERSISTENCE_ERROR otherwise.
 */
int MQTTPersistence_remove(Clients* c, char *type, int qos, int msgId)
{
	int rc = 0;

	FUNC_ENTRY;
	if (c->persistence != NULL)
	{
		char *key = malloc(MESSAGE_FILENAME_LENGTH + 1);
		if ( (strcmp(type,PERSISTENCE_PUBLISH_SENT) == 0) && qos == 2 )
		{
			sprintf(key, "%s%d", PERSISTENCE_PUBLISH_SENT, msgId) ;
			rc = c->persistence->premove(c->phandle, key);
			sprintf(key, "%s%d", PERSISTENCE_PUBREL, msgId) ;
			rc = c->persistence->premove(c->phandle, key);
		}
		else /* PERSISTENCE_PUBLISH_SENT && qos == 1 */
		{    /* or PERSISTENCE_PUBLISH_RECEIVED */
			sprintf(key, "%s%d", type, msgId) ;
			rc = c->persistence->premove(c->phandle, key);
		}
		free(key);
	}

	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Checks whether the message IDs wrapped by looking for the largest gap between two consecutive
 * message IDs in the outboundMsgs queue.
 * @param client the client as ::Clients.
 */
void MQTTPersistence_wrapMsgID(Clients *client)
{
	ListElement* wrapel = NULL;
	ListElement* current = NULL;

	FUNC_ENTRY;
	if ( client->outboundMsgs->count > 0 )
	{
		int firstMsgID = ((Messages*)client->outboundMsgs->first->content)->msgid;
		int lastMsgID = ((Messages*)client->outboundMsgs->last->content)->msgid;
		int gap = MAX_MSG_ID - lastMsgID + firstMsgID;
		current = ListNextElement(client->outboundMsgs, &current);

		while(ListNextElement(client->outboundMsgs, &current) != NULL)
		{
			int curMsgID = ((Messages*)current->content)->msgid;
			int curPrevMsgID = ((Messages*)current->prev->content)->msgid;
			int curgap = curMsgID - curPrevMsgID;
			if ( curgap > gap )
			{
				gap = curgap;
				wrapel = current;
			}
		}
	}

	if ( wrapel != NULL )
	{
		/* put wrapel at the beginning of the queue */
		client->outboundMsgs->first->prev = client->outboundMsgs->last;
		client->outboundMsgs->last->next = client->outboundMsgs->first;
		client->outboundMsgs->first = wrapel;
		client->outboundMsgs->last = wrapel->prev;
		client->outboundMsgs->first->prev = NULL;
		client->outboundMsgs->last->next = NULL;
	}
	FUNC_EXIT;
}
