/*******************************************************************
  Copyright (c) 2013, 2014 IBM Corp.
 
  All rights reserved. This program and the accompanying materials
  are made available under the terms of the Eclipse Public License v1.0
  and Eclipse Distribution License v1.0 which accompany this distribution. 
 
  The Eclipse Public License is available at 
     http://www.eclipse.org/legal/epl-v10.html
  and the Eclipse Distribution License is available at 
    http://www.eclipse.org/org/documents/edl-v10.php.
 
  Contributors:
     Ian Craggs - initial implementation and/or documentation
*******************************************************************/

#include "MQTTClient.h"
#include <string.h>
#include <stdlib.h>

#if !defined(_WINDOWS)
	#include <sys/time.h>
  	#include <sys/socket.h>
	#include <unistd.h>
  	#include <errno.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAXHOSTNAMELEN 256
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


char* topics[] =  {"TopicA", "TopicA/B", "Topic/C", "TopicA/C", "/TopicA"};
char* wildtopics[] = {"TopicA/+", "+/C", "#", "/#", "/+", "+/+", "TopicA/#"};
char* nosubscribe_topics[] = {"nosubscribe",};

struct Options
{
	char* connection;         /**< connection to system under test. */
	char* clientid1;
	char* clientid2;
	char* username;
	char* password;
	int verbose;
	int test_no;
	int MQTTVersion;
	int iterations;
} options =
{
	"tcp://localhost:1883",
	"myclientid",
	"myclientid2",
	NULL,
	NULL,
	0,
	0,
	MQTTVERSION_3_1_1,
	1,
};


void usage()
{
	printf("options:\n  connection, clientid1, clientid2, username, password, MQTTversion, iterations, verbose\n");
	exit(-1);
}

void getopts(int argc, char** argv)
{
	int count = 1;
	
	while (count < argc)
	{
		if (strcmp(argv[count], "--test_no") == 0)
		{
			if (++count < argc)
				options.test_no = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--connection") == 0)
		{
			if (++count < argc)
			{
				options.connection = argv[count];
				printf("\nSetting connection to %s\n", options.connection);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--clientid1") == 0)
		{
			if (++count < argc)
			{
				options.clientid1 = argv[count];
				printf("\nSetting clientid1 to %s\n", options.clientid1);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--clientid2") == 0)
		{
			if (++count < argc)
			{
				options.clientid2 = argv[count];
				printf("\nSetting clientid2 to %s\n", options.clientid2);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--username") == 0)
		{
			if (++count < argc)
			{
				options.username = argv[count];
				printf("\nSetting username to %s\n", options.username);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--password") == 0)
		{
			if (++count < argc)
			{
				options.password = argv[count];
				printf("\nSetting password to %s\n", options.password);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--MQTTversion") == 0)
		{
			if (++count < argc)
			{
				options.MQTTVersion = atoi(argv[count]);
				printf("setting MQTT version to %d\n", options.MQTTVersion);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--iterations") == 0)
		{
			if (++count < argc)
				options.iterations = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--verbose") == 0)
		{
			options.verbose = 1;
			printf("\nSetting verbose on\n");
		}
		count++;
	}
}


#if defined(WIN32) || defined(_WINDOWS)
#define msleep Sleep
#define START_TIME_TYPE DWORD
static DWORD start_time = 0;
START_TIME_TYPE start_clock(void)
{
	return GetTickCount();
}
#elif defined(AIX)
#define mqsleep sleep
#define START_TIME_TYPE struct timespec
START_TIME_TYPE start_clock(void)
{
	static struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	return start;
}
#else
#define msleep(A) usleep(A*1000)
#define START_TIME_TYPE struct timeval
/* TODO - unused - remove? static struct timeval start_time; */
START_TIME_TYPE start_clock(void)
{
	struct timeval start_time;
	gettimeofday(&start_time, NULL);
	return start_time;
}
#endif

#define LOGA_DEBUG 0
#define LOGA_INFO 1
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
void MyLog(int LOGA_level, char* format, ...)
{
	static char msg_buf[256];
	va_list args;
	struct timeb ts;

	struct tm *timeinfo;

	if (LOGA_level == LOGA_DEBUG && options.verbose == 0)
	  return;
	
	ftime(&ts);
	timeinfo = localtime(&ts.time);
	strftime(msg_buf, 80, "%Y%m%d %H%M%S", timeinfo);

	sprintf(&msg_buf[strlen(msg_buf)], ".%.3hu ", ts.millitm);

	va_start(args, format);
	vsnprintf(&msg_buf[strlen(msg_buf)], sizeof(msg_buf) - strlen(msg_buf), format, args);
	va_end(args);

	printf("%s\n", msg_buf);
	fflush(stdout);
}


int tests = 0;
int failures = 0;


void myassert(char* filename, int lineno, char* description, int value, char* format, ...)
{
	++tests;
	if (!value)
	{
		va_list args;

		++failures;
		printf("Assertion failed, file %s, line %d, description: %s\n", filename, lineno, description);

		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		//cur_output += sprintf(cur_output, "<failure type=\"%s\">file %s, line %d </failure>\n", 
        //               description, filename, lineno);
	}
    else
    	MyLog(LOGA_DEBUG, "Assertion succeeded, file %s, line %d, description: %s", filename, lineno, description);  
}


#define assert(a, b, c, d) myassert(__FILE__, __LINE__, a, b, c, d)
#define assert1(a, b, c, d, e) myassert(__FILE__, __LINE__, a, b, c, d, e)

typedef struct
{
	char* topicName;
	int topicLen;
	MQTTClient_message* m;
} messageStruct;

messageStruct messagesArrived[1000];
int messageCount = 0;

int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* m)
{
	messagesArrived[messageCount].topicName = topicName;
	messagesArrived[messageCount].topicLen = topicLen;
	messagesArrived[messageCount++].m = m;
	MyLog(LOGA_DEBUG, "Callback: %d message received on topic %s is %.*s.",
					messageCount, topicName, m->payloadlen, (char*)(m->payload));
	return 1;
}


void cleanup()
{
	// clean all client state
	char* clientids[] = {options.clientid1, options.clientid2};
	int i, rc;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient aclient;

	MyLog(LOGA_INFO, "Cleaning up");

	opts.keepAliveInterval = 20;
	opts.cleansession = 1;
	opts.username = options.username;
	opts.password = options.password;
	opts.MQTTVersion = options.MQTTVersion;
	
	for (i = 0; i < 2; ++i)
	{
		rc = MQTTClient_create(&aclient, options.connection, clientids[i], MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
		assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

		rc = MQTTClient_connect(aclient, &opts);
		assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

		rc = MQTTClient_disconnect(aclient, 100);
		assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

		MQTTClient_destroy(&aclient);
	}

	// clean retained messages 
	rc = MQTTClient_create(&aclient, options.connection, options.clientid1, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_setCallbacks(aclient, NULL, NULL, messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_subscribe(aclient, "#", 0);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	msleep(2000); // wait for all retained messages to arrive

	rc = MQTTClient_unsubscribe(aclient, "#");
	assert("Good rc from unsubscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	for (i = 0; i < messageCount; ++i)
	{
		if (messagesArrived[i].m->retained)
		{
			MyLog(LOGA_INFO, "Deleting retained message for topic %s", (char*)messagesArrived[i].topicName);
			rc = MQTTClient_publish(aclient, messagesArrived[i].topicName, 0, "", 0, 1, NULL);
			assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);
		}
	}

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	MQTTClient_destroy(&aclient);

	for (i = 0; i < messageCount; ++i)
	{
		MQTTClient_free(messagesArrived[i].topicName);
		MQTTClient_freeMessage(&messagesArrived[i].m);
	}
	messageCount = 0;

	MyLog(LOGA_INFO, "Finished cleaning up");
}

 
int basic_test()
{
	int i, rc;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient aclient;

	MyLog(LOGA_INFO, "Starting basic test");

	tests = failures = 0;

	opts.keepAliveInterval = 20;
	opts.cleansession = 1;
	opts.username = options.username;
	opts.password = options.password;
	opts.MQTTVersion = options.MQTTVersion;

	rc = MQTTClient_create(&aclient, options.connection, options.clientid1, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_setCallbacks(aclient, NULL, NULL, messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_subscribe(aclient, topics[0], 0);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[0], 5, "qos 0", 0, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[0], 5, "qos 1", 1, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[0], 5, "qos 2", 2, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	assert("3 Messages received", messageCount == 3, "messageCount was %d", messageCount);
	for (i = 0; i < messageCount; ++i)
	{
		MQTTClient_free(messagesArrived[i].topicName);
		MQTTClient_freeMessage(&messagesArrived[i].m);
	}
	messageCount = 0;

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	opts.MQTTVersion = MQTTVERSION_3_1;
	rc = MQTTClient_connect(aclient, &opts); // should fail - wrong protocol version
	assert("Bad rc from connect", rc == MQTTCLIENT_FAILURE, "rc was %d", rc);
	
	MQTTClient_destroy(&aclient);

	MyLog(LOGA_INFO, "Basic test %s", (failures == 0) ?  "succeeded" : "failed");
	return failures;
}



int offline_message_queueing_test()
{
	int i, rc;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient aclient;
	MQTTClient bclient;

	MyLog(LOGA_INFO, "Offline message queueing test");

	tests = failures = 0;

	opts.keepAliveInterval = 20;
	opts.cleansession = 0;
	opts.username = options.username;
	opts.password = options.password;
	opts.MQTTVersion = options.MQTTVersion;

	rc = MQTTClient_create(&aclient, options.connection, options.clientid1, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_setCallbacks(aclient, NULL, NULL, messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_subscribe(aclient, wildtopics[5], 2);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_create(&bclient, options.connection, options.clientid2, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	opts.cleansession = 1;
	rc = MQTTClient_connect(bclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(bclient, topics[1], 5, "qos 0", 0, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(bclient, topics[2], 5, "qos 1", 1, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);	

	rc = MQTTClient_publish(bclient, topics[3], 5, "qos 2", 2, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_disconnect(bclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	MQTTClient_destroy(&bclient);

	opts.cleansession = 0;
	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

    msleep(200);  // receive the queued messages

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	MQTTClient_destroy(&aclient);

	assert("2 or 3 messages received", messageCount == 3 || messageCount == 2, "messageCount was %d", messageCount);

	MyLog(LOGA_INFO, "This server %s queueing QoS 0 messages for offline clients", (messageCount == 3) ? "is" : "is not");

	for (i = 0; i < messageCount; ++i)
	{
		MQTTClient_free(messagesArrived[i].topicName);
		MQTTClient_freeMessage(&messagesArrived[i].m);
	}
	messageCount = 0;

	MyLog(LOGA_INFO, "Offline message queueing test %s", (failures == 0) ?  "succeeded" : "failed");
	return failures;
}


int retained_message_test()
{ 
	int i, rc;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient aclient;

	MyLog(LOGA_INFO, "Retained message test");

	tests = failures = 0;

	opts.keepAliveInterval = 20;
	opts.cleansession = 1;
	opts.username = options.username;
	opts.password = options.password;
	opts.MQTTVersion = options.MQTTVersion;

	assert("0 messages received", messageCount == 0, "messageCount was %d", messageCount);
  
    // set retained messages
	rc = MQTTClient_create(&aclient, options.connection, options.clientid1, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_setCallbacks(aclient, NULL, NULL, messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[1], 5, "qos 0", 0, 1, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[2], 5, "qos 1", 1, 1, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);	

	rc = MQTTClient_publish(aclient, topics[3], 5, "qos 2", 2, 1, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

    msleep(200);

	rc = MQTTClient_subscribe(aclient, wildtopics[5], 2);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

    msleep(200);

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	assert("3 messages received", messageCount == 3, "messageCount was %d", messageCount);

	for (i = 0; i < messageCount; ++i)
	{
		assert("messages should be retained", messagesArrived[i].m->retained, "retained was %d", 
			messagesArrived[i].m->retained);
		MQTTClient_free(messagesArrived[i].topicName);
		MQTTClient_freeMessage(&messagesArrived[i].m);
	}
	messageCount = 0;

    // clear retained messages
	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[1], 0, "", 0, 1, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_publish(aclient, topics[2], 0, "", 1, 1, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);	

	rc = MQTTClient_publish(aclient, topics[3], 0, "", 2, 1, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

    msleep(200); // wait for QoS 2 exchange to be completed
	rc = MQTTClient_subscribe(aclient, wildtopics[5], 2);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

    msleep(200);

	rc = MQTTClient_disconnect(aclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	assert("0 messages received", messageCount == 0, "messageCount was %d", messageCount);

	MQTTClient_destroy(&aclient);

	MyLog(LOGA_INFO, "Retained message test %s", (failures == 0) ?  "succeeded" : "failed");
	return failures;
}

#define SOCKET_ERROR -1

int test6_socket_error(char* aString, int sock)
{
#if defined(WIN32)
	int errno;
#endif

#if defined(WIN32)
	errno = WSAGetLastError();
#endif
	if (errno != EINTR && errno != EAGAIN && errno != EINPROGRESS && errno != EWOULDBLOCK)
	{
		if (strcmp(aString, "shutdown") != 0 || (errno != ENOTCONN && errno != ECONNRESET))
			printf("Socket error %d in %s for socket %d", errno, aString, sock);
	}
	return errno;
}

int test6_socket_close(int socket)
{
	int rc;

#if defined(WIN32)
	if (shutdown(socket, SD_BOTH) == SOCKET_ERROR)
		test6_socket_error("shutdown", socket);
	if ((rc = closesocket(socket)) == SOCKET_ERROR)
		test6_socket_error("close", socket);
#else
	if (shutdown(socket, SHUT_RDWR) == SOCKET_ERROR)
		test6_socket_error("shutdown", socket);
	if ((rc = close(socket)) == SOCKET_ERROR)
		test6_socket_error("close", socket);
#endif
	return rc;
}

typedef struct
{
	int socket;
	time_t lastContact;
#if defined(OPENSSL)
	SSL* ssl;
	SSL_CTX* ctx;
#endif
} networkHandles;


typedef struct
{
	char* clientID;					/**< the string id of the client */
	char* username;					/**< MQTT v3.1 user name */
	char* password;					/**< MQTT v3.1 password */
	unsigned int cleansession : 1;	/**< MQTT clean session flag */
	unsigned int connected : 1;		/**< whether it is currently connected */
	unsigned int good : 1; 			/**< if we have an error on the socket we turn this off */
	unsigned int ping_outstanding : 1;
	int connect_state : 4;
	networkHandles net;
/* ... */
} Clients;


typedef struct
{
	char* serverURI;
	Clients* c;
	MQTTClient_connectionLost* cl;
	MQTTClient_messageArrived* ma;
	MQTTClient_deliveryComplete* dc;
	void* context;

	int connect_sem;
	int rc; /* getsockopt return code in connect */
	int connack_sem;
	int suback_sem;
	int unsuback_sem;
	void* pack;
} MQTTClients;

 
int will_message_test()
{
	int i, rc;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient_willOptions wopts =  MQTTClient_willOptions_initializer;
	MQTTClient aclient, bclient;

	MyLog(LOGA_INFO, "Will message test");

	tests = failures = 0;

	opts.keepAliveInterval = 2;
	opts.cleansession = 1;
	opts.username = options.username;
	opts.password = options.password;
	opts.MQTTVersion = options.MQTTVersion;

	opts.will = &wopts;
	opts.will->message = "client not disconnected";
	opts.will->qos = 1;
	opts.will->retained = 0;
	opts.will->topicName = topics[2];

	rc = MQTTClient_create(&aclient, options.connection, options.clientid1, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_connect(aclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_create(&bclient, options.connection, options.clientid2, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_setCallbacks(bclient, NULL, NULL, messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	opts.keepAliveInterval = 20;
	opts.will = NULL;
	rc = MQTTClient_connect(bclient, &opts);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_subscribe(bclient, topics[2], 2);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

    msleep(100);

   	test6_socket_close(((MQTTClients*)aclient)->c->net.socket); 
    msleep(5000);

	rc = MQTTClient_disconnect(bclient, 100);
	assert("Disconnect successful", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	MQTTClient_destroy(&bclient);

	assert("will message received", messageCount == 1, "messageCount was %d", messageCount);

	rc = MQTTClient_disconnect(aclient, 100);

	MQTTClient_destroy(&aclient);

	MyLog(LOGA_INFO, "Will message test %s", (failures == 0) ?  "succeeded" : "failed");
	return failures;
}


#if 0
def overlapping_subscriptions_test():
  # overlapping subscriptions. When there is more than one matching subscription for the same client for a topic,
  # the server may send back one message with the highest QoS of any matching subscription, or one message for
  # each subscription with a matching QoS.
  succeeded = True
  try:
    callback.clear()
    callback2.clear()
    aclient.connect(host=host, port=port)
    aclient.subscribe([wildtopics[6], wildtopics[0]], [2, 1])
    aclient.publish(topics[3], b"overlapping topic filters", 2)
    time.sleep(1)
    assert len(callback.messages) in [1, 2]
    if len(callback.messages) == 1:
      print("This server is publishing one message for all matching overlapping subscriptions, not one for each.")
      assert callback.messages[0][2] == 2
    else:
      print("This server is publishing one message per each matching overlapping subscription.")
      assert (callback.messages[0][2] == 2 and callback.messages[1][2] == 1) or \
             (callback.messages[0][2] == 1 and callback.messages[1][2] == 2), callback.messages
    aclient.disconnect()
  except:
    traceback.print_exc()
    succeeded = False
  print("Overlapping subscriptions test", "succeeded" if succeeded else "failed")
  return succeeded

 
def keepalive_test():
  # keepalive processing.  We should be kicked off by the server if we don't send or receive any data, and don't send
  # any pings either.
  succeeded = True
  try:
    callback2.clear()
    aclient.connect(host=host, port=port, cleansession=True, keepalive=5, willFlag=True, 
          willTopic=topics[4], willMessage=b"keepalive expiry") 
    bclient.connect(host=host, port=port, cleansession=True, keepalive=0)
    bclient.subscribe([topics[4]], [2])
    time.sleep(15)
    bclient.disconnect()
    assert len(callback2.messages) == 1, "length should be 1: %s" % callback2.messages # should have the will message
  except:
    traceback.print_exc()
    succeeded = False
  print("Keepalive test", "succeeded" if succeeded else "failed")
  return succeeded


def redelivery_on_reconnect_test():
  # redelivery on reconnect. When a QoS 1 or 2 exchange has not been completed, the server should retry the 
  # appropriate MQTT packets
  succeeded = True
  try:
    callback.clear()
    callback2.clear()
    bclient.connect(host=host, port=port, cleansession=False)
    bclient.subscribe([wildtopics[6]], [2])
    bclient.pause() # stops background processing 
    bclient.publish(topics[1], b"", 1, retained=False)
    bclient.publish(topics[3], b"", 2, retained=False)
    time.sleep(1)
    bclient.disconnect()
    assert len(callback2.messages) == 0
    bclient.connect(host=host, port=port, cleansession=False)
    bclient.resume()
    time.sleep(3)
    assert len(callback2.messages) == 2, "length should be 2: %s" % callback2.messages
    bclient.disconnect()
  except:
    traceback.print_exc()
    succeeded = False
  print("Redelivery on reconnect test", "succeeded" if succeeded else "failed")
  return succeeded


# 0 length clientid
def zero_length_clientid_test():
  succeeded = True
  try:
    client0 = mqtt.client.Client("")
    fails = False
    try:
      client0.connect(host=host, port=port, cleansession=False) # should be rejected
    except:
      fails = True
    assert fails == True
    fails = False
    try:
      client0.connect(host=host, port=port, cleansession=True) # should work
    except:
      fails = True
    assert fails == False
    client0.disconnect() 
  except:
    traceback.print_exc()
    succeeded = False
  print("Zero length clientid test", "succeeded" if succeeded else "failed")
  return succeeded

def subscribe_failure_test():
  # Subscribe failure.  A new feature of MQTT 3.1.1 is the ability to send back negative reponses to subscribe
  # requests.  One way of doing this is to subscribe to a topic which is not allowed to be subscribed to.
  succeeded = True
  try:
    callback.clear()
    aclient.connect(host=host, port=port)
    aclient.subscribe([nosubscribe_topics[0]], [2])
    time.sleep(.2)
    # subscribeds is a list of (msgid, [qos])
    assert callback.subscribeds[0][1][0] == 0x80, "return code should be 0x80 %s" % callback.subscribeds
  except:
    traceback.print_exc()
    succeeded = False
  print("Subscribe failure test", "succeeded" if succeeded else "failed")
  return succeeded


def dollar_topics_test():
  # $ topics. The specification says that a topic filter which starts with a wildcard does not match topic names that
  # begin with a $.  Publishing to a topic which starts with a $ may not be allowed on some servers (which is entirely valid),
  # so this test will not work and should be omitted in that case.
  succeeded = True
  try:
    callback2.clear()
    bclient.connect(host=host, port=port, cleansession=True, keepalive=0)
    bclient.subscribe([wildtopics[5]], [2])
    time.sleep(1) # wait for all retained messages, hopefully
    callback2.clear() 
    bclient.publish("$"+topics[1], b"", 1, retained=False)
    time.sleep(.2)
    assert len(callback2.messages) == 0, callback2.messages
    bclient.disconnect()
  except:
    traceback.print_exc()
    succeeded = False
  print("$ topics test", "succeeded" if succeeded else "failed")
  return succeeded
#endif

int main(int argc, char** argv)
{
	int i;
	int all_failures = 0;

	getopts(argc, argv);

	for (i = 0; i < options.iterations; ++i)
	{
		cleanup();
		all_failures += basic_test();
		all_failures += offline_message_queueing_test();
		all_failures += retained_message_test();
		all_failures += will_message_test();
	}

	MyLog(LOGA_INFO, "Test suite %s", (failures == 0) ?  "succeeded" : "failed");

#if 0

  run_dollar_topics_test = run_zero_length_clientid_test = run_subscribe_failure_test = False
  iterations = 1

  host = "localhost"
  port = 1883
  for o, a in opts:
    if o in ("--help"):
      usage()
      sys.exit()
    elif o in ("-z", "--zero_length_clientid"):
      run_zero_length_clientid_test = True
    elif o in ("-d", "--dollar_topics"):
      run_dollar_topics_test = True
    elif o in ("-s", "--subscribe_failure"):
      run_subscribe_failure_test = True
    elif o in ("-n", "--nosubscribe_topic_filter"):
      nosubscribe_topic_filter = a
    elif o in ("-h", "--hostname"):
      host = a
    elif o in ("-p", "--port"):
      port = int(a)
    elif o in ("--iterations"):
      iterations = int(a)
    else:
      assert False, "unhandled option"

  tests = [basic_test, retained_message_test, offline_message_queueing_test, will_message_test,
           overlapping_subscriptions_test, keepalive_test, redelivery_on_reconnect_test]

  if run_zero_length_clientid_test:
    tests.append(zero_length_clientid_test)

  if run_subscribe_failure_test:
    tests.append(subscribe_failure_test)

  if run_dollar_topics_test:
    tests.append(dollar_topics_test)

  for i in range(iterations):
    print("test suite", "succeeded" if False not in [test() for test in tests] else "failed")
#endif
}













 







