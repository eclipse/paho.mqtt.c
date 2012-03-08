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
 * \brief functions which apply to client structures
 * */


#include "Clients.h"

#include <string.h>
#include <stdio.h>


/**
 * List callback function for comparing clients by clientid
 * @param a first integer value
 * @param b second integer value
 * @return boolean indicating whether a and b are equal
 */
int clientIDCompare(void* a, void* b)
{
	Clients* client = (Clients*)a;
	/*printf("comparing clientdIDs %s with %s\n", client->clientID, (char*)b);*/
	return strcmp(client->clientID, (char*)b) == 0;
}


/**
 * List callback function for comparing clients by socket
 * @param a first integer value
 * @param b second integer value
 * @return boolean indicating whether a and b are equal
 */
int clientSocketCompare(void* a, void* b)
{
	Clients* client = (Clients*)a;
	/*printf("comparing %d with %d\n", (char*)a, (char*)b); */
	return client->socket == *(int*)b;
}
