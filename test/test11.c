/*******************************************************************************
 * Copyright (c) 2009, 2018 IBM Corp.
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
 *    Ian Craggs - MQTT 3.1.1 support
 *    Ian Craggs - test8 - failure callbacks
 *    Ian Craggs - MQTT V5
 *******************************************************************************/


/**
 * @file
 * Tests for the Paho Asynchronous MQTT C client
 */


#include "MQTTAsync.h"
#include <string.h>
#include <stdlib.h>

#if !defined(_WINDOWS)
	#include <sys/time.h>
  #include <sys/socket.h>
	#include <unistd.h>
  #include <errno.h>
#else
	#include <windows.h>
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void usage(void)
{
	printf("help!!\n");
	exit(EXIT_FAILURE);
}

struct Options
{
	char* connection;         /**< connection to system under test. */
	int verbose;
	int test_no;
	int size;									/**< size of big message */
	int MQTTVersion;
	int iterations;
} options =
{
	"localhost:1883",
	0,
	-1,
	10000,
	MQTTVERSION_5,
	1,
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
		else if (strcmp(argv[count], "--size") == 0)
		{
			if (++count < argc)
				options.size = atoi(argv[count]);
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
	vsnprintf(&msg_buf[strlen(msg_buf)], sizeof(msg_buf) - strlen(msg_buf), format, args);
	va_end(args);

	printf("%s\n", msg_buf);
	fflush(stdout);
}
#endif


#if defined(WIN32) || defined(_WINDOWS)
#define mqsleep(A) Sleep(1000*A)
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
#define mqsleep sleep
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
	return (res.tv_sec)*1000 + (res.tv_usec)/1000;
}
#endif

#define assert(a, b, c, d) myassert(__FILE__, __LINE__, a, b, c, d)
#define assert1(a, b, c, d, e) myassert(__FILE__, __LINE__, a, b, c, d, e)

int tests = 0;
int failures = 0;
FILE* xml;
START_TIME_TYPE global_start_time;
char output[3000];
char* cur_output = output;

void write_test_result(void)
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

		cur_output += sprintf(cur_output, "<failure type=\"%s\">file %s, line %d </failure>\n",
                        description, filename, lineno);
	}
    else
    	MyLog(LOGA_DEBUG, "Assertion succeeded, file %s, line %d, description: %s", filename, lineno, description);
}


void logProperties(MQTTProperties *props)
{
	int i = 0;

	for (i = 0; i < props->count; ++i)
	{
		int id = props->array[i].identifier;
		const char* name = MQTTPropertyName(id);
		char* intformat = "Property name %s value %d";

		switch (MQTTProperty_getType(id))
		{
		case PROPERTY_TYPE_BYTE:
		  MyLog(LOGA_INFO, intformat, name, props->array[i].value.byte);
		  break;
		case TWO_BYTE_INTEGER:
		  MyLog(LOGA_INFO, intformat, name, props->array[i].value.integer2);
		  break;
		case FOUR_BYTE_INTEGER:
		  MyLog(LOGA_INFO, intformat, name, props->array[i].value.integer4);
		  break;
		case VARIABLE_BYTE_INTEGER:
		  MyLog(LOGA_INFO, intformat, name, props->array[i].value.integer4);
		  break;
		case BINARY_DATA:
		case UTF_8_ENCODED_STRING:
		  MyLog(LOGA_INFO, "Property name %s value len %.*s", name,
				  props->array[i].value.data.len, props->array[i].value.data.data);
		  break;
		case UTF_8_STRING_PAIR:
		  MyLog(LOGA_INFO, "Property name %s key %.*s value %.*s", name,
			  props->array[i].value.data.len, props->array[i].value.data.data,
		  	  props->array[i].value.value.len, props->array[i].value.value.data);
		  break;
		}
	}
}


struct aa
{
	int disconnected;
	char* test_topic;
	int test_finished;
} test_client_topic_aliases_globals =
{
	0, "test topic aliases globals", 0
};


void test1_onDisconnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync c = (MQTTAsync)context;
	MyLog(LOGA_DEBUG, "In onDisconnect callback %p", c);
	test_client_topic_aliases_globals.test_finished = 1;
}


int test1_messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	MQTTAsync c = (MQTTAsync)context;
	static int message_count = 0;
	int rc;

	MyLog(LOGA_DEBUG, "In messageArrived callback %p", c);

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);

	return 1;
}


void test_client_topic_aliases_disconnected(void* context, MQTTProperties* props, enum MQTTReasonCodes rc)
{
	MQTTAsync c = (MQTTAsync)context;
	MyLog(LOGA_INFO, "Callback: disconnected, reason code \"%s\"", MQTTReasonCodeString(rc));
	logProperties(props);
	test_client_topic_aliases_globals.disconnected = 1;
}


void test_client_topic_aliases_onConnect(void* context, MQTTAsync_successData5* response)
{
	MQTTAsync c = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTProperty property;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	int rc;

	MyLog(LOGA_DEBUG, "In connect onSuccess callback, context %p", context);

	assert("Reason code should be 0", response->reasonCode == SUCCESS,
		   "Reason code was %d\n", response->reasonCode);

	MyLog(LOGA_INFO, "Connack properties:");
	logProperties(&response->props);

	pubmsg.payload = "a much longer message that we can shorten to the extent that we need to payload up to 11";
	pubmsg.payloadlen = 11;
	pubmsg.qos = 1;
	pubmsg.retained = 0;

	/* a Topic Alias of 0 is not allowed, so we should be disconnected */
	property.identifier = TOPIC_ALIAS;
	property.value.integer2 = 0;
	MQTTProperties_add(&pubmsg.properties, &property);

	rc = MQTTAsync_sendMessage(c, test_client_topic_aliases_globals.test_topic, &pubmsg, &opts);
	assert("Good rc from connect", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);
	if (rc != MQTTASYNC_SUCCESS)
		test_client_topic_aliases_globals.test_finished = 1;
	MQTTProperties_free(&pubmsg.properties);
}


/*********************************************************************

Test1: client topic aliases

*********************************************************************/
int test_client_topic_aliases(struct Options options)
{
	int subsqos = 2;
	MQTTAsync c;
	MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer5;
	MQTTAsync_willOptions wopts = MQTTAsync_willOptions_initializer;
	MQTTProperties props = MQTTProperties_initializer;
	MQTTProperties willProps = MQTTProperties_initializer;
	MQTTProperty property;
	int rc = 0;
	char* test_topic = "V5 C client test client topic aliases";

	MyLog(LOGA_INFO, "Starting V5 test 1 - client topic aliases");
	fprintf(xml, "<testcase classname=\"test11\" name=\"client topic aliases\"");
	global_start_time = start_clock();

	rc = MQTTAsync_create(&c, options.connection, "async_test",
			MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTASYNC_SUCCESS, "rc was %d\n", rc);
	if (rc != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&c);
		goto exit;
	}

	rc = MQTTAsync_setCallbacks(c, c, NULL, test1_messageArrived, NULL);
	assert("Good rc from setCallbacks", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);

	rc = MQTTAsync_setDisconnected(c, c, test_client_topic_aliases_disconnected);

	opts.keepAliveInterval = 20;
	opts.username = "testuser";
	opts.password = "testpassword";
	opts.MQTTVersion = options.MQTTVersion;

	opts.will = &wopts;
	opts.will->message = "will message";
	opts.will->qos = 1;
	opts.will->retained = 0;
	opts.will->topicName = "will topic";
	opts.will = NULL;
	opts.onSuccess5 = test_client_topic_aliases_onConnect;
	opts.onFailure5 = NULL;
	opts.context = c;

	property.identifier = SESSION_EXPIRY_INTERVAL;
	property.value.integer4 = 30;
	MQTTProperties_add(&props, &property);

	property.identifier = USER_PROPERTY;
	property.value.data.data = "test user property";
	property.value.data.len = strlen(property.value.data.data);
	property.value.value.data = "test user property value";
	property.value.value.len = strlen(property.value.value.data);
	MQTTProperties_add(&props, &property);

	opts.connectProperties = &props;
	opts.willProperties = &willProps;

	MyLog(LOGA_DEBUG, "Connecting");
	rc = MQTTAsync_connect(c, &opts);
	assert("Good rc from connect", rc == MQTTASYNC_SUCCESS, "rc was %d", rc);
	if (rc != MQTTASYNC_SUCCESS)
		goto exit;

	while (test_client_topic_aliases_globals.disconnected == 0)
		#if defined(WIN32)
			Sleep(100);
		#else
			usleep(10000L);
		#endif

	MQTTProperties_free(&props);
	MQTTProperties_free(&willProps);
	MQTTAsync_destroy(&c);

exit:
	MyLog(LOGA_INFO, "TEST1: test %s. %d tests run, %d failures.",
			(failures == 0) ? "passed" : "failed", tests, failures);
	write_test_result();
	return failures;
}


void trace_callback(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
	printf("Trace : %d, %s\n", level, message);
}


int main(int argc, char** argv)
{
	int rc = 0;
 	int (*tests[])() = {NULL, test_client_topic_aliases}; /* indexed starting from 1 */
	MQTTAsync_nameValue* info;
	int i;

	xml = fopen("TEST-test4.xml", "w");
	fprintf(xml, "<testsuite name=\"test4\" tests=\"%d\">\n", (int)(ARRAY_SIZE(tests)) - 1);

	getopts(argc, argv);

	MQTTAsync_setTraceCallback(trace_callback);

	info = MQTTAsync_getVersionInfo();
	while (info->name)
	{
	  MyLog(LOGA_INFO, "%s: %s", info->name, info->value);
	  info++;
	}

	for (i = 0; i < options.iterations; ++i)
	{
	 	if (options.test_no == -1)
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
	}

	if (rc == 0)
		MyLog(LOGA_INFO, "verdict pass");
	else
		MyLog(LOGA_INFO, "verdict fail");

	fprintf(xml, "</testsuite>\n");
	fclose(xml);

	return rc;
}
