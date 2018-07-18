/*******************************************************************************
 * Copyright (c) 2012, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *    Guilherme Maciel Ferreira - add keep alive option
 *******************************************************************************/

#if !defined(PUBSUB_OPTS_H)
#define PUBSUB_OPTS_H

#include "MQTTAsync.h"
#include "MQTTClientPersistence.h"

struct pubsub_opts
{
	int MQTTVersion;
	int tracelevel;
	char* topic;
	char* clientid;
	char* delimiter;
	int maxdatalen;
	int qos;
	int retained;
	char* username;
	char* password;
	char* host;
	char* port;
	char* connection;
	int verbose;
	int keepalive;
	char* will_topic;
	char* will_payload;
	int will_qos;
	int will_retain;
	int insecure;
	char* capath;
	char* cert;
	char* cafile;
	char* key;
	char* keypass;
	char* ciphers;
	int message_expiry;
	struct {
		char *name;
		char *value;
	} user_property;
};

int getopts(int argc, char** argv, struct pubsub_opts* opts);

#endif


