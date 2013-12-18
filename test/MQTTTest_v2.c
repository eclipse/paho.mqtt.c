/*
 * -----------------------------------------------------------------
 * IBM Websphere MQ Telemetry
 * MQTTV3ASample MQTT v3 Asynchronous Client application
 *
 * Version: @(#) MQMBID sn=p000-L130522.1 su=_M3QBMsMbEeK31Ln-reX3cg pn=com.ibm.mq.mqxr.listener/SDK/clients/c/samples/MQTTV3ASample.c
 *
 *   <copyright 
 *   notice="lm-source-program" 
 *   pids="5724-H72," 
 *   years="2010,2012" 
 *   crc="218290716" > 
 *   Licensed Materials - Property of IBM  
 *    
 *   5724-H72, 
 *    
 *   (C) Copyright IBM Corp. 2010, 2012 All Rights Reserved.  
 *    
 *   US Government Users Restricted Rights - Use, duplication or  
 *   disclosure restricted by GSA ADP Schedule Contract with  
 *   IBM Corp.  
 *   </copyright> 
 * -----------------------------------------------------------------
 */

/**
 * This sample application demonstrates basic usage
 * of the MQTT v3 Asynchronous Client api.
 *
 * It can be run in one of two modes:
 *  - as a publisher, sending a single message to a topic on the server
 *  - as a subscriber, listening for messages from the server
 *
 */


#include <memory.h>
#include <MQTTAsync.h>
#include <MQTTClientPersistence.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>

#if defined(WIN32)
#include <Windows.h>
#define sleep Sleep
#else
#include <stdlib.h>
#include <sys/time.h>
#endif

 volatile int toStop = 0;
 volatile int finished = 0;
 volatile int connected = 0;
 volatile int quietMode = 0;
 volatile int sent = 0;
 volatile int delivery = 0;
 volatile MQTTAsync_token deliveredtoken;
 static char clientId[24];
 struct Options
 {
 	char* action;
 	char* topic;
 	char* message;
 	int qos;
 	char* broker;
 	char* port;
	int message_count;
 } options =
 {
 	"publish",
 	NULL,
 	"2",
 	2,
 	"localhost",
 	"1883",
	100
 };

 void printHelp()
 {
 	printf("Syntax:\n\n");
 	printf("    MQTTV3ASample [-h] [-a publish|subscribe] [-t <topic>] [-m <message text>]\n");
 	printf("         [-s 0|1|2] [-b <hostname|IP address>] [-p <brokerport>] \n\n");
 	printf("    -h  Print this help text and quit\n");
 	printf("    -q  Quiet mode (default is false)\n");
 	printf("    -a  Perform the relevant action (default is publish)\n");
 	printf("    -t  Publish/subscribe to <topic> instead of the default\n");
 	printf("            (publish: \"MQTTV3ASample/C/v3\", subscribe: \"MQTTV3ASample/#\")\n");
 	printf("    -m  Use this message instead of the default (\"Message from MQTTv3 C asynchronous client\")\n");
 	printf("    -s  Use this QoS instead of the default (2)\n");
 	printf("    -b  Use this name/IP address instead of the default (localhost)\n");
 	printf("    -p  Use this port instead of the default (1883)\n");
 	printf("\nDelimit strings containing spaces with \"\"\n");
 	printf("\nPublishers transmit a single message then disconnect from the broker.\n");
 	printf("Subscribers remain connected to the broker and receive appropriate messages\n");
 	printf("until Control-C (^C) is received from the keyboard.\n\n");
 }


 void handleSignal(int sig)
 {
 	toStop = 1;
 }

 int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
 {
 	int i;
 	char* payloadptr;
 	if((sent++ % 1000) == 0)
 		printf("%d messages received\n", sent++);
 	//printf("Message arrived\n");
 	//printf("     topic: %s\n", topicName);
 	//printf("   message: ");

 	//payloadptr = message->payload;
 	//for(i=0; i<message->payloadlen; i++)
 	//{
 	//	putchar(*payloadptr++);
 	//}
 	//putchar('\n');
 	MQTTAsync_freeMessage(&message);
 	MQTTAsync_free(topicName);
 	return 1;
 }

 void onSubscribe(void* context, MQTTAsync_successData* response)
 {
 	printf("Subscribe succeeded\n");
 }

 void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
 {
 	printf("Subscribe failed\n");
 	finished = 1;
 }

 void onDisconnect(void* context, MQTTAsync_successData* response)
 {
 	printf("Successful disconnection\n");
 	finished = 1;
 }


 void onSendFailure(void* context, MQTTAsync_failureData* response)
 {
	printf("onSendFailure: message with token value %d delivery failed\n", response->token);
 }



 void onSend(void* context, MQTTAsync_successData* response)
 {
	static last_send = 0;

	if (response->token - last_send != 1)
		printf("Error in onSend, token value %d, last_send %d\n", response->token, last_send);
 	
	last_send++;

 	if ((response->token % 1000) == 0)
 		printf("onSend: message with token value %d delivery confirmed\n", response->token);
 }

 void deliveryComplete(void* context, MQTTAsync_token token)
 {
 	sent++;
 	if ((sent % 1000) == 0)
 		printf("deliveryComplete: message with token value %d delivery confirmed\n", token);
	if (sent != token)
		printf("Error, sent %d != token %d\n", sent, token);
	if (sent == options.message_count)
		toStop = 1;
 }

 void onConnectFailure(void* context, MQTTAsync_failureData* response)
 {
 	printf("Connect failed\n");
 	finished = 1;
 }


 void onConnect(void* context, MQTTAsync_successData* response)
 {
 	printf("Connected\n");
 	connected=1;
 }

 void connectionLost(void *context, char *cause)
 {
 	MQTTAsync client = (MQTTAsync)context;
 	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
 	int rc;

 	printf("\nConnection lost\n");
 	printf("     cause: %s\n", cause);

 	printf("Reconnecting\n");
 	conn_opts.keepAliveInterval = 20;
 	conn_opts.cleansession = 1;
 	conn_opts.onSuccess = onConnect;
 	conn_opts.onFailure = onConnectFailure;
 	conn_opts.context = client;
 	conn_opts.retryInterval = 1000;
 	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
 	{
 		printf("Failed to start connect, return code %d\n", rc);
 		finished = 1;
 	}
 }


void handleTrace(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
	printf("%s\n", message);
}


/**
 * The main entry point of the sample.
 *
 * This method handles parsing the arguments specified on the
 * command-line before performing the specified action.
 */
 int main(int argc, char** argv)
 {
 	int rc = 0;
 	int ch;
 	char url[256];

	// Default settings:
 	int i=0;

 	MQTTAsync client;
 	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
 	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
 	MQTTAsync_token token;

 	signal(SIGINT, handleSignal);
 	signal(SIGTERM, handleSignal);

 	quietMode = 0;
	// Parse the arguments -
 	for (i=1; i<argc; i++)
 	{
		// Check this is a valid argument
 		if (strlen(argv[i]) == 2 && argv[i][0] == '-')
 		{
 			char arg = argv[i][1];
			// Handle the no-value arguments
 			if (arg == 'h' || arg == '?')
 			{
 				printHelp();
 				return 255;
 			}
 			else if (arg == 'q')
 			{
 				quietMode = 1;
 				continue;
 			}

			// Validate there is a value associated with the argument
 			if (i == argc - 1 || argv[i+1][0] == '-')
 			{
 				printf("Missing value for argument: %s\n", argv[i]);
 				printHelp();
 				return 255;
 			}
 			switch(arg)
 			{
 				case 'a': options.action = argv[++i];      break;
 				case 't': options.topic = argv[++i];       break;
 				case 'm': options.message = argv[++i];     break;
 				case 's': options.qos = atoi(argv[++i]);   break;
 				case 'c': options.message_count = atoi(argv[++i]);   break;
 				case 'b': options.broker = argv[++i];      break;
 				case 'p': options.port = argv[++i];  break;
 				default:
 				printf("Unrecognised argument: %s\n", argv[i]);
 				printHelp();
 				return 255;
 			}
 		}
 		else
 		{
 			printf("Unrecognised argument: %s\n", argv[i]);
 			printHelp();
 			return 255;
 		}
 	}

	// Validate the provided arguments
 	if (strcmp(options.action, "publish") != 0 && strcmp(options.action, "subscribe") != 0)
 	{
 		printf("Invalid action: %s\n", options.action);
 		printHelp();
 		return 255;
 	}
 	if (options.qos < 0 || options.qos > 2)
 	{
 		printf("Invalid QoS: %d\n", options.qos);
 		printHelp();
 		return 255;
 	}
 	if (options.topic == NULL || ( options.topic != NULL && strlen(options.topic) == 0) )
 	{
		// Set the default topic according to the specified action
 		if (strcmp(options.action, "publish") == 0)
 			options.topic = "MQTTV3ASample/C/v3";
 		else
 			options.topic = "MQTTV3ASample/#";
 	}

	// Construct the full broker URL and clientId
 	sprintf(url, "tcp://%s:%s", options.broker, options.port);
 	sprintf(clientId, "SampleCV3A_%s", options.action);


 	MQTTAsync_create(&client, url, clientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	MQTTAsync_setTraceCallback(handleTrace);
	MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);

 	MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, deliveryComplete);

 	conn_opts.cleansession = 0;
 	conn_opts.onSuccess = onConnect;
 	conn_opts.onFailure = onConnectFailure;
 	conn_opts.context = client;
 	conn_opts.keepAliveInterval = 0;
 	conn_opts.retryInterval = 0;
 	//conn_opts.maxInflight= 30;

 	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
 	{
 		printf("Failed to start connect, return code %d\n", rc);
 		goto exit;
 	}
 	printf("Waiting for connect\n");
 	while (connected == 0 && finished == 0 && toStop == 0) {
 		printf("Waiting for connect: %d %d %d\n", connected, finished, toStop);
 		usleep(10000L);
 	}

 	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
 		printf("Waiting for connect: %d %d %d\n", connected, finished, toStop);

 	printf("Successful connection\n");

 	if (connected == 1 && strcmp(options.action, "publish") == 0)
	{	
 		unsigned long i; 
 		struct timeval tv;
 		gettimeofday(&tv,NULL);
 		printf("start seconds : %ld\n",tv.tv_sec); 
 		for (i = 0; i < options.message_count; i++)
 		{
 			opts.onSuccess = onSend;
			opts.onFailure = onSendFailure;
 			opts.context = client;
 			pubmsg.payload = options.message;
 			pubmsg.payloadlen = strlen(options.message);
 			pubmsg.qos = options.qos;
 			pubmsg.retained = 0;
 			deliveredtoken = 0;
 			usleep(100);

 			if ((rc = MQTTAsync_sendMessage(client, options.topic, &pubmsg, &opts))
 				!= MQTTASYNC_SUCCESS)
 			{
 				printf("Failed to start sendMessage, return code %d\n", rc);
 				exit(-1);
 			}
 		}

 		gettimeofday(&tv,NULL);

 		printf("end seconds : %ld\n",tv.tv_sec); 
 	} else if (strcmp(options.action, "subscribe") == 0) {
 		opts.onSuccess = onSubscribe;
 		opts.onFailure = onSubscribeFailure;
 		opts.context = client;
 		if ((rc = MQTTAsync_subscribe(client, options.topic, options.qos, &opts)) != MQTTASYNC_SUCCESS) {
 			printf("Failed to subscribe, return code %d\n", rc);
 			exit(-1);
 		}
 	}

 	while (!finished)
 	{
#if defined(WIN32)
 		Sleep(100);
#else
 		usleep(1000L);
#endif
 		if (toStop == 1)
 		{
 			MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

 			opts.onSuccess = onDisconnect;
 			opts.context = client;
 			printf("Entering disconnection phase\n");
 			if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
 			{
 				printf("Failed to start disconnect, return code %d\n", rc);
 				exit(-1);
 			}
 			toStop = 0;
 		}
 	}

 	exit:
	printf("calling destroy\n");
 	MQTTAsync_destroy(&client);
 	return rc;
 }
