/*******************************************************************************
 * Copyright (c) 2020 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    https://www.eclipse.org/legal/epl-2.0/
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial implementation
 *******************************************************************************/

#include "MQTTTime.h"
#include "StackTrace.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

void MQTTTime_sleep(ELAPSED_TIME_TYPE milliseconds)
{
	FUNC_ENTRY;
#if defined(_WIN32) || defined(_WIN64)
	Sleep((DWORD)milliseconds);
#else
	usleep((useconds_t)(milliseconds*1000));
#endif
	FUNC_EXIT;
}

#if defined(_WIN32) || defined(_WIN64)
START_TIME_TYPE MQTTTime_start_clock(void)
{
#if WINVER >= _WIN32_WINNT_VISTA
	return GetTickCount64();
#else
	return GetTickCount();
#endif
}
#elif defined(AIX)
START_TIME_TYPE MQTTTime_start_clock(void)
{
	static struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	return start;
}
#else
START_TIME_TYPE MQTTTime_start_clock(void)
{
	static struct timeval start;
	static struct timespec start_ts;

	clock_gettime(CLOCK_MONOTONIC, &start_ts);
	start.tv_sec = start_ts.tv_sec;
	start.tv_usec = start_ts.tv_nsec / 1000;
	return start;
}
#endif
START_TIME_TYPE MQTTTime_now(void)
{
	return MQTTTime_start_clock();
}


#if defined(_WIN32) || defined(_WIN64)
/*
 * @param new most recent time in milliseconds from GetTickCount()
 * @param old older time in milliseconds from GetTickCount()
 * @return difference in milliseconds
 */
DIFF_TIME_TYPE MQTTTime_difftime(START_TIME_TYPE new, START_TIME_TYPE old)
{
#if WINVER >= _WIN32_WINNT_VISTA
	return (DIFF_TIME_TYPE )(new - old);
#else
	if (old < new)       /* check for wrap around condition in GetTickCount */
		return (DIFF_TIME_TYPE)(new - old);
	else
	    return (DIFF_TIME_TYPE)((0xFFFFFFFFL - old) + 1 + new);
#endif
}
#elif defined(AIX)
#define assert(a)
DIFF_TIME_TYPE MQTTTime_difftime(START_TIME_TYPE new, START_TIME_TYPE old)
{
	struct timespec result;

	ntimersub(new, old, result);
	return (DIFF_TIME_TYPE)((result.tv_sec)*1000L + (result.tv_nsec)/1000000L); /* convert to milliseconds */
}
#else
DIFF_TIME_TYPE MQTTTime_difftime(START_TIME_TYPE new, START_TIME_TYPE old)
{
	struct timeval result;

	timersub(&new, &old, &result);
	return (DIFF_TIME_TYPE)(((DIFF_TIME_TYPE)result.tv_sec)*1000 + ((DIFF_TIME_TYPE)result.tv_usec)/1000); /* convert to milliseconds */
}
#endif


ELAPSED_TIME_TYPE MQTTTime_elapsed(START_TIME_TYPE milliseconds)
{
	return (ELAPSED_TIME_TYPE)MQTTTime_difftime(MQTTTime_now(), milliseconds);
}
