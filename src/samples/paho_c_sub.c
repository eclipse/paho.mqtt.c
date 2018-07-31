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
 *    Ian Craggs - fix for bug 413429 - connectionLost not called
 *    Guilherme Maciel Ferreira - add keep alive option
 *******************************************************************************/

#include "MQTTAsync.h"
#include "MQTTClientPersistence.h"
#include "pubsub_opts.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


#if defined(WIN32)
#include <windows.h>
#define sleep Sleep
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if defined(_WRS_KERNEL)
#include <OsWrapper.h>
#endif

volatile int finished = 0;
int subscribed = 0;
int disconnected = 0;


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
	finished = 1;
}


struct pubsub_opts opts =
{
	0, 0, 0, 0, "\n", 100,  	/* debug/app options */
	NULL, NULL, 1, 0, 0, /* message options */
	MQTTVERSION_DEFAULT, NULL, "paho-c-sub", 0, 0, NULL, NULL, "localhost", "1883", NULL, 10, /* MQTT options */
	NULL, NULL, 0, 0, /* will options */
	0, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
	0, {NULL, NULL}, /* MQTT V5 options */
};


int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	size_t delimlen = 0;

	if (opts.verbose)
		printf("%s\t", topicName);
	if (opts.delimiter)
		delimlen = strlen(opts.delimiter);
	if (opts.delimiter == NULL || (message->payloadlen > delimlen &&
		strncmp(opts.delimiter, &((char*)message->payload)[message->payloadlen - delimlen], delimlen) == 0))
		printf("%.*s", message->payloadlen, (char*)message->payload);
	else
		printf("%.*s%s", message->payloadlen, (char*)message->payload, opts.delimiter);
	if (message->struct_version == 1 && opts.verbose)
		logProperties(&message->properties);
	fflush(stdout);
	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);
	return 1;
}


void onDisconnect(void* context, MQTTAsync_successData* response)
{
	disconnected = 1;
}


void onSubscribe5(void* context, MQTTAsync_successData5* response)
{
	subscribed = 1;
}

void onSubscribe(void* context, MQTTAsync_successData* response)
{
	subscribed = 1;
}


void onSubscribeFailure5(void* context, MQTTAsync_failureData5* response)
{
	if (!opts.quiet)
		fprintf(stderr, "Subscribe failed, rc %d reason code %d\n", response->code, response->reasonCode);
	finished = 1;
}


void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	if (!opts.quiet)
		fprintf(stderr, "Subscribe failed, rc %d\n", response->code);
	finished = 1;
}


void onConnectFailure5(void* context, MQTTAsync_failureData5* response)
{
	if (!opts.quiet)
		fprintf(stderr, "Connect failed, rc %d reason code %d\n", response->code, response->reasonCode);
	finished = 1;
}


void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	if (!opts.quiet)
		fprintf(stderr, "Connect failed, rc %d\n", response ? response->code : -99);
	finished = 1;
}


void onConnect5(void* context, MQTTAsync_successData5* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_callOptions copts = MQTTAsync_callOptions_initializer;
	int rc;

	if (opts.verbose)
		printf("Subscribing to topic %s with client %s at QoS %d\n", opts.topic, opts.clientid, opts.qos);

	copts.onSuccess5 = onSubscribe5;
	copts.onFailure5 = onSubscribeFailure5;
	copts.context = client;
	if ((rc = MQTTAsync_subscribe(client, opts.topic, opts.qos, &copts)) != MQTTASYNC_SUCCESS)
	{
		if (!opts.quiet)
			fprintf(stderr, "Failed to start subscribe, return code %d\n", rc);
		finished = 1;
	}
}


void onConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
	int rc;

	if (opts.verbose)
		printf("Subscribing to topic %s with client %s at QoS %d\n", opts.topic, opts.clientid, opts.qos);

	ropts.onSuccess = onSubscribe;
	ropts.onFailure = onSubscribeFailure;
	ropts.context = client;
	if ((rc = MQTTAsync_subscribe(client, opts.topic, opts.qos, &ropts)) != MQTTASYNC_SUCCESS)
	{
		if (!opts.quiet)
			fprintf(stderr, "Failed to start subscribe, return code %d\n", rc);
		finished = 1;
	}
}

MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;


void trace_callback(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
	fprintf(stderr, "Trace : %d, %s\n", level, message);
}


int main(int argc, char** argv)
{
	MQTTAsync client;
	MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
	MQTTAsync_willOptions will_opts = MQTTAsync_willOptions_initializer;
	MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
	int rc = 0;
	char* url = NULL;
	const char* version = NULL;
	const char* program_name = "paho_c_sub";
	MQTTAsync_nameValue* infos = MQTTAsync_getVersionInfo();

	if (argc < 2)
		usage(&opts, (pubsub_opts_nameValue*)infos, program_name);

	if (getopts(argc, argv, &opts) != 0)
		usage(&opts, (pubsub_opts_nameValue*)infos, program_name);

	if (strchr(opts.topic, '#') || strchr(opts.topic, '+'))
		opts.verbose = 1;

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

	rc = MQTTAsync_create(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	MQTTAsync_setCallbacks(client, client, NULL, messageArrived, NULL);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);

	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.cleansession = 1;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.MQTTVersion = opts.MQTTVersion;
	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTAsync_connectOptions conn_opts5 = MQTTAsync_connectOptions_initializer5;
		conn_opts = conn_opts5;
		conn_opts.onSuccess5 = onConnect5;
		conn_opts.onFailure5 = onConnectFailure5;
	}
	else
	{
		conn_opts.onSuccess = onConnect;
		conn_opts.onFailure = onConnectFailure;
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
			ssl_opts.verify = 0;
		ssl_opts.CApath = opts.capath;
		ssl_opts.keyStore = opts.cert;
		ssl_opts.trustStore = opts.cafile;
		ssl_opts.privateKey = opts.key;
		ssl_opts.privateKeyPassword = opts.keypass;
		ssl_opts.enabledCipherSuites = opts.ciphers;
		conn_opts.ssl = &ssl_opts;
	}

	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		if (!opts.quiet)
			fprintf(stderr, "Failed to start connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	while (!subscribed)
		mysleep(100);

	if (finished)
		goto exit;

	while (!finished)
		mysleep(100);

	disc_opts.onSuccess = onDisconnect;
	if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
	{
		if (!opts.quiet)
			fprintf(stderr, "Failed to start disconnect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	while (!disconnected)
		mysleep(100);

exit:
	MQTTAsync_destroy(&client);

	return EXIT_SUCCESS;
}
