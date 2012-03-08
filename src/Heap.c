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
 * \brief functions to manage the heap with the goal of eliminating memory leaks
 *
 * For any module to use these functions transparently, simply include the Heap.h
 * header file.  Malloc and free will be redefined, but will behave in exactly the same
 * way as normal, so no recoding is necessary.
 *
 * */

#include "LinkedList.h"
#include "Log.h"
#include "StackTrace.h"
#include "Thread.h"

#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Heap.h"

#undef malloc
#undef realloc
#undef free

static heap_info state = {0, 0};

int ListRemoveCurrentItem(List* aList);

typedef struct
{
	char* file;
	int line;
	void* ptr;
	size_t size;
	char* stack;
} storageElement;

static List heap = {NULL, NULL, NULL};

/**
 * Allocates a block of memory.  A direct replacement for malloc, but keeps track of items
 * allocated in a list, so that free can check that a item is being freed correctly and that
 * we can check that all memory is freed at shutdown.
 * @param file use the __FILE__ macro to indicate which file this item was allocated in
 * @param line use the __LINE__ macro to indicate which line this item was allocated at
 * @param size the size of the item to be allocated
 * @return pointer to the allocated item, or NULL if there was an error
 */
void* mymalloc(char* file, int line, size_t size)
{
	ListElement* e = NULL;
	storageElement* s = NULL;
	static char* errmsg = "Memory allocation error";

	if ((s = malloc(sizeof(storageElement))) == NULL)
	{
		Log(LOG_ERROR, -1, errmsg);
		return NULL;
	}
	if ((s->file = malloc(strlen(file)+1)) == NULL)
	{
		Log(LOG_ERROR, -1, errmsg);
		free(s);
		return NULL;
	}
	strcpy(s->file, file);
	s->line = line;
	if ((s->ptr = malloc(size)) == NULL)
	{
		Log(LOG_ERROR, -1, errmsg);
		free(s->file);
		free(s);
		return NULL;
	}
	s->size = size;
	s->stack = NULL;
	if (trace_settings.trace_level == TRACE_MAXIMUM)
	{
		if ((s->stack = StackTrace_get(Thread_getid())) == NULL)
		{
			Log(LOG_ERROR, -1, errmsg);
			free(s->ptr);
			free(s->file);
			free(s);
			return NULL;
		}
		else
		{
			Log(TRACE_MAX, -1, "Allocating %d bytes in heap at line %d of file %s, ptr %p, heap use now %d bytes",
													 s->size, line, file, s->ptr, state.current_size);
			Log(TRACE_MAX, -1, "Stack trace is %s", s->stack);
		}
	}
	if ((e = malloc(sizeof(ListElement))) == NULL)
	{
		Log(LOG_ERROR, -1, errmsg);
		if (s->stack)
			free(s->stack);
		free(s->ptr);
		free(s->file);
		free(s);
		return NULL;
	}
	ListAppendNoMalloc(&heap, s, e, sizeof(ListElement));
	state.current_size += size;
	if (state.current_size > state.max_size)
		state.max_size = state.current_size;
	return s->ptr;
}


/**
 * List callback function for comparing storage elements
 * @param a first integer value
 * @param b second integer value
 * @return boolean indicating whether a and b are equal
 */
int ptrCompare(void* a, void* b)
{
	storageElement* s = (storageElement*)a;
	return s->ptr == b;
}


/**
 * Utility to find an item in the heap.  Lets you know if the heap already contains
 * the memory location in question.
 * @param p pointer to a memory location
 * @return pointer to the storage element if found, or NULL
 */
void* Heap_findItem(void* p)
{
	ListElement* e = ListFindItem(&heap, p, ptrCompare);
	return (e == NULL) ? NULL : e->content;
}


/**
 * Remove an item from the recorded heap without actually freeing it.
 * Use sparingly!
 * @param file use the __FILE__ macro to indicate which file this item was allocated in
 * @param line use the __LINE__ macro to indicate which line this item was allocated at
 * @param p pointer to the item to be removed
 */
void Internal_heap_unlink(char* file, int line, void* p)
{
	ListElement* e = NULL;

	if ((e = ListFindItem(&heap, p, ptrCompare)) == NULL)
		Log(LOG_ERROR, -1, "Failed to remove heap item at file %s line %d", file, line);
	else
	{
		storageElement* s = (storageElement*)(heap.current->content);
		Log(TRACE_MAX, -1, "Freeing %d bytes in heap at file %s line %d, heap use now %d bytes",
											 s->size, file, line, state.current_size);
		free(s->file);
		if (s->stack)
			free(s->stack);
		state.current_size -= s->size;
		ListRemoveCurrentItem(&heap);
	}
}


/**
 * Remove an item from the recorded heap without actually freeing it.
 * Use sparingly!
 * @param file use the __FILE__ macro to indicate which file this item was allocated in
 * @param line use the __LINE__ macro to indicate which line this item was allocated at
 * @param p pointer to the item to be removed
 */
void Heap_unlink(char* file, int line, void* p)
{
	Internal_heap_unlink(file, line, p);
}


/**
 * Frees a block of memory.  A direct replacement for free, but checks that a item is in
 * the allocates list first.
 * @param file use the __FILE__ macro to indicate which file this item was allocated in
 * @param line use the __LINE__ macro to indicate which line this item was allocated at
 * @param p pointer to the item to be freed
 */
void myfree(char* file, int line, void* p)
{
	Internal_heap_unlink(file, line, p);
	free(p);
}


/**
 * Reallocates a block of memory.  A direct replacement for realloc, but keeps track of items
 * allocated in a list, so that free can check that a item is being freed correctly and that
 * we can check that all memory is freed at shutdown.
 * @param file use the __FILE__ macro to indicate which file this item was reallocated in
 * @param line use the __LINE__ macro to indicate which line this item was reallocated at
 * @param p pointer to the item to be reallocated
 * @param size the new size of the item
 * @return pointer to the allocated item, or NULL if there was an error
 */
void *myrealloc(char* file, int line, void* p, size_t size)
{
	void* rc = NULL;

	ListElement* e = ListFindItem(&heap, p, ptrCompare);
	if (e == NULL)
		Log(LOG_ERROR, -1, "Failed to reallocate heap item at file %s line %d", file, line);
	else
	{
		storageElement* s = (storageElement*)(heap.current->content);
		state.current_size += size - s->size;
		if (state.current_size > state.max_size)
			state.max_size = state.current_size;
		rc = s->ptr = realloc(s->ptr, size);
		s->size = size;
		s->file = realloc(s->file, strlen(file)+1);
		strcpy(s->file, file);
		s->line = line;
		if (s->stack)
		{
			free(s->stack);
			s->stack = StackTrace_get(Thread_getid());
		}
	}
	return rc;
}


/**
 * Removes and frees the current item in a list.  A list function only used by myfree.
 * @param aList the list from which the item is to be removed
 * @return 1=item removed, 0=item not removed
 */
int ListRemoveCurrentItem(List* aList)
{
	ListElement* next = NULL;

	if (aList->current->prev == NULL)
		/* so this is the first element, and we have to update the "first" pointer */
		aList->first = aList->current->next;
	else
		aList->current->prev->next = aList->current->next;

	if (aList->current->next == NULL)
		aList->last = aList->current->prev;
	else
		aList->current->next->prev = aList->current->prev;

	next = aList->current->next;
	free(aList->current->content);
	free(aList->current);
	aList->current = next;
	return 1; /* successfully removed item */
}


/**
 * Scans the heap and reports any items currently allocated.
 * To be used at shutdown if any heap items have not been freed.
 */
void HeapScan()
{
	ListElement* current = NULL;
	Log(TRACE_MIN, -1, "Heap scan start, total %d bytes", state.current_size);
	while (ListNextElement(&heap, &current))
	{
		storageElement* s = (storageElement*)(current->content);
		Log(TRACE_MIN, -1, "Heap element size %d, line %d, file %s, ptr %p", s->size, s->line, s->file, s->ptr);
		Log(TRACE_MIN, -1, "  Content %*.s", (10 > s->size) ? s->size : 10, (char*)s->ptr);
		if (s->stack)
			Log(TRACE_MIN, -1, "  Stack trace: %s", s->stack);
	}
	Log(TRACE_MIN, -1, "Heap scan end");
}


/**
 * Heap initialization.
 */
int Heap_initialize()
{
	return 0;
}


/**
 * Heap termination.
 */
void Heap_terminate()
{
	Log(TRACE_MIN, -1, "Maximum heap use was %d bytes", state.max_size);
	if (state.current_size > 20) /* One log list is freed after this function is called */
	{
		Log(LOG_ERROR, -1, "Some memory not freed at shutdown, possible memory leak");
		HeapScan();
	}
}


/**
 * Access to heap state
 * @return pointer to the heap state structure
 */
heap_info* Heap_get_info()
{
	return &state;
}


/**
 * Dump a string from the heap so that it can be displayed conveniently
 * @param file file handle to dump the heap contents to
 * @param str the string to dump
 */
int HeapDumpString(FILE* file, char* str)
{
	int rc = 0;

	if (str)
	{
		int* i = (int*)str;
		int j;
		int len = strlen(str);

		if (fwrite(&(str), sizeof(int), 1, file) != 1)
			rc = -1;
		if (fwrite(&(len), sizeof(int), 1 ,file) != 1)
			rc = -1;
		for (j = 0; j < len; j++)
		{
			if (fwrite(i, sizeof(int), 1, file) != 1)
				rc = -1;
			i++;
		}
	}
	return rc;
}


/**
 * Dump the state of the heap
 * @param file file handle to dump the heap contents to
 * @return completion code, success == 0
 */
int HeapDump(FILE* file)
{
	int rc = 0;

	if (file != NULL)
	{
		ListElement* current = NULL;
		while (ListNextElement(&heap, &current))
		{
			storageElement* s = (storageElement*)(current->content);
			int* i = s->ptr;
			unsigned int j;

			if (fwrite(&(s->ptr), sizeof(int), 1, file) != 1)
				rc = -1;
			if (fwrite(&(s->size), sizeof(int), 1, file) != 1)
				rc = -1;
			for (j = 0; j < s->size; j++)
			{
				if (fwrite(i, sizeof(int), 1, file) != -1)
					rc = -1;
				i++;
			}
		}
	}
	return rc;
}
