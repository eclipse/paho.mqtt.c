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
 *******************************************************************************/

 /*

Synchronous API version of paho_c_pub.c

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


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}


struct pubsub_opts opts =
{
	MQTTVERSION_DEFAULT, 0,
	NULL, "paho-cs-pub", "\n", 100, 0, 0, NULL, NULL, "localhost", "1883", NULL, 0, 10,
	NULL, NULL, 0, 0, /* will options */
	0, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
	0, {NULL, NULL}, /* publish properties */
};


void usage(void)
{
	printf("MQTT stdout subscriber\n");
	printf("Usage: stdoutsub topicname <options>, where options are:\n");
	printf("  --host <hostname> (default is %s)\n", opts.host);
	printf("  --port <port> (default is %s)\n", opts.port);
	printf("  --qos <qos> (default is %d)\n", opts.qos);
	printf("  --delimiter <delim> (default is no delimiter)\n");
	printf("  --clientid <clientid> (default is %s)\n", opts.clientid);
	printf("  --username none\n");
	printf("  --password none\n");
	printf("  --verbose <on or off> (default is on if the topic has a wildcard, else off)\n");
	printf("  --keepalive <seconds> (default is 10 seconds)\n");
	exit(EXIT_FAILURE);
}


int myconnect(MQTTClient* client)
{
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
	MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
	int rc = 0;

	if (opts.verbose)
		printf("Connecting\n");

	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTClient_connectOptions conn_opts5 = MQTTClient_connectOptions_initializer5;
		conn_opts = conn_opts5;
	}

	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.MQTTVersion = opts.MQTTVersion;

	if (opts.will_topic) 	/* will options */
	{
		will_opts.message = opts.will_payload;
		will_opts.topicName = opts.will_topic;
		will_opts.qos = opts.will_qos;
		will_opts.retained = opts.will_retain;
		conn_opts.will = &will_opts;
	}

	if (opts.connection && (strncmp(opts.connection, "ssl://", 6) == 0 ||
			strncmp(opts.connection, "wss://", 6) == 0))
	{
		if (opts.insecure)
			ssl_opts.enableServerCertAuth = 0;
		ssl_opts.CApath = opts.capath;
		ssl_opts.keyStore = opts.cert;
		ssl_opts.trustStore = opts.cafile;
		ssl_opts.privateKey = opts.key;
		ssl_opts.privateKeyPassword = opts.keypass;
		ssl_opts.enabledCipherSuites = opts.ciphers;
		conn_opts.ssl = &ssl_opts;
	}

	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTProperties props = MQTTProperties_initializer;
		MQTTProperties willProps = MQTTProperties_initializer;
		MQTTResponse response = MQTTResponse_initializer;

		conn_opts.cleanstart = 1;
		response = MQTTClient_connect5(client, &conn_opts, &props, &willProps);
		rc = response.reasonCode;
	}
	else
	{
		conn_opts.cleansession = 1;
		rc = MQTTClient_connect(client, &conn_opts);
	}

	if (opts.verbose && rc == MQTTCLIENT_SUCCESS)
		printf("Connected\n");

	return rc;
}


int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* m)
{
	/* not expecting any messages */
	return 1;
}


void trace_callback(enum MQTTCLIENT_TRACE_LEVELS level, char* message)
{
	printf("Trace : %d, %s\n", level, message);
}


int main(int argc, char** argv)
{
	MQTTClient client;
	MQTTProperties pub_props = MQTTProperties_initializer;
	char* buffer = NULL;
	int rc = 0;
	char* url;

	if (argc < 2)
		usage();

	if (getopts(argc, argv, &opts) != 0)
		usage();

	if (opts.connection)
		url = opts.connection;
	else
	{
		url = malloc(100);
		sprintf(url, "%s:%s", opts.host, opts.port);
	}
	if (opts.verbose)
		printf("URL is %s\n", url);

	if (opts.tracelevel > 0)
	{
		MQTTClient_setTraceCallback(trace_callback);
		MQTTClient_setTraceLevel(opts.tracelevel);
	}

	rc = MQTTClient_create(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);

	rc = MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

	myconnect(client);

	buffer = malloc(opts.maxdatalen);

	if (opts.MQTTVersion >= MQTTVERSION_5)
	{
		MQTTProperty property;

		if (opts.message_expiry > 0)
		{
			property.identifier = MESSAGE_EXPIRY_INTERVAL;
			property.value.integer4 = opts.message_expiry;
			MQTTProperties_add(&pub_props, &property);
		}
		if (opts.user_property.name)
		{
			property.identifier = USER_PROPERTY;
			property.value.data.data = opts.user_property.name;
			property.value.data.len = strlen(opts.user_property.name);
			property.value.value.data = opts.user_property.value;
			property.value.value.len = strlen(opts.user_property.value);
			MQTTProperties_add(&pub_props, &property);
		}
	}

	while (!toStop)
	{
		int data_len = 0;
		int delim_len = 0;

		delim_len = (int)strlen(opts.delimiter);
		do
		{
			buffer[data_len++] = getchar();
			if (data_len > delim_len)
			{
				if (strncmp(opts.delimiter, &buffer[data_len - delim_len], delim_len) == 0)
					break;
			}
		} while (data_len < opts.maxdatalen);

		if (opts.verbose)
			printf("Publishing data of length %d\n", data_len);
		if (opts.MQTTVersion == MQTTVERSION_5)
		{
			MQTTResponse response = MQTTResponse_initializer;

			response = MQTTClient_publish5(client, opts.topic, data_len, buffer, opts.qos, opts.retained, &pub_props, NULL);
			rc = response.reasonCode;
		}
		else
			rc = MQTTClient_publish(client, opts.topic, data_len, buffer, opts.qos, opts.retained, NULL);
		if (rc != 0)
		{
			myconnect(client);
			if (opts.MQTTVersion == MQTTVERSION_5)
			{
				MQTTResponse response = MQTTResponse_initializer;

				response = MQTTClient_publish5(client, opts.topic, data_len, buffer, opts.qos, opts.retained, &pub_props, NULL);
				rc = response.reasonCode;
			}
			else
				rc = MQTTClient_publish(client, opts.topic, data_len, buffer, opts.qos, opts.retained, NULL);
		}
		if (opts.qos > 0)
			MQTTClient_yield();
	}

	printf("Stopping\n");

	free(buffer);

	if (opts.MQTTVersion == MQTTVERSION_5)
		rc = MQTTClient_disconnect5(client, 0, SUCCESS, NULL);
	else
		rc = MQTTClient_disconnect(client, 0);

 	MQTTClient_destroy(&client);

	return EXIT_SUCCESS;
}
