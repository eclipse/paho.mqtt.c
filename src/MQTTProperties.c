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

#include "MQTTProperties.h"

#include "MQTTPacket.h"
#include "Heap.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct nameToType
{
  enum PropertyNames name;
  enum PropertyTypes type;
} namesToTypes[] =
{
  {PAYLOAD_FORMAT_INDICATOR, PROPERTY_TYPE_BYTE},
  {MESSAGE_EXPIRY_INTERVAL, FOUR_BYTE_INTEGER},
  {CONTENT_TYPE, UTF_8_ENCODED_STRING},
  {RESPONSE_TOPIC, UTF_8_ENCODED_STRING},
  {CORRELATION_DATA, BINARY_DATA},
  {SUBSCRIPTION_IDENTIFIER, VARIABLE_BYTE_INTEGER},
  {SESSION_EXPIRY_INTERVAL, FOUR_BYTE_INTEGER},
  {ASSIGNED_CLIENT_IDENTIFER, UTF_8_ENCODED_STRING},
  {SERVER_KEEP_ALIVE, TWO_BYTE_INTEGER},
  {AUTHENTICATION_METHOD, UTF_8_ENCODED_STRING},
  {AUTHENTICATION_DATA, BINARY_DATA},
  {REQUEST_PROBLEM_INFORMATION, PROPERTY_TYPE_BYTE},
  {WILL_DELAY_INTERVAL, FOUR_BYTE_INTEGER},
  {REQUEST_RESPONSE_INFORMATION, PROPERTY_TYPE_BYTE},
  {RESPONSE_INFORMATION, UTF_8_ENCODED_STRING},
  {SERVER_REFERENCE, UTF_8_ENCODED_STRING},
  {REASON_STRING, UTF_8_ENCODED_STRING},
  {RECEIVE_MAXIMUM, TWO_BYTE_INTEGER},
  {TOPIC_ALIAS_MAXIMUM, TWO_BYTE_INTEGER},
  {TOPIC_ALIAS, TWO_BYTE_INTEGER},
  {MAXIMUM_QOS, PROPERTY_TYPE_BYTE},
  {RETAIN_AVAILABLE, PROPERTY_TYPE_BYTE},
  {USER_PROPERTY, UTF_8_STRING_PAIR},
  {MAXIMUM_PACKET_SIZE, FOUR_BYTE_INTEGER},
  {WILDCARD_SUBSCRIPTION_AVAILABLE, PROPERTY_TYPE_BYTE},
  {SUBSCRIPTION_IDENTIFIER_AVAILABLE, PROPERTY_TYPE_BYTE},
  {SHARED_SUBSCRIPTION_AVAILABLE, PROPERTY_TYPE_BYTE}
};


int MQTTProperty_getType(int identifier)
{
  int i, rc = -1;

  for (i = 0; i < ARRAY_SIZE(namesToTypes); ++i)
  {
    if (namesToTypes[i].name == identifier)
    {
      rc = namesToTypes[i].type;
      break;
    }
  }
  return rc;
}


int MQTTProperties_len(MQTTProperties* props)
{
  /* properties length is an mbi */
  return props->length + MQTTPacket_VBIlen(props->length);
}


int MQTTProperties_add(MQTTProperties* props, MQTTProperty* prop)
{
  int rc = 0, type;

  if (props->count == props->max_count)
    rc = -1;  /* max number of properties already in structure */
  else if ((type = MQTTProperty_getType(prop->identifier)) < 0)
    rc = -2;
  else
  {
    int len = 0;

    props->array[props->count++] = *prop;
    /* calculate length */
    switch (type)
    {
      case PROPERTY_TYPE_BYTE:
        len = 1;
        break;
      case TWO_BYTE_INTEGER:
        len = 2;
        break;
      case FOUR_BYTE_INTEGER:
        len = 4;
        break;
      case VARIABLE_BYTE_INTEGER:
        len = MQTTPacket_VBIlen(prop->value.integer4);
        break;
      case BINARY_DATA:
      case UTF_8_ENCODED_STRING:
        len = 2 + prop->value.data.len;
        break;
      case UTF_8_STRING_PAIR:
        len = 2 + prop->value.data.len;
        len += 2 + prop->value.value.len;
        break;
    }
    props->length += len + 1; /* add identifier byte */
  }

  return rc;
}


int MQTTProperty_write(char** pptr, MQTTProperty* prop)
{
  int rc = -1,
      type = -1;

  type = MQTTProperty_getType(prop->identifier);
  if (type >= PROPERTY_TYPE_BYTE && type <= UTF_8_STRING_PAIR)
  {
    writeChar(pptr, prop->identifier);
    switch (type)
    {
      case PROPERTY_TYPE_BYTE:
        writeChar(pptr, prop->value.byte);
        rc = 1;
        break;
      case TWO_BYTE_INTEGER:
        writeInt(pptr, prop->value.integer2);
        rc = 2;
        break;
      case FOUR_BYTE_INTEGER:
        writeInt4(pptr, prop->value.integer4);
        rc = 4;
        break;
      case VARIABLE_BYTE_INTEGER:
        rc = MQTTPacket_encode(*pptr, prop->value.integer4);
        break;
      case BINARY_DATA:
      case UTF_8_ENCODED_STRING:
        writeMQTTLenString(pptr, prop->value.data);
        rc = prop->value.data.len + 2; /* include length field */
        break;
      case UTF_8_STRING_PAIR:
        writeMQTTLenString(pptr, prop->value.data);
        writeMQTTLenString(pptr, prop->value.value);
        rc = prop->value.data.len + prop->value.value.len + 4; /* include length fields */
        break;
    }
  }
  return rc + 1; /* include identifier byte */
}


/**
 * write the supplied properties into a packet buffer
 * @param pptr pointer to the buffer - move the pointer as we add data
 * @param remlength the max length of the buffer
 * @return whether the write succeeded or not, number of bytes written or < 0
 */
int MQTTProperties_write(char** pptr, MQTTProperties* properties)
{
  int rc = -1;
  int i = 0, len = 0;

  /* write the entire property list length first */
  *pptr += MQTTPacket_encode(*pptr, properties->length);
  len = rc = 1;
  for (i = 0; i < properties->count; ++i)
  {
    rc = MQTTProperty_write(pptr, &properties->array[i]);
    if (rc < 0)
      break;
    else
      len += rc;
  }
  if (rc >= 0)
    rc = len;

  return rc;
}


int MQTTProperty_read(MQTTProperty* prop, char** pptr, char* enddata)
{
  int type = -1,
    len = 0;

  prop->identifier = readChar(pptr);
  type = MQTTProperty_getType(prop->identifier);
  if (type >= PROPERTY_TYPE_BYTE && type <= UTF_8_STRING_PAIR)
  {
    switch (type)
    {
      case PROPERTY_TYPE_BYTE:
        prop->value.byte = readChar(pptr);
        len = 1;
        break;
      case TWO_BYTE_INTEGER:
        prop->value.integer2 = readInt(pptr);
        len = 2;
        break;
      case FOUR_BYTE_INTEGER:
        prop->value.integer4 = readInt4(pptr);
        len = 4;
        break;
      case VARIABLE_BYTE_INTEGER:
        len = MQTTPacket_decodeBuf(*pptr, &prop->value.integer4);
        *pptr += len;
        break;
      case BINARY_DATA:
      case UTF_8_ENCODED_STRING:
        len = MQTTLenStringRead(&prop->value.data, pptr, enddata);
        break;
      case UTF_8_STRING_PAIR:
        len = MQTTLenStringRead(&prop->value.data, pptr, enddata);
        len += MQTTLenStringRead(&prop->value.value, pptr, enddata);
        break;
    }
  }
  return len + 1; /* 1 byte for identifier */
}


int MQTTProperties_read(MQTTProperties* properties, char** pptr, char* enddata)
{
  int rc = 0;
  int remlength = 0;

  properties->count = 0;
  if (enddata - (*pptr) > 0) /* enough length to read the VBI? */
  {
    *pptr += MQTTPacket_decodeBuf(*pptr, &remlength);
    properties->length = remlength;
    while (remlength > 0)
    {
    	  if (properties->count == properties->max_count)
    	  {
    		  properties->max_count += 10;
    		  properties->array = realloc(properties->array, sizeof(MQTTProperty) * properties->max_count);
    	  }
      remlength -= MQTTProperty_read(&properties->array[properties->count], pptr, enddata);
      properties->count++;
    }
    if (remlength == 0)
      rc = 1; /* data read successfully */
  }

  return rc;
}

struct {
	enum PropertyNames value;
	const char* name;
} nameToString[] =
{
  {PAYLOAD_FORMAT_INDICATOR, "PAYLOAD_FORMAT_INDICATOR"},
  {MESSAGE_EXPIRY_INTERVAL, "MESSAGE_EXPIRY_INTERVAL"},
  {CONTENT_TYPE, "CONTENT_TYPE"},
  {RESPONSE_TOPIC, "RESPONSE_TOPIC"},
  {CORRELATION_DATA, "CORRELATION_DATA"},
  {SUBSCRIPTION_IDENTIFIER, "SUBSCRIPTION_IDENTIFIER"},
  {SESSION_EXPIRY_INTERVAL, "SESSION_EXPIRY_INTERVAL"},
  {ASSIGNED_CLIENT_IDENTIFER, "ASSIGNED_CLIENT_IDENTIFER"},
  {SERVER_KEEP_ALIVE, "SERVER_KEEP_ALIVE"},
  {AUTHENTICATION_METHOD, "AUTHENTICATION_METHOD"},
  {AUTHENTICATION_DATA, "AUTHENTICATION_DATA"},
  {REQUEST_PROBLEM_INFORMATION, "REQUEST_PROBLEM_INFORMATION"},
  {WILL_DELAY_INTERVAL, "WILL_DELAY_INTERVAL"},
  {REQUEST_RESPONSE_INFORMATION, "REQUEST_RESPONSE_INFORMATION"},
  {RESPONSE_INFORMATION, "RESPONSE_INFORMATION"},
  {SERVER_REFERENCE, "SERVER_REFERENCE"},
  {REASON_STRING, "REASON_STRING"},
  {RECEIVE_MAXIMUM, "RECEIVE_MAXIMUM"},
  {TOPIC_ALIAS_MAXIMUM, "TOPIC_ALIAS_MAXIMUM"},
  {TOPIC_ALIAS, "TOPIC_ALIAS"},
  {MAXIMUM_QOS, "MAXIMUM_QOS"},
  {RETAIN_AVAILABLE, "RETAIN_AVAILABLE"},
  {USER_PROPERTY, "USER_PROPERTY"},
  {MAXIMUM_PACKET_SIZE, "MAXIMUM_PACKET_SIZE"},
  {WILDCARD_SUBSCRIPTION_AVAILABLE, "WILDCARD_SUBSCRIPTION_AVAILABLE"},
  {SUBSCRIPTION_IDENTIFIER_AVAILABLE, "SUBSCRIPTION_IDENTIFIER_AVAILABLE"},
  {SHARED_SUBSCRIPTION_AVAILABLE, "SHARED_SUBSCRIPTION_AVAILABLE"}
};

const char* MQTTPropertyName(enum PropertyNames value)
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


DLLExport void MQTTProperties_free(MQTTProperties* props)
{
  int i = 0;

  for (i = 0; i < props->count; ++i)
  {
    int id = props->array[i].identifier;

    switch (MQTTProperty_getType(id))
    {
    case BINARY_DATA:
    	case UTF_8_ENCODED_STRING:
    	  break;
    	case UTF_8_STRING_PAIR:
	  break;
    }
  }
  free(props->array);
}
