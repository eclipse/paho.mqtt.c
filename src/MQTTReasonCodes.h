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

#if !defined(MQTTREASONCODES_H)
#define MQTTREASONCODES_H

enum MQTTReasonCodes {
  SUCCESS = 0,
  NORMAL_DISCONNECTION = 0,
  GRANTED_QOS_0 = 0,
  GRANTED_QOS_1 = 1,
  GRANTED_QOS_2 = 2,
  DISCONNECT_WITH_WILL_MESSAGE = 4,
  NO_MATCHING_SUBSCRIBERS = 16,
  NO_SUBSCRIPTION_FOUND = 17,
  CONTINUE_AUTHENTICATION = 24,
  RE_AUTHENTICATE = 25,
  UNSPECIFIED_ERROR = 128,
  MALFORMED_PACKET = 129,
  PROTOCOL_ERROR = 130,
  IMPLEMENTATION_SPECIFIC_ERROR = 131,
  UNSUPPORTED_PROTOCOL_VERSION = 132,
  CLIENT_IDENTIFIER_NOT_VALID = 133,
  BAD_USER_NAME_OR_PASSWORD = 134,
  NOT_AUTHORIZED = 135,
  SERVER_UNAVAILABLE = 136,
  SERVER_BUSY = 137,
  BANNED = 138,
  SERVER_SHUTTING_DOWN = 139,
  BAD_AUTHENTICATION_METHOD = 140,
  KEEP_ALIVE_TIMEOUT = 141,
  SESSION_TAKEN_OVER = 142,
  TOPIC_FILTER_INVALID = 143,
  TOPIC_NAME_INVALID = 144,
  PACKET_IDENTIFIER_IN_USE = 145,
  PACKET_IDENTIFIER_NOT_FOUND = 146,
  RECEIVE_MAXIMUM_EXCEEDED = 147,
  TOPIC_ALIAS_INVALID = 148,
  PACKET_TOO_LARGE = 149,
  MESSAGE_RATE_TOO_HIGH = 150,
  QUOTA_EXCEEDED = 151,
  ADMINISTRATIVE_ACTION = 152,
  PAYLOAD_FORMAT_INVALID = 153,
  RETAIN_NOT_SUPPORTED = 154,
  QOS_NOT_SUPPORTED = 155,
  USE_ANOTHER_SERVER = 156,
  SERVER_MOVED = 157,
  SHARED_SUBSCRIPTIONS_NOT_SUPPORTED = 158,
  CONNECTION_RATE_EXCEEDED = 159,
  MAXIMUM_CONNECT_TIME = 160,
  SUBSCRIPTION_IDENTIFIERS_NOT_SUPPORTED = 161,
  WILDCARD_SUBSCRIPTIONS_NOT_SUPPORTED = 162
};

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

DLLExport const char* MQTTReasonCodeString(enum MQTTReasonCodes);

#endif
