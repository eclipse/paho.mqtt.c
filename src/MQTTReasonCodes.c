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

#include "MQTTReasonCodes.h"

#include "Heap.h"
#include "StackTrace.h"

#include <memory.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static struct {
	enum MQTTReasonCodes value;
	const char* name;
} nameToString[] =
{
  {SUCCESS, "SUCCESS"},
  {NORMAL_DISCONNECTION, "Normal disconnection"},
  {GRANTED_QOS_0, "Granted QoS 0"},
  {GRANTED_QOS_1, "Granted QoS 1"},
  {GRANTED_QOS_2, "Granted QoS 2"},
  {DISCONNECT_WITH_WILL_MESSAGE, "Disconnect with Will Messge"},
  {NO_MATCHING_SUBSCRIBERS, "No matching subscribers"},
  {NO_SUBSCRIPTION_FOUND, "No subscription found"},
  {CONTINUE_AUTHENTICATION, "Continue authentication"},
  {RE_AUTHENTICATE, "Re-authenticate"},
  {UNSPECIFIED_ERROR, "Unspecified error"},
  {MALFORMED_PACKET, "Malformed Packet"},
  {PROTOCOL_ERROR, "Protocol error"},
  {IMPLEMENTATION_SPECIFIC_ERROR, "Implementation specific error"},
  {UNSUPPORTED_PROTOCOL_VERSION, "Unsupported Protocol Version"},
  {CLIENT_IDENTIFIER_NOT_VALID, "Client Identifier not valid"},
  {BAD_USER_NAME_OR_PASSWORD, "Bad User Name or Password"},
  {NOT_AUTHORIZED, "Not authorized"},
  {SERVER_UNAVAILABLE, "Server unavailable"},
  {SERVER_BUSY, "Server busy"},
  {BANNED, "Banned"},
  {SERVER_SHUTTING_DOWN, "Server shutting down"},
  {BAD_AUTHENTICATION_METHOD, "Bad authentication method"},
  {KEEP_ALIVE_TIMEOUT, "Keep Alive timeout"},
  {SESSION_TAKEN_OVER, "Session taken over"},
  {TOPIC_FILTER_INVALID, "Topic filter invalid"},
  {TOPIC_NAME_INVALID, "Topic name invalid"},
  {PACKET_IDENTIFIER_IN_USE, "Packet Identifier in use"},
  {PACKET_IDENTIFIER_NOT_FOUND, "Packet Identifier not found"},
  {RECEIVE_MAXIMUM_EXCEEDED, "Receive Maximum exceeded"},
  {TOPIC_ALIAS_INVALID, "Topic Alias invalid"},
  {PACKET_TOO_LARGE, "Packet too large"},
  {MESSAGE_RATE_TOO_HIGH, "Message rate too high"},
  {QUOTA_EXCEEDED, "Quota exceeded"},
  {ADMINISTRATIVE_ACTION, "Administrative action"},
  {PAYLOAD_FORMAT_INVALID, "Payload format invalid"},
  {RETAIN_NOT_SUPPORTED, "Retain not supported"},
  {QOS_NOT_SUPPORTED, "QoS not supported"},
  {USE_ANOTHER_SERVER, "Use another server"},
  {SERVER_MOVED, "Server moved"},
  {SHARED_SUBSCRIPTIONS_NOT_SUPPORTED, "Shared subscriptions not supported"},
  {CONNECTION_RATE_EXCEEDED, "Connection rate exceeded"},
  {MAXIMUM_CONNECT_TIME, "Maximum connect time"},
  {SUBSCRIPTION_IDENTIFIERS_NOT_SUPPORTED, "Subscription Identifiers not supported"},
  {WILDCARD_SUBSCRIPTIONS_NOT_SUPPORTED, "Wildcard Subscriptions not supported"}
};

const char* MQTTReasonCodeString(enum MQTTReasonCodes value)
{
  int i = 0;
  const char* result = NULL;

  for (i = 0; i < ARRAY_SIZE(nameToString); ++i)
  {
    if (nameToString[i].value == value)
    {
    	  result = nameToString[i].name;
    	  break;
    }
  }
  return result;
}








