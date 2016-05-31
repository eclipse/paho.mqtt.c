/*******************************************************************************
 * Copyright (c) 2012, 2016 IBM Corp.
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
 
 /*
 stdin publisher
 
 compulsory parameters:
 
  --topic topic to publish on
 
 defaulted parameters:
 
	--host localhost
	--port 1883
	--qos 0
	--delimiters \n
	--clientid stdin-publisher-async
	--maxdatalen 100
	--keepalive 10
	
	--userid none
	--password none
 
*/

#include "MQTTAsync.h"

#include <stdio.h>
#include <signal.h>
#include <memory.h>


#if defined(WIN32)
#include <Windows.h>
#define sleep Sleep
#else
#include <sys/time.h>
#include <stdlib.h>
#endif


volatile int toStop = 0;


struct
{
	char* clientid;
	char* delimiter;
	int maxdatalen;
	int qos;
	int retained;
	char* username;
	char* password;
	char* host;
	char* port;
	int verbose;
	int keepalive;
} opts =
{
	"stdin-publisher-async", "\n", 100, 0, 0, NULL, NULL, "localhost", "1883", 0, 10
};


void usage(void)
{
	printf("MQTT stdin publisher\n");
	printf("Usage: stdinpub topicname <options>, where options are:\n");
	printf("  --host <hostname> (default is %s)\n", opts.host);
	printf("  --port <port> (default is %s)\n", opts.port);
	printf("  --qos <qos> (default is %d)\n", opts.qos);
	printf("  --retained (default is %s)\n", opts.retained ? "on" : "off");
	printf("  --delimiter <delim> (default is \\n)\n");
	printf("  --clientid <clientid> (default is %s)\n", opts.clientid);
	printf("  --maxdatalen <bytes> (default is %d)\n", opts.maxdatalen);
	printf("  --username none\n");
	printf("  --password none\n");
	printf("  --keepalive <seconds> (default is 10 seconds)\n");
	exit(EXIT_FAILURE);
}



void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}

void getopts(int argc, char** argv);

int messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* m)
{
	/* not expecting any messages */
	return 1;
}


static int disconnected = 0;

void onDisconnect(void* context, MQTTAsync_successData* response)
{
	disconnected = 1;
}


static int connected = 0;
void myconnect(MQTTAsync* client);

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Connect failed, rc %d\n", response ? response->code : -1);
	connected = -1; 

	MQTTAsync client = (MQTTAsync)context;
	myconnect(client);
}


void onConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	int rc;
	
	printf("Connected\n");
	connected = 1;
}

void myconnect(MQTTAsync* client)
{
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
	int rc = 0;

	printf("Connecting\n");
	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.cleansession = 1;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	ssl_opts.enableServerCertAuth = 0;
	conn_opts.ssl = &ssl_opts;
	conn_opts.automaticReconnect = 1;
	connected = 0;
	if ((rc = MQTTAsync_connect(*client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}


static int published = 0;

void onPublishFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Publish failed, rc %d\n", response ? -1 : response->code);
	published = -1; 
}


void onPublish(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;

	published = 1;
}


void connectionLost(void* context, char* cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
	int rc = 0;

	printf("Connecting\n");
	conn_opts.keepAliveInterval = 10;
	conn_opts.cleansession = 1;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	ssl_opts.enableServerCertAuth = 0;
	conn_opts.ssl = &ssl_opts;
	connected = 0;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char** argv)
{
	MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
	MQTTAsync_responseOptions pub_opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
	MQTTAsync client;
	char* topic = NULL;
	char* buffer = NULL;
	int rc = 0;
	char url[100];

	if (argc < 2)
		usage();
	
	getopts(argc, argv);
	
	sprintf(url, "%s:%s", opts.host, opts.port);
	if (opts.verbose)
		printf("URL is %s\n", url);
	
	topic = argv[1];
	printf("Using topic %s\n", topic);

	create_opts.sendWhileDisconnected = 1;
	rc = MQTTAsync_createWithOptions(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);

	rc = MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, NULL);

	myconnect(&client);

	buffer = malloc(opts.maxdatalen);
	
	while (!toStop)
	{
		int data_len = 0;
		int delim_len = 0;
		
		delim_len = strlen(opts.delimiter);
		do
		{
			buffer[data_len++] = getchar();
			if (data_len > delim_len)
			{
			//printf("comparing %s %s\n", opts.delimiter, &buffer[data_len - delim_len]);
			if (strncmp(opts.delimiter, &buffer[data_len - delim_len], delim_len) == 0)
				break;
			}
		} while (data_len < opts.maxdatalen);
				
		if (opts.verbose)
				printf("Publishing data of length %d\n", data_len);
		pub_opts.onSuccess = onPublish;
		pub_opts.onFailure = onPublishFailure;
		do
		{
			rc = MQTTAsync_send(client, topic, data_len, buffer, opts.qos, opts.retained, &pub_opts);
		}
		while (rc != MQTTASYNC_SUCCESS);
	}
	
	printf("Stopping\n");
	
	free(buffer);

	disc_opts.onSuccess = onDisconnect;
	if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start disconnect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	while	(!disconnected)
		#if defined(WIN32)
			Sleep(100);
		#else
			usleep(10000L);
		#endif

	MQTTAsync_destroy(&client);

	return EXIT_SUCCESS;
}

void getopts(int argc, char** argv)
{
	int count = 2;
	
	while (count < argc)
	{
		if (strcmp(argv[count], "--retained") == 0)
			opts.retained = 1;
		if (strcmp(argv[count], "--verbose") == 0)
			opts.verbose = 1;
		else if (strcmp(argv[count], "--qos") == 0)
		{
			if (++count < argc)
			{
				if (strcmp(argv[count], "0") == 0)
					opts.qos = 0;
				else if (strcmp(argv[count], "1") == 0)
					opts.qos = 1;
				else if (strcmp(argv[count], "2") == 0)
					opts.qos = 2;
				else
					usage();
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--host") == 0)
		{
			if (++count < argc)
				opts.host = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--port") == 0)
		{
			if (++count < argc)
				opts.port = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--clientid") == 0)
		{
			if (++count < argc)
				opts.clientid = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--username") == 0)
		{
			if (++count < argc)
				opts.username = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--password") == 0)
		{
			if (++count < argc)
				opts.password = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--maxdatalen") == 0)
		{
			if (++count < argc)
				opts.maxdatalen = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--delimiter") == 0)
		{
			if (++count < argc)
				opts.delimiter = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--keepalive") == 0)
		{
			if (++count < argc)
				opts.keepalive = atoi(argv[count]);
			else
				usage();
		}
		count++;
	}
	
}

