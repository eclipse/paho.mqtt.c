/*******************************************************************************
 * Copyright (c) 2012, 2016 IBM Corp.
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


/**
 * @file
 * Offline buffering and automatic reconnect tests for the MQ Telemetry Asynchronous MQTT C client
 *
 */


#include "MQTTAsync.h"
#include <string.h>
#include <stdlib.h>
#include "Thread.h"

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

char unique[50]; // unique suffix/prefix to add to clientid/topic etc

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void usage()
{
	printf("help!!\n");
	exit(-1);
}

struct Options
{
	char* connection;            /**< connection to system under test. */
	char* proxy_connection;      /**< connection to proxy */
	int verbose;
	int test_no;
} options =
{
	"iot.eclipse.org:1883",
	"localhost:1883",
	0,
	0,
};

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
				options.connection = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--verbose") == 0)
			options.verbose = 1;
		count++;
	}
}


#if 0
#include <logaX.h>   /* For general log messages                      */
#define MyLog logaLine
#else
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
	vsnprintf(&msg_buf[strlen(msg_buf)], sizeof(msg_buf) - strlen(msg_buf),
			format, args);
	va_end(args);

	printf("%s\n", msg_buf);
	fflush(stdout);
}
#endif

void MySleep(long milliseconds)
{
#if defined(WIN32) || defined(WIN64)
	Sleep(milliseconds);
#else
	usleep(milliseconds*1000);
#endif
}

#if defined(WIN32) || defined(_WINDOWS)
#define START_TIME_TYPE DWORD
static DWORD start_time = 0;
START_TIME_TYPE start_clock(void)
{
	return GetTickCount();
}
#elif defined(AIX)
#define START_TIME_TYPE struct timespec
START_TIME_TYPE start_clock(void)
{
	static struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	return start;
}
#else
#define START_TIME_TYPE struct timeval
/* TODO - unused - remove? static struct timeval start_time; */
START_TIME_TYPE start_clock(void)
{
	struct timeval start_time;
	gettimeofday(&start_time, NULL);
	return start_time;
}
#endif

#if defined(WIN32)
long elapsed(START_TIME_TYPE start_time)
{
	return GetTickCount() - start_time;
}
#elif defined(AIX)
#define assert(a)
long elapsed(struct timespec start)
{
	struct timespec now, res;

	clock_gettime(CLOCK_REALTIME, &now);
	ntimersub(now, start, res);
	return (res.tv_sec)*1000L + (res.tv_nsec)/1000000L;
}
#else
long elapsed(START_TIME_TYPE start_time)
{
	struct timeval now, res;

	gettimeofday(&now, NULL);
	timersub(&now, &start_time, &res);
	return (res.tv_sec) * 1000 + (res.tv_usec) / 1000;
}
#endif

#define assert(a, b, c, d) myassert(__FILE__, __LINE__, a, b, c, d)
#define assert1(a, b, c, d, e) myassert(__FILE__, __LINE__, a, b, c, d, e)

#define MAXMSGS 30;

int tests = 0;
int failures = 0;
FILE* xml;
START_TIME_TYPE global_start_time;
char output[3000];
char* cur_output = output;


void write_test_result()
{
	long duration = elapsed(global_start_time);

	fprintf(xml, " time=\"%ld.%.3ld\" >\n", duration / 1000, duration % 1000); 
	if (cur_output != output)
	{
		fprintf(xml, "%s", output);
		cur_output = output;	
	}
	fprintf(xml, "</testcase>\n");
}

void myassert(char* filename, int lineno, char* description, int value,
		char* format, ...)
{
	++tests;
	if (!value)
	{
		va_list args;

		++failures;
		printf("Assertion failed, file %s, line %d, description: %s", filename,
				lineno, description);

		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		cur_output += sprintf(cur_output, "<failure type=\"%s\">file %s, line %d </failure>\n", 
                        description, filename, lineno);
	}
	else
		MyLog(LOGA_DEBUG,
				"Assertion succeeded, file %s, line %d, description: %s",
				filename, lineno, description);
}

/*********************************************************************

 Tests: offline buffering - sending messages while disconnected
 
 1. send some messages while disconnected, check that they are sent
 2. repeat test 1 using serverURIs
 3. repeat test 1 using auto reconnect
 4. repeast test 2 using auto reconnect
 5. check max-buffered
 6. check auto-reconnect parms alter behaviour as expected
 
 Tests: automatic reconnect
 
 - check that connected() is called 
 - check that reconnect() causes reconnect attempt
 - check that reconnect() fails if no connect has been previously attempted

 *********************************************************************/
 



/*********************************************************************

 Test1: offline buffering - sending messages while disconnected
 
 1. call connect
 2. use proxy to disconnect the client 
 3. while the client is disconnected, send more messages
 4. when the client reconnects, check that those messages are sent

 *********************************************************************/
 
int test1_will_message_received = 0; 
int test1_messages_received = 0;
 
int test1_messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	MQTTAsync c = (MQTTAsync)context;
	static int message_count = 0;
	int rc;

	MyLog(LOGA_DEBUG, "Message received on topic %s, \"%.*s\"", topicName, message->payloadlen, message->payload);
	
	if (memcmp(message->payload, "will message", message->payloadlen) == 0)
	  test1_will_message_received = 1;
	else
	  test1_messages_received++;

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);

	return 1;
}

int test1Finished = 0;

int test1OnFailureCalled = 0;

void test1cOnFailure(void* context, MQTTAsync_failureData* response)
{
	MyLog(LOGA_DEBUG, "In connect onFailure callback, context %p", context);

	test1OnFailureCalled++;
	test1Finished = 1;
}

void test1dOnFailure(void* context, MQTTAsync_failureData* response)
{
	MyLog(LOGA_DEBUG, "In connect onFailure callback, context %p", context);

	test1OnFailureCalled++;
	test1Finished = 1;
}

void test1cOnConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	MyLog(LOGA_DEBUG, "In connect onSuccess callback for client d, context %p\n", context);
	MQTTAsync c = (MQTTAsync)context;
	int rc;
	
	/* send a message to the proxy to break the connection */
	pubmsg.payload = "TERMINATE";
	pubmsg.payloadlen = strlen(pubmsg.payload);
	pubmsg.qos = 0;
	pubmsg.retained = 0;
	rc = MQTTAsync_sendMessage(c, "MQTTSAS topic", &pubmsg, NULL);
	assert("Good rc from sendMessage", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);
}


int test1dReady = 0;
char willTopic[100];
char test_topic[50];

void test1donSubscribe(void* context, MQTTAsync_successData* response)
{
	MQTTAsync c = (MQTTAsync)context;
	MyLog(LOGA_DEBUG, "In subscribe onSuccess callback for client d, %p granted qos %d", c, response->alt.qos);
  test1dReady = 1;
}


void test1dOnConnect(void* context, MQTTAsync_successData* response)
{
  MQTTAsync c = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;
	int qoss[2] = {2, 2};
	char* topics[2] = {willTopic, test_topic};

	MyLog(LOGA_DEBUG, "In connect onSuccess callback for client c, context %p\n", context);
	opts.onSuccess = test1donSubscribe;
	opts.context = c;

	rc = MQTTAsync_subscribeMany(c, 2, topics, qoss, &opts);
	assert("Good rc from subscribe", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);
	if (rc != MQTTASYNC_SUCCESS)
		test1Finished = 1;
}

int test1c_connected = 0;

void test1cConnected(void* context, char* cause)
{
  MQTTAsync c = (MQTTAsync)context;

	MyLog(LOGA_DEBUG, "In connected callback for client c, context %p\n", context);
  test1c_connected = 1;
}


int test1(struct Options options)
{
	char* testname = "test1";
	int subsqos = 2;
	MQTTAsync c, d;
	MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_willOptions wopts = MQTTAsync_willOptions_initializer;
	MQTTAsync_createOptions createOptions = MQTTAsync_createOptions_initializer;
	int rc = 0;
	int count = 0;
	char clientidc[50];
	char clientidd[50];
	int i = 0;
	MQTTAsync_token *tokens;
	
	sprintf(willTopic, "paho-test9-1-%s", unique);
	sprintf(clientidc, "paho-test9-1-c-%s", unique);
	sprintf(clientidd, "paho-test9-1-d-%s", unique);
	sprintf(test_topic, "paho-test9-1-test topic %s", unique);

	test1Finished = 0;
	failures = 0;
	MyLog(LOGA_INFO, "Starting Offline buffering 1 - messages while disconnected");
	fprintf(xml, "<testcase classname=\"test1\" name=\"%s\"", testname);
	global_start_time = start_clock();

  createOptions.sendWhileDisconnected = 1;
	rc = MQTTAsync_createWithOptions(&c, options.proxy_connection, clientidc, MQTTCLIENT_PERSISTENCE_DEFAULT, 
	      NULL, &createOptions);
	assert("good rc from create", rc == MQTTASYNC_SUCCESS, "rc was %d \n", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&c);
		goto exit;
	}
	
	rc = MQTTAsync_create(&d, options.connection, clientidd, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create", rc == MQTTASYNC_SUCCESS, "rc was %d \n", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&c);
		goto exit;
	}

	opts.keepAliveInterval = 20;
	opts.cleansession = 1;
	//opts.username = "testuser";
	//opts.password = "testpassword";
	
	rc = MQTTAsync_setCallbacks(d, d, NULL, test1_messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);

	opts.will = NULL; /* don't need will for this client, as it's going to be connected all the time */
	opts.context = d;
	opts.onSuccess = test1dOnConnect;
	opts.onFailure = test1dOnFailure;
	MyLog(LOGA_DEBUG, "Connecting client d");
	rc = MQTTAsync_connect(d, &opts);
	assert("Good rc from connect", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		failures++;
		goto exit;
	}
	
	/* wait until d is ready: connected and subscribed */
	count = 0;
	while (!test1dReady && ++count < 10000)
		MySleep(100);
	assert("Count should be less than 10000", count < 10000, "count was %d", count); /* wrong */
	
	rc = MQTTAsync_setConnected(c, c, test1cConnected);
	assert("Good rc from setConnectedCallback", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);
	
	/* let client c go: connect, and send disconnect command to proxy */
	opts.will = &wopts;
	opts.will->message = "will message";
	opts.will->qos = 1;
	opts.will->retained = 0;
	opts.will->topicName = willTopic;
	opts.onSuccess = test1cOnConnect;
	opts.onFailure = test1cOnFailure;
	opts.context = c;
	opts.cleansession = 0;

	MyLog(LOGA_DEBUG, "Connecting client c");
	rc = MQTTAsync_connect(c, &opts);
	assert("Good rc from connect", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		failures++;
		goto exit;
	}
		
	/* wait for will message */
	while (!test1_will_message_received && ++count < 10000)
		MySleep(100);
	
	MyLog(LOGA_DEBUG, "Now we can send some messages to be buffered");
	
	test1c_connected = 0;
	/* send some messages.  Then reconnect (check connected callback), and check that those messages are received */
	for (i = 0; i < 3; ++i)
	{
	  char buf[50];
	  
	  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	  sprintf(buf, "QoS %d message", i);
	  pubmsg.payload = buf;
	  pubmsg.payloadlen = strlen(pubmsg.payload) + 1;
	  pubmsg.qos = i;
	  pubmsg.retained = 0;
	  rc = MQTTAsync_sendMessage(c, test_topic, &pubmsg, &opts);
	  assert("Good rc from sendMessage", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
  }

  rc = MQTTAsync_getPendingTokens(c, &tokens);
 	assert("Good rc from getPendingTokens", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
 	i = 0;
 	while (tokens[i] != -1)
 	  ++i;
 	assert("Number of getPendingTokens should be 3", i == 3, "i was %d ", i);
  
  rc = MQTTAsync_reconnect(c);
 	assert("Good rc from reconnect", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
	
	/* wait for client to be reconnected */
	while (!test1c_connected == 0 && ++count < 10000)
		MySleep(100);
	
	/* wait for success or failure callback */
	while (test1_messages_received < 3 && ++count < 10000)
		MySleep(100);
		
  rc = MQTTAsync_disconnect(c, NULL);
 	assert("Good rc from disconnect", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
 	
  rc = MQTTAsync_disconnect(d, NULL);
 	assert("Good rc from disconnect", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);

exit:
  MQTTAsync_destroy(&c);
  MQTTAsync_destroy(&d);
	MyLog(LOGA_INFO, "%s: test %s. %d tests run, %d failures.",
			(failures == 0) ? "passed" : "failed", testname, tests, failures);
	write_test_result();
	return failures;
}


void handleTrace(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
	printf("%s\n", message);
}


int main(int argc, char** argv)
{
	int* numtests = &tests;
	int rc = 0;
	int (*tests[])() = 	{ NULL, test1, };
	
	sprintf(unique, "%u", rand());
	MyLog(LOGA_INFO, "Random prefix/suffix is %s", unique);

	xml = fopen("TEST-test9.xml", "w");
	fprintf(xml, "<testsuite name=\"test9\" tests=\"%lu\">\n", ARRAY_SIZE(tests) - 1);

	MQTTAsync_setTraceCallback(handleTrace);
	getopts(argc, argv);

	if (options.test_no == 0)
	{ /* run all the tests */
		for (options.test_no = 1; options.test_no < ARRAY_SIZE(tests); ++options.test_no)
		{
			failures = 0;
			MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_PROTOCOL);
			rc += tests[options.test_no](options); /* return number of failures.  0 = test succeeded */
		}
	}
	else
	{
		MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);
		rc = tests[options.test_no](options); /* run just the selected test */
	}

	MyLog(LOGA_INFO, "Total tests run: %d", *numtests);
	if (rc == 0)
		MyLog(LOGA_INFO, "verdict pass");
	else
		MyLog(LOGA_INFO, "verdict fail");

	fprintf(xml, "</testsuite>\n");
	fclose(xml);

	return rc;
}

