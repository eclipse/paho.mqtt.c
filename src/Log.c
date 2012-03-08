/*******************************************************************************
 * Copyright (c) 2009, 2012 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "Log.h"
#include "MQTTPacket.h"
#include "MQTTProtocol.h"
#include "MQTTProtocolClient.h"
#include "Messages.h"
#include "LinkedList.h"
#include "StackTrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#if !defined(WIN32)
#include <syslog.h>
#define GETTIMEOFDAY 1
#else
#define snprintf _snprintf
#endif

#if defined(GETTIMEOFDAY)
	#include <sys/time.h>
#else
	#include <sys/timeb.h>
#endif


#if !defined(min)
#define min(A,B) ( (A) < (B) ? (A):(B))
#endif

trace_settings_type trace_settings =
{
	TRACE_MINIMUM,
	400,
	-1
};

#define MAX_FUNCTION_NAME_LENGTH 256

typedef struct
{
#if defined(GETTIMEOFDAY)
	struct timeval ts;
#else
	struct timeb ts;
#endif
	int sametime_count;
	int number;
	int thread_id;
	int depth;
	char name[MAX_FUNCTION_NAME_LENGTH + 1];
	int line;
	int has_rc;
	int rc;
	int level;
} traceEntry;

static int start_index = -1,
			next_index = 0;
static traceEntry* trace_queue = NULL;
static int trace_queue_size = 0;

static FILE* trace_destination = NULL;	/**< flag to indicate if trace is to be sent to a stream */
static int trace_output_level = -1;

static int sametime_count = 0;
#if defined(GETTIMEOFDAY)
struct timeval ts, last_ts;
#else
struct timeb ts, last_ts;
#endif
static char msg_buf[512];


int Log_initialize()
{
	int rc = -1;
	char* envval = NULL;

	if ((trace_queue = malloc(sizeof(traceEntry) * trace_settings.max_trace_entries)) == NULL)
		return rc;
	trace_queue_size = trace_settings.max_trace_entries;

	if ((envval = getenv("MQTT_C_CLIENT_TRACE")) != NULL && strlen(envval) > 0)
	{
		if (strcmp(envval, "ON") == 0 || (trace_destination = fopen(envval, "wt")) == NULL)
			trace_destination = stdout;
	}
	if ((envval = getenv("MQTT_C_CLIENT_TRACE_LEVEL")) != NULL && strlen(envval) > 0)
	{
		if (strcmp(envval, "MAXIMUM") == 0 || strcmp(envval, "TRACE_MAXIMUM") == 0)
			trace_settings.trace_level = TRACE_MAXIMUM;
		else if (strcmp(envval, "MEDIUM") == 0 || strcmp(envval, "TRACE_MEDIUM") == 0)
			trace_settings.trace_level = TRACE_MEDIUM;
		else if (strcmp(envval, "MINIMUM") == 0 || strcmp(envval, "TRACE_MEDIUM") == 0)
			trace_settings.trace_level = TRACE_MINIMUM;
		else if (strcmp(envval, "PROTOCOL") == 0  || strcmp(envval, "TRACE_PROTOCOL") == 0)
			trace_output_level = TRACE_PROTOCOL;
	}
	return rc;
}


void Log_terminate()
{
	free(trace_queue);
	trace_queue = NULL;
	trace_queue_size = 0;
	if (trace_destination)
	{
		if (trace_destination != stdout)
			fclose(trace_destination);
		trace_destination = NULL;
	}
	start_index = -1;
	next_index = 0;
	trace_output_level = -1;
	sametime_count = 0;
}


static traceEntry* Log_pretrace()
{
	traceEntry *cur_entry = NULL;

	/* calling ftime/gettimeofday seems to be comparatively expensive, so we need to limit its use */
	if (++sametime_count % 20 == 0)
	{
#if defined(GETTIMEOFDAY)
		gettimeofday(&ts, NULL);
		if (ts.tv_sec != last_ts.tv_sec || ts.tv_usec != last_ts.tv_usec)
#else
		ftime(&ts);
		if (ts.time != last_ts.time || ts.millitm != last_ts.millitm)
#endif
		{
			sametime_count = 0;
			last_ts = ts;
		}
	}

	if (trace_queue_size != trace_settings.max_trace_entries)
	{
		traceEntry* new_trace_queue = malloc(sizeof(traceEntry) * trace_settings.max_trace_entries);

		memcpy(new_trace_queue, trace_queue, min(trace_queue_size, trace_settings.max_trace_entries) * sizeof(traceEntry));
		free(trace_queue);
		trace_queue = new_trace_queue;
		trace_queue_size = trace_settings.max_trace_entries;

		if (start_index > trace_settings.max_trace_entries + 1 ||
				next_index > trace_settings.max_trace_entries + 1)
		{
			start_index = -1;
			next_index = 0;
		}
	}

	/* add to trace buffer */
	cur_entry = &trace_queue[next_index];
	if (next_index == start_index) /* means the buffer is full */
	{
		if (++start_index == trace_settings.max_trace_entries)
			start_index = 0;
	} else if (start_index == -1)
		start_index = 0;
	if (++next_index == trace_settings.max_trace_entries)
		next_index = 0;

	return cur_entry;
}


static char* Log_formatTraceEntry(traceEntry* cur_entry)
{
	struct tm *timeinfo;
	int buf_pos = 31;

#if defined(GETTIMEOFDAY)
	timeinfo = localtime(&cur_entry->ts.tv_sec);
#else
	timeinfo = localtime(&cur_entry->ts.time);
#endif
	strftime(&msg_buf[7], 80, "%Y%m%d %H%M%S ", timeinfo);
#if defined(GETTIMEOFDAY)
	sprintf(&msg_buf[22], ".%.3lu ", cur_entry->ts.tv_usec / 1000L);
#else
	sprintf(&msg_buf[22], ".%.3hu ", cur_entry->ts.millitm);
#endif
	buf_pos = 27;

	sprintf(msg_buf, "(%.4d)", cur_entry->sametime_count);
	msg_buf[6] = ' ';

	if (cur_entry->has_rc == 2)
		strncpy(&msg_buf[buf_pos], cur_entry->name, sizeof(msg_buf)-buf_pos);
	else
	{
		char* format = Messages_get(cur_entry->number, cur_entry->level);
		if (cur_entry->has_rc == 1)
			snprintf(&msg_buf[buf_pos], sizeof(msg_buf)-buf_pos, format, cur_entry->thread_id,
					cur_entry->depth, "", cur_entry->depth, cur_entry->name, cur_entry->line, cur_entry->rc);
		else
			snprintf(&msg_buf[buf_pos], sizeof(msg_buf)-buf_pos, format, cur_entry->thread_id,
					cur_entry->depth, "", cur_entry->depth, cur_entry->name, cur_entry->line);
	}
	return msg_buf;
}


static void Log_posttrace(int log_level, traceEntry* cur_entry)
{
	if (trace_destination &&
		((trace_output_level == -1) ? log_level >= trace_settings.trace_level : log_level >= trace_output_level))
	{
		fprintf(trace_destination, "%s\n", &Log_formatTraceEntry(cur_entry)[7]);
		fflush(trace_destination);
	}
}


static void Log_trace(int log_level, char* buf)
{
	traceEntry *cur_entry = NULL;

	if (trace_queue == NULL)
		return;

	cur_entry = Log_pretrace();

	memcpy(&(cur_entry->ts), &ts, sizeof(ts));
	cur_entry->sametime_count = sametime_count;

	cur_entry->has_rc = 2;
	strncpy(cur_entry->name, buf, sizeof(cur_entry->name));
	cur_entry->name[MAX_FUNCTION_NAME_LENGTH] = '\0';

	Log_posttrace(log_level, cur_entry);
}


/**
 * Log a message.  If possible, all messages should be indexed by message number, and
 * the use of the format string should be minimized or negated altogether.  If format is
 * provided, the message number is only used as a message label.
 * @param log_level the log level of the message
 * @param msgno the id of the message to use if the format string is NULL
 * @param aFormat the printf format string to be used if the message id does not exist
 * @param ... the printf inserts
 */
void Log(int log_level, int msgno, char* format, ...)
{
	char* temp = NULL;
	static char msg_buf[512];

	if (log_level >= trace_settings.trace_level)
	{
		va_list args;

		if (format == NULL && (temp = Messages_get(msgno, log_level)) != NULL)
			format = temp;

		va_start(args, format);
		vsnprintf(msg_buf, sizeof(msg_buf), format, args);

		Log_trace(log_level, msg_buf);
		va_end(args);
	}

	/*if (log_level >= LOG_ERROR)
	{
		char* filename = NULL;
		Log_recordFFDC(&msg_buf[7]);
	}

	if (log_level == LOG_FATAL)
		exit(-1);*/
}


/**
 * The reason for this function is to make trace logging as fast as possible so that the
 * function exit/entry history can be captured by default without unduly impacting
 * performance.  Therefore it must do as little as possible.
 * @param log_level the log level of the message
 * @param msgno the id of the message to use if the format string is NULL
 * @param aFormat the printf format string to be used if the message id does not exist
 * @param ... the printf inserts
 */
void Log_stackTrace(int log_level, int msgno, int thread_id, int current_depth, const char* name, int line, int* rc)
{
	traceEntry *cur_entry = NULL;

	if (trace_queue == NULL)
		return;

	if (log_level < trace_settings.trace_level)
		return;

	cur_entry = Log_pretrace();

	memcpy(&(cur_entry->ts), &ts, sizeof(ts));
	cur_entry->sametime_count = sametime_count;
	cur_entry->number = msgno;
	cur_entry->thread_id = thread_id;
	cur_entry->depth = current_depth;
	strcpy(cur_entry->name, name);
	cur_entry->level = log_level;
	cur_entry->line = line;
	if (rc == NULL)
		cur_entry->has_rc = 0;
	else
	{
		cur_entry->has_rc = 1;
		cur_entry->rc = *rc;
	}

	Log_posttrace(log_level, cur_entry);
}


FILE* Log_destToFile(char* dest)
{
	FILE* file = NULL;

	if (strcmp(dest, "stdout") == 0)
		file = stdout;
	else if (strcmp(dest, "stderr") == 0)
		file = stderr;
	else
	{
		if (strstr(dest, "FFDC"))
			file = fopen(dest, "ab");
		else
			file = fopen(dest, "wb");
	}
	return file;
}


int Log_compareEntries(char* entry1, char* entry2)
{
	int comp = strncmp(&entry1[7], &entry2[7], 19);

	/* if timestamps are equal, use the sequence numbers */
	if (comp == 0)
		comp = strncmp(&entry1[1], &entry2[1], 4);

	return comp;
}


#if 0
/**
 * Write the contents of the stored trace to a stream
 * @param dest string which contains a file name or the special strings stdout or stderr
 */
int Log_dumpTrace(char* dest)
{
	FILE* file = NULL;
	ListElement* cur_trace_entry = NULL;
	const int msgstart = 7;
	int rc = -1;
	int trace_queue_index = 0;

	if ((file = Log_destToFile(dest)) == NULL)
	{
		Log(LOG_ERROR, 9, NULL, "trace", dest, "trace entries");
		goto exit;
	}

	fprintf(file, "=========== Start of trace dump ==========\n");
	/* Interleave the log and trace entries together appropriately */
	ListNextElement(trace_buffer, &cur_trace_entry);
	trace_queue_index = start_index;
	if (trace_queue_index == -1)
		trace_queue_index = next_index;
	else
	{
		Log_formatTraceEntry(&trace_queue[trace_queue_index++]);
		if (trace_queue_index == trace_settings.max_trace_entries)
			trace_queue_index = 0;
	}
	while (cur_trace_entry || trace_queue_index != next_index)
	{
		if (cur_trace_entry && trace_queue_index != -1)
		{	/* compare these timestamps */
			if (Log_compareEntries((char*)cur_trace_entry->content, msg_buf) > 0)
				cur_trace_entry = NULL;
		}

		if (cur_trace_entry)
		{
			fprintf(file, "%s\n", &((char*)(cur_trace_entry->content))[msgstart]);
			ListNextElement(trace_buffer, &cur_trace_entry);
		}
		else
		{
			fprintf(file, "%s\n", &msg_buf[7]);
			if (trace_queue_index != next_index)
			{
				Log_formatTraceEntry(&trace_queue[trace_queue_index++]);
				if (trace_queue_index == trace_settings.max_trace_entries)
					trace_queue_index = 0;
			}
		}
	}
	fprintf(file, "========== End of trace dump ==========\n\n");
	if (file != stdout && file != stderr && file != NULL)
		fclose(file);
	rc = 0;
exit:
	return rc;
}
#endif


