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
 *    Ian Craggs - initial implementation
 *    Ian Craggs, Allan Stockdill-Mander - async client updates
 *    Ian Craggs - fix for bug #420851
 *    Ian Craggs - change MacOS semaphore implementation
 *******************************************************************************/
#include "MQTTClient.h"

#if !defined(THREAD_H)
#define THREAD_H

#include "mutex_type.h" /* Needed for mutex_type */

#if defined(WIN32) || defined(WIN64)
	#include <windows.h>
	#define thread_type HANDLE
	#define thread_id_type DWORD
	#define thread_return_type DWORD
	#define thread_fn LPTHREAD_START_ROUTINE
	#define cond_type HANDLE
	#define sem_type HANDLE
	#undef ETIMEDOUT
	#define ETIMEDOUT WSAETIMEDOUT
#else
	#include <pthread.h>

	#define thread_type pthread_t
	#define thread_id_type pthread_t
	#define thread_return_type void*
	typedef thread_return_type (*thread_fn)(void*);
	typedef struct { pthread_cond_t cond; pthread_mutex_t mutex; } cond_type_struct;
	typedef cond_type_struct *cond_type;
	#if defined(OSX)
	  #include <dispatch/dispatch.h>
	  typedef dispatch_semaphore_t sem_type;
	#else
	  #include <semaphore.h>
	  typedef sem_t *sem_type;
	#endif

	cond_type Thread_create_cond(void);
	int Thread_signal_cond(cond_type);
	int Thread_wait_cond(cond_type condvar, int timeout);
	int Thread_destroy_cond(cond_type);
#endif

DLLExport thread_type Thread_start(thread_fn, void*);

DLLExport mutex_type Thread_create_mutex();
DLLExport int Thread_lock_mutex(mutex_type);
DLLExport int Thread_unlock_mutex(mutex_type);
void Thread_destroy_mutex(mutex_type);

DLLExport thread_id_type Thread_getid();

sem_type Thread_create_sem(void);
int Thread_wait_sem(sem_type sem, int timeout);
int Thread_check_sem(sem_type sem);
int Thread_post_sem(sem_type sem);
int Thread_destroy_sem(sem_type sem);


#endif
