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

/*
 #if !defined(_RTSHEADER)
 #include <rts.h>
 #endif
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
#define snprintf _snprintf
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void usage()
{
	printf("Options:\n");
	printf("\t--test_no <test_no> - Run test number <test_no>\n");
	printf("\t--server <hostname> - Connect to <hostname> for tests\n");
	printf("\t--client_key <key_file> - Use <key_file> as the client certificate for SSL authentication\n");
	printf("\t--client_key_pass <password> - Use <password> to access the private key in the client certificate\n");
	printf("\t--server_key <key_file> - Use <key_file> as the trusted certificate for server\n");
	printf("\t--verbose - Enable verbose output \n");
	printf("\t--help - This help output\n");
	exit(-1);
}

struct Options
{
	char connection[100]; /**< connection to system under test. */
	char mutual_auth_connection[100];   /**< connection to system under test. */
	char nocert_mutual_auth_connection[100];
	char server_auth_connection[100];
	char anon_connection[100];
	char* client_key_file;
	char* client_key_pass;
	char* server_key_file;
	char* client_private_key_file;
	int verbose;
	int test_no;
	int size;
} options =
{ 
	"ssl://m2m.eclipse.org:18883",
	"ssl://m2m.eclipse.org:18884",
	"ssl://m2m.eclipse.org:18887",
	"ssl://m2m.eclipse.org:18885",
	"ssl://m2m.eclipse.org:18886",
	"../../../test/ssl/client.pem",
	NULL,
	"../../../test/ssl/test-root-ca.crt",
	NULL,
	0,
	0,
	5000000
};

typedef struct
{
	MQTTAsync client;
	char clientid[24];
	char topic[100];
	int maxmsgs;
	int rcvdmsgs[3];
	int sentmsgs[3];
	int testFinished;
	int subscribed;
} AsyncTestClient;

#define AsyncTestClient_initializer {NULL, "\0", "\0", 0, {0, 0, 0}, {0, 0, 0}, 0, 0}

void getopts(int argc, char** argv)
{
	int count = 1;

	while (count < argc)
	{
		if (strcmp(argv[count], "--help") == 0)
		{
			usage();
		}
		else if (strcmp(argv[count], "--test_no") == 0)
		{
			if (++count < argc)
				options.test_no = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--client_key") == 0)
		{
			if (++count < argc)
				options.client_key_file = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--client_key_pass") == 0)
		{
			if (++count < argc)
				options.client_key_pass = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--server_key") == 0)
		{
			if (++count < argc)
				options.server_key_file = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--verbose") == 0)
		{
			options.verbose = 1;
			printf("\nSetting verbose on\n");
		}
		else if (strcmp(argv[count], "--hostname") == 0)
		{
			if (++count < argc)
			{
				sprintf(options.connection, "ssl://%s:18883", argv[count]);
				printf("Setting connection to %s\n", options.connection);
				sprintf(options.mutual_auth_connection, "ssl://%s:18884", argv[count]);
				printf("Setting mutual_auth_connection to %s\n", options.mutual_auth_connection);
				sprintf(options.nocert_mutual_auth_connection, "ssl://%s:18887", argv[count]);
				printf("Setting nocert_mutual_auth_connection to %s\n",
					options.nocert_mutual_auth_connection);
				sprintf(options.server_auth_connection, "ssl://%s:18885", argv[count]);
				printf("Setting server_auth_connection to %s\n", options.server_auth_connection);
				sprintf(options.anon_connection, "ssl://%s:18886", argv[count]);
				printf("Setting anon_connection to %s\n", options.anon_connection);
			}
			else
				usage();
		}
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
 2. check max-buffered
 3. check auto-reconnect parms alter behaviour as expected

 *********************************************************************/
 



/*********************************************************************

 Test1: offline buffering - sending messages while disconnected
 
 1. call connect
 2. use proxy to disconnect the client 
 3. while the client is disconnected, send more messages
 4. when the client reconnects, check that those messages are sent

 *********************************************************************/

int test1Finished = 0;

int test1OnFailureCalled = 0;

void test1cOnFailure(void* context, MQTTAsync_failureData* response)
{
	MyLog(LOGA_DEBUG, "In connect onFailure callback, context %p", context);

	test1OnFailureCalled++;
	test1Finished = 1;
}

void test1cOnFailure(void* context, MQTTAsync_failureData* response)
{
	MyLog(LOGA_DEBUG, "In connect onFailure callback, context %p", context);

	test1OnFailureCalled++;
	test1Finished = 1;
}

void test1cOnConnect(void* context, MQTTAsync_successData* response)
{

	MyLog(LOGA_DEBUG, "In connect onSuccess callback, context %p\n", context);

	

	
}


int test1(struct Options options)
{
	char* testname = "test1";
	int subsqos = 2;
	MQTTAsync c, d;
	MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_willOptions wopts = MQTTAsync_willOptions_initializer;
	int rc = 0;
	char* test_topic = "C client offline buffering test";
	int count = 0;

	test1Finished = 0;
	failures = 0;
	MyLog(LOGA_INFO, "Starting Offline buffering 1 - messages while disconnected");
	fprintf(xml, "<testcase classname=\"test1\" name=\"%s\"", testname);
	global_start_time = start_clock();

	rc = MQTTAsync_create(&c, options.connection, "paho-test9-test1-c", MQTTCLIENT_PERSISTENCE_DEFAULT,
			NULL);
	assert("good rc from create", rc == MQTTASYNC_SUCCESS, "rc was %d \n", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&c);
		goto exit;
	}
	
	rc = MQTTAsync_create(&d, options.connection, "paho-test9-test1-d", MQTTCLIENT_PERSISTENCE_DEFAULT,
			NULL);
	assert("good rc from create", rc == MQTTASYNC_SUCCESS, "rc was %d \n", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&c);
		goto exit;
	}

	opts.keepAliveInterval = 20;
	opts.cleansession = 1;
	opts.username = "testuser";
	opts.password = "testpassword";

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
	
	/* let client c go: connect, publish some messages, and send disconnect command to proxy */
	opts.will = &wopts;
	opts.will->message = "will message";
	opts.will->qos = 1;
	opts.will->retained = 0;
	opts.will->topicName = "will topic";
	opts.onSuccess = test1cOnConnect;
	opts.onFailure = test1cOnFailure;
	opts.context = c;

	MyLog(LOGA_DEBUG, "Connecting client c");
	rc = MQTTAsync_connect(c, &opts);
	assert("Good rc from connect", rc == MQTTASYNC_SUCCESS, "rc was %d ", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		failures++;
		goto exit;
	}
	
	/* wait for success or failure callback */
	while (!test1Finished && ++count < 10000)
		MySleep(100);

	exit: MQTTAsync_destroy(&c);
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

	xml = fopen("TEST-test9.xml", "w");
	fprintf(xml, "<testsuite name=\"test9\" tests=\"%lu\">\n", ARRAY_SIZE(tests) - 1);

	MQTTAsync_setTraceCallback(handleTrace);
	getopts(argc, argv);

	if (options.test_no == 0)
	{ /* run all the tests */
		for (options.test_no = 1; options.test_no < ARRAY_SIZE(tests); ++options.test_no)
		{
			failures = 0;
			MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);
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

