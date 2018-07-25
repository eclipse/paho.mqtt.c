/*******************************************************************************
 * Copyright (c) 2012, 2013 IBM Corp.
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
 *    Ian Craggs - change delimiter option from char to string
 *    Guilherme Maciel Ferreira - add keep alive option
 *******************************************************************************/

/*

 stdout subscriber

*/
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "pubsub_opts.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


#if defined(WIN32)
#define sleep Sleep
#else
#include <sys/time.h>
#endif


volatile int toStop = 0;


struct pubsub_opts opts =
{
	0, 0, 0, "\n", 100,  	/* debug/app options */
	NULL, NULL, 1, 0, 0, /* message options */
	MQTTVERSION_DEFAULT, NULL, "paho-cs-sub", 0, 0, NULL, NULL, "localhost", "1883", NULL, 10, /* MQTT options */
	NULL, NULL, 0, 0, /* will options */
	0, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
	0, {NULL, NULL}, /* MQTT V5 options */
};


void myconnect(MQTTClient* client, MQTTClient_connectOptions* opts)
{
	int rc = 0;
	if ((rc = MQTTClient_connect(*client, opts)) != 0)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}


int main(int argc, char** argv)
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	char* topic = NULL;
	int rc = 0;
	char url[100];
	MQTTClient_nameValue* infos = MQTTClient_getVersionInfo();
	const char* version = NULL;

	while (infos->name)
	{
		if (strcmp(infos->name, "Version") == 0)
		{
			version = infos->value;
			break;
		}
		++infos;
	}

	if (argc < 2)
		usage(&opts, version);

	topic = argv[1];

	if (strchr(topic, '#') || strchr(topic, '+'))
		opts.verbose = 1;
	if (opts.verbose)
		printf("topic is %s\n", topic);

	if (getopts(argc, argv, &opts) != 0)
		usage(&opts, version);

	sprintf(url, "%s:%s", opts.host, opts.port);

	rc = MQTTClient_create(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);

	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.reliable = 0;
	conn_opts.cleansession = 1;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;

	myconnect(&client, &conn_opts);

	rc = MQTTClient_subscribe(client, topic, opts.qos);

	while (!toStop)
	{
		char* topicName = NULL;
		int topicLen;
		MQTTClient_message* message = NULL;

		rc = MQTTClient_receive(client, &topicName, &topicLen, &message, 1000);
		if (message)
		{
			if (opts.verbose)
				printf("%s\t", topicName);
			if (opts.delimiter == NULL)
				printf("%.*s", message->payloadlen, (char*)message->payload);
			else
				printf("%.*s%s", message->payloadlen, (char*)message->payload, opts.delimiter);
			fflush(stdout);
			MQTTClient_freeMessage(&message);
			MQTTClient_free(topicName);
		}
		if (rc != 0)
			myconnect(&client, &conn_opts);
	}

	printf("Stopping\n");

	MQTTClient_disconnect(client, 0);

	MQTTClient_destroy(&client);

	return EXIT_SUCCESS;
}
