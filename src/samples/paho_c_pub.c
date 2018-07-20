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

#include "MQTTAsync.h"
#include "pubsub_opts.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#if defined(WIN32)
#include <windows.h>
#define sleep Sleep
#else
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#if defined(_WRS_KERNEL)
#include <OsWrapper.h>
#endif

volatile int toStop = 0;

struct pubsub_opts opts =
{
	MQTTVERSION_DEFAULT, 0,
	NULL, "paho-c-pub", "\n", 100, 0, 0, NULL, NULL, "localhost", "1883", NULL, 0, 10,
	NULL, NULL, 0, 0, /* will options */
	0, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
	0, {NULL, NULL}, /* publish properties */
};

void usage(void)
{
	printf("Eclipse Paho MQTT C publisher\n"
	"Usage: paho_c_pub <topicname> <options>, where options are:\n"
	"  -t (--topic) MQTT topic to publish to\n"
	"  -h (--host) host to connect to (default is %s)\n"
	"  -p (--port) network port to connect to (default is %s)\n"
	"  -c (--connection) connection string, overrides host/port e.g wss://hostname:port/ws\n"
	"  -q (--qos) MQTT QoS to publish on (0, 1 or 2) (default is %d)\n"
	"  -r (--retained) use MQTT retain option? (default is %s)\n"
	"  -i (--clientid) <clientid> (default is %s)\n"
	"  -u (--username) MQTT username (default is none)\n"
	"  -P (--password) MQTT password (default is none)\n"
	"  -k (--keepalive) MQTT keepalive timeout value (default is %d seconds)\n"
	"  --delimiter <delim> (default is \\n)\n"
	"  --maxdatalen <bytes> (default is %d)\n",
	opts.host, opts.port, opts.qos, opts.retained ? "on" : "off",
			opts.clientid, opts.maxdatalen, opts.keepalive);
	exit(EXIT_FAILURE);
}


void mysleep(int ms)
{
	#if defined(WIN32)
		Sleep(ms);
	#else
		usleep(ms * 1000);
	#endif
}

void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}


int messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* m)
{
	/* not expecting any messages */
	return 1;
}


static int disconnected = 0;

void onDisconnect5(void* context, MQTTAsync_successData5* response)
{
	disconnected = 1;
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
	disconnected = 1;
}


static int connected = 0;
void myconnect(MQTTAsync client);

void onConnectFailure5(void* context, MQTTAsync_failureData5* response)
{
	printf("Connect failed, rc %d reason code %d\n", response->code, response->reasonCode);
	connected = -1;

	MQTTAsync client = (MQTTAsync)context;
	myconnect(client);
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Connect failed, rc %d\n", response ? response->code : -1);
	connected = -1;

	MQTTAsync client = (MQTTAsync)context;
	myconnect(client);
}


void onConnect5(void* context, MQTTAsync_successData5* response)
{
	if (opts.verbose)
		printf("Connected\n");
	connected = 1;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
	if (opts.verbose)
		printf("Connected\n");
	connected = 1;
}


static int published = 0;

void onPublishFailure5(void* context, MQTTAsync_failureData5* response)
{
	if (opts.verbose)
		printf("Publish failed, rc %d reason code %d\n", response->code, response->reasonCode);
	published = -1;
}

void onPublishFailure(void* context, MQTTAsync_failureData* response)
{
	if (opts.verbose)
		printf("Publish failed, rc %d\n", response->code);
	published = -1;
}


void onPublish5(void* context, MQTTAsync_successData5* response)
{
	if (opts.verbose)
		printf("Publish succeeded, reason code %d\n", response->reasonCode);
	published = 1;
}


void onPublish(void* context, MQTTAsync_successData* response)
{
	if (opts.verbose)
		printf("Publish succeeded\n");
	published = 1;
}


void myconnect(MQTTAsync client)
{
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
	MQTTAsync_willOptions will_opts = MQTTAsync_willOptions_initializer;
	int rc = 0;

	if (opts.verbose)
		printf("Connecting\n");
	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.MQTTVersion = opts.MQTTVersion;
	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTAsync_connectOptions conn_opts5 = MQTTAsync_connectOptions_initializer5;
		conn_opts = conn_opts5;
		conn_opts.onSuccess5 = onConnect5;
		conn_opts.onFailure5 = onConnectFailure5;
		conn_opts.cleanstart = 1;
	}
	else
	{
		conn_opts.onSuccess = onConnect;
		conn_opts.onFailure = onConnectFailure;
		conn_opts.cleansession = 1;
	}
	conn_opts.context = client;
	conn_opts.automaticReconnect = 1;

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

	connected = 0;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}


void trace_callback(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
	printf("Trace : %d, %s\n", level, message);
}


int main(int argc, char** argv)
{
	MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
	MQTTAsync_responseOptions pub_opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
	MQTTAsync client;
	char* buffer = NULL;
	char* url = NULL;
	int rc = 0;

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
		MQTTAsync_setTraceCallback(trace_callback);
		MQTTAsync_setTraceLevel(opts.tracelevel);
	}

	create_opts.sendWhileDisconnected = 1;
	rc = MQTTAsync_createWithOptions(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);


	rc = MQTTAsync_setCallbacks(client, client, NULL, messageArrived, NULL);

	myconnect(client);

	buffer = malloc(opts.maxdatalen);

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
		if (opts.MQTTVersion >= MQTTVERSION_5)
		{
			MQTTProperty property;
			MQTTProperties props = MQTTProperties_initializer;

			pub_opts.onSuccess5 = onPublish5;
			pub_opts.onFailure5 = onPublishFailure5;

			if (opts.message_expiry > 0)
			{
				property.identifier = MESSAGE_EXPIRY_INTERVAL;
				property.value.integer4 = opts.message_expiry;
				MQTTProperties_add(&props, &property);
			}
			if (opts.user_property.name)
			{
				property.identifier = USER_PROPERTY;
				property.value.data.data = opts.user_property.name;
				property.value.data.len = strlen(opts.user_property.name);
				property.value.value.data = opts.user_property.value;
				property.value.value.len = strlen(opts.user_property.value);
				MQTTProperties_add(&props, &property);
			}
			pub_opts.properties = props;
		}
		else
		{
			pub_opts.onSuccess = onPublish;
			pub_opts.onFailure = onPublishFailure;
		}
		rc = MQTTAsync_send(client, opts.topic, data_len, buffer, opts.qos, opts.retained, &pub_opts);
		if (opts.verbose && rc != MQTTASYNC_SUCCESS)
			printf("Error from MQTTAsync_send %d\n", rc);
	}

	printf("Stopping\n");

	free(buffer);

	if (opts.MQTTVersion >= MQTTVERSION_5)
		disc_opts.onSuccess5 = onDisconnect5;
	else
		disc_opts.onSuccess = onDisconnect;
	if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start disconnect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	while (!disconnected)
		mysleep(100);

	MQTTAsync_destroy(&client);

	return EXIT_SUCCESS;
}


