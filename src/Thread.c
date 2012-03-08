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

/**
 * @file
 * \brief Threading related functions
 *
 * Used to create platform independent threading functions
 */


#include "Thread.h"
#if defined(THREAD_UNIT_TESTS)
#define NOSTACKTRACE
#endif
#include "StackTrace.h"

#undef malloc
#undef realloc
#undef free

#if !defined(WIN32)
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#endif
#include <memory.h>
#include <stdlib.h>

/**
 * Start a new thread
 * @param fn the function to run, must be of the correct signature
 * @param parameter pointer to the function parameter, can be NULL
 * @return the new thread
 */
thread_type Thread_start(thread_fn fn, void* parameter)
{
	#if defined(WIN32)
	thread_type thread = NULL;
#else
	thread_type thread = 0;
#endif

	FUNC_ENTRY;
#if defined(WIN32)
	thread = CreateThread(NULL, 0, fn, parameter, 0, NULL);
#else
	if (pthread_create(&thread, NULL, fn, parameter) != 0)
		thread = 0;
#endif
	FUNC_EXIT;
	return thread;
}


/**
 * Create a new mutex
 * @return the new mutex
 */
mutex_type Thread_create_mutex()
{
	mutex_type mutex = NULL;
	int rc = 0;

	FUNC_ENTRY;
	#if defined(WIN32)
		mutex = CreateMutex(NULL, 0, NULL);
	#else
		mutex = malloc(sizeof(pthread_mutex_t));
		rc = pthread_mutex_init(mutex, NULL);
	#endif
	FUNC_EXIT_RC(rc);
	return mutex;
}


/**
 * Lock a mutex which has already been created, block until ready
 * @param mutex the mutex
 * @return completion code
 */
int Thread_lock_mutex(mutex_type mutex)
{
	int rc = -1;

	/* don't add entry/exit trace points as the stack log uses mutexes - recursion beckons */
	#if defined(WIN32)
		if (WaitForSingleObject(mutex, INFINITE) != WAIT_FAILED)
	#else
		if ((rc = pthread_mutex_lock(mutex)) == 0)
	#endif
		rc = 0;

	return rc;
}


/**
 * Unlock a mutex which has already been locked
 * @param mutex the mutex
 * @return completion code
 */
int Thread_unlock_mutex(mutex_type mutex)
{
	int rc = -1;

	/* don't add entry/exit trace points as the stack log uses mutexes - recursion beckons */
	#if defined(WIN32)
		if (ReleaseMutex(mutex) != 0)
	#else
		if ((rc = pthread_mutex_unlock(mutex)) == 0)
	#endif
		rc = 0;

	return rc;
}


/**
 * Destroy a mutex which has already been created
 * @param mutex the mutex
 */
void Thread_destroy_mutex(mutex_type mutex)
{
	int rc = 0;

	FUNC_ENTRY;
	#if defined(WIN32)
		rc = CloseHandle(mutex);
	#else
		rc = pthread_mutex_destroy(mutex);
		free(mutex);
	#endif
	FUNC_EXIT_RC(rc);
}


/**
 * Get the thread id of the thread from which this function is called
 * @return thread id, type varying according to OS
 */
thread_id_type Thread_getid()
{
	#if defined(WIN32)
		return GetCurrentThreadId();
	#else
		return pthread_self();
	#endif
}


/**
 * Create a new semaphore
 * @return the new condition variable
 */
sem_type Thread_create_sem()
{
	sem_type sem = NULL;
	int rc = 0;

	FUNC_ENTRY;
	#if defined(WIN32)
		sem = CreateEvent(
		        NULL,               // default security attributes
		        FALSE,              // manual-reset event?
		        FALSE,              // initial state is nonsignaled
		        NULL                // object name
		        );
	#else
	  sem = malloc(sizeof(sem_t));
	  rc = sem_init(sem, 0, 0);
	#endif
	FUNC_EXIT_RC(rc);
	return sem;
}


/**
 * Lock a mutex which has already been created, block until ready
 * @param mutex the mutex
 * @return completion code
 */
int Thread_wait_sem(sem_type sem)
{
/* sem_timedwait is the obvious call to use, but seemed not to work on the Viper,
 * so I've used trywait in a loop instead. Ian Craggs 23/7/2010
 */
	int rc = -1;
	int timeout = 10; /* seconds */
#define USE_TRYWAIT
#if defined(USE_TRYWAIT)
	int i = 0;
	int interval = 10000;
	int count = (1000000 / interval) * timeout;
#elif !defined(WIN32)
	struct timespec ts;
#endif

	FUNC_ENTRY;
	#if defined(WIN32)
		rc = WaitForSingleObject(sem, timeout*1000L);
	#elif defined(USE_TRYWAIT)
		while (++i < count && (rc = sem_trywait(sem)) != 0)
		{
			if (rc == -1 && ((rc = errno) != EAGAIN))
				break;
			usleep(interval); /* microseconds - .1 of a second */
		}
	#else
		if (clock_gettime(CLOCK_REALTIME, &ts) != -1)
		{
			ts.tv_sec += timeout;
		    rc = sem_timedwait(sem, &ts);
		}
	#endif

 	FUNC_EXIT_RC(rc);
 	return rc;
}


int Thread_check_sem(sem_type sem)
{
#if defined(WIN32)
	return WaitForSingleObject(sem, 0) == WAIT_OBJECT_0;
#else
	int semval = -1;
	sem_getvalue(sem, &semval);
	return semval > 0;
#endif
}


/**
 * Lock a mutex which has already been created, block until ready
 * @param mutex the mutex
 * @return completion code
 */
int Thread_post_sem(sem_type sem)
{
	int rc = 0;

	FUNC_ENTRY;
	#if defined(WIN32)
		if (SetEvent(sem) == 0)
			rc = GetLastError();
	#else
		if (sem_post(sem) == -1)
			rc = errno;
	#endif

 	FUNC_EXIT_RC(rc);
  return rc;
}


/**
 * Destroy a semaphore which has already been created
 * @param sem the semaphore
 */
int Thread_destroy_sem(sem_type sem)
{
	int rc = 0;

	FUNC_ENTRY;
	#if defined(WIN32)
		rc = CloseHandle(sem);
	#else
		rc = sem_destroy(sem);
		free(sem);
	#endif
	FUNC_EXIT_RC(rc);
	return rc;
}



#if defined(THREAD_UNIT_TESTS)

#include <stdio.h>

thread_return_type secondary(void* n)
{
	int rc = 0;

	/*
	cond_type cond = n;

	printf("Secondary thread about to wait\n");
	rc = Thread_wait_cond(cond);
	printf("Secondary thread returned from wait %d\n", rc);*/

	sem_type sem = n;

	printf("Secondary thread about to wait\n");
	rc = Thread_wait_sem(sem);
	printf("Secondary thread returned from wait %d\n", rc);

	printf("Secondary thread about to wait\n");
	rc = Thread_wait_sem(sem);
	printf("Secondary thread returned from wait %d\n", rc);
	printf("Secondary check sem %d\n", Thread_check_sem(sem));

	return 0;
}


int main(int argc, char *argv[])
{
	int rc = 0;

	sem_type sem = Thread_create_sem();

	printf("check sem %d\n", Thread_check_sem(sem));

	printf("post secondary\n");
	rc = Thread_post_sem(sem);
	printf("posted secondary %d\n", rc);

	printf("check sem %d\n", Thread_check_sem(sem));

	printf("Starting secondary thread\n");
	Thread_start(secondary, (void*)sem);

	sleep(3);
	printf("check sem %d\n", Thread_check_sem(sem));

	printf("post secondary\n");
	rc = Thread_post_sem(sem);
	printf("posted secondary %d\n", rc);

	sleep(3);

	printf("Main thread ending\n");
}

#endif
