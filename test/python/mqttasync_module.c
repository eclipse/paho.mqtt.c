#include <Python.h>

#include "MQTTAsync.h"
#include "LinkedList.h"

static PyObject *MqttV3Error;


static PyObject* mqttv3_create(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	char* serverURI;
	char* clientId;
	int persistence_option = MQTTCLIENT_PERSISTENCE_DEFAULT;
	PyObject *pyoptions = NULL, *temp;
	MQTTAsync_createOptions options = MQTTAsync_createOptions_initializer;
	int rc;

	if (!PyArg_ParseTuple(args, "ss|iO", &serverURI, &clientId,
			&persistence_option, &pyoptions))
		return NULL;

	if (persistence_option != MQTTCLIENT_PERSISTENCE_DEFAULT
			&& persistence_option != MQTTCLIENT_PERSISTENCE_NONE)
	{
		PyErr_SetString(PyExc_TypeError, "persistence must be DEFAULT or NONE");
		return NULL;
	}

	if (pyoptions)
	{

		if (!PyDict_Check(pyoptions))
		{
			PyErr_SetString(PyExc_TypeError,
					"Create options parameter must be a dictionary");
			return NULL;
		}

		if ((temp = PyDict_GetItemString(pyoptions, "sendWhileDisconnected"))
				!= NULL)
		{
			if (PyInt_Check(temp))
				options.sendWhileDisconnected = (int) PyInt_AsLong(temp);
			else
			{
				PyErr_SetString(PyExc_TypeError, "sendWhileDisconnected value must be int");
				return NULL;
			}
		}

		if ((temp = PyDict_GetItemString(pyoptions, "maxBufferedMessages"))
				!= NULL)
		{
			if (PyInt_Check(temp))
				options.maxBufferedMessages = (int) PyInt_AsLong(temp);
			else
			{
				PyErr_SetString(PyExc_TypeError, "maxBufferedMessages value must be int");
				return NULL;
			}
		}
		rc = MQTTAsync_createWithOptions(&c, serverURI, clientId, persistence_option, NULL, &options);
	}
	else
		rc = MQTTAsync_create(&c, serverURI, clientId, persistence_option, NULL);


	return Py_BuildValue("ik", rc, c);
}


static List* callbacks = NULL;
static List* connected_callbacks = NULL;

enum msgTypes
{
	CONNECT, PUBLISH, SUBSCRIBE, SUBSCRIBE_MANY, UNSUBSCRIBE
};

typedef struct
{
	MQTTAsync c;
	PyObject *context;
	PyObject *cl, *ma, *dc;
} CallbackEntry;


typedef struct
{
	MQTTAsync c;
	PyObject *context;
	PyObject *co;
} ConnectedEntry;


int clientCompare(void* a, void* b)
{
	CallbackEntry* e = (CallbackEntry*) a;
	return e->c == (MQTTAsync) b;
}


int connectedCompare(void* a, void* b)
{
	ConnectedEntry* e = (ConnectedEntry*) a;
	return e->c == (MQTTAsync) b;
}


void connected(void* context, char* cause)
{
	/* call the right Python function, using the context */
	PyObject *arglist;
	PyObject *result;
	ConnectedEntry* e = context;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();
	arglist = Py_BuildValue("Os", e->context, cause);
	result = PyEval_CallObject(e->co, arglist);
	Py_DECREF(arglist);
	PyGILState_Release(gstate);
}


void connectionLost(void* context, char* cause)
{
	/* call the right Python function, using the context */
	PyObject *arglist;
	PyObject *result;
	CallbackEntry* e = context;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();
	arglist = Py_BuildValue("Os", e->context, cause);
	result = PyEval_CallObject(e->cl, arglist);
	Py_DECREF(arglist);
	PyGILState_Release(gstate);
}


int messageArrived(void* context, char* topicName, int topicLen,
		MQTTAsync_message* message)
{
	PyObject *result = NULL;
	CallbackEntry* e = context;
	int rc = -99;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();
	if (topicLen == 0)
		result = PyObject_CallFunction(e->ma, "Os{ss#sisisisi}", e->context,
				topicName, "payload", message->payload, message->payloadlen,
				"qos", message->qos, "retained", message->retained, "dup",
				message->dup, "msgid", message->msgid);
	else
		result = PyObject_CallFunction(e->ma, "Os#{ss#sisisisi}", e->context,
				topicName, topicLen, "payload", message->payload,
				message->payloadlen, "qos", message->qos, "retained",
				message->retained, "dup", message->dup, "msgid",
				message->msgid);
	if (result)
	{
		if (PyInt_Check(result))
			rc = (int) PyInt_AsLong(result);
		Py_DECREF(result);
	}
	PyGILState_Release(gstate);
	MQTTAsync_free(topicName);
	MQTTAsync_freeMessage(&message);
	return rc;
}


void deliveryComplete(void* context, MQTTAsync_token dt)
{
	PyObject *arglist;
	PyObject *result;
	CallbackEntry* e = context;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();
	arglist = Py_BuildValue("Oi", e->context, dt);
	result = PyEval_CallObject(e->dc, arglist);
	Py_DECREF(arglist);
	PyGILState_Release(gstate);
}


static PyObject* mqttv3_setcallbacks(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	CallbackEntry* e = NULL;
	int rc;

	e = malloc(sizeof(CallbackEntry));

	if (!PyArg_ParseTuple(args, "kOOOO", &c, (PyObject**) &e->context, &e->cl,
			&e->ma, &e->dc))
		return NULL;
	e->c = c;

	if ((e->cl != Py_None && !PyCallable_Check(e->cl))
			|| (e->ma != Py_None && !PyCallable_Check(e->ma))
			|| (e->dc != Py_None && !PyCallable_Check(e->dc)))
	{
		PyErr_SetString(PyExc_TypeError,
				"3rd, 4th and 5th parameters must be callable or None");
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_setCallbacks(c, e, connectionLost, messageArrived, deliveryComplete);
	Py_END_ALLOW_THREADS

	if (rc == MQTTASYNC_SUCCESS)
	{
		ListElement* temp = NULL;
		if ((temp = ListFindItem(callbacks, c, clientCompare)) != NULL)
		{
			ListDetach(callbacks, temp->content);
			free(temp->content);
		}
		ListAppend(callbacks, e, sizeof(e));
		Py_XINCREF(e->cl);
		Py_XINCREF(e->ma);
		Py_XINCREF(e->dc);
		Py_XINCREF(e->context);
	}

	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_setconnected(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	ConnectedEntry* e = NULL;
	int rc;

	e = malloc(sizeof(ConnectedEntry));

	if (!PyArg_ParseTuple(args, "kOO", &c, (PyObject**) &e->context, &e->co))
		return NULL;
	e->c = c;

	if (e->co != Py_None && !PyCallable_Check(e->co))
	{
		PyErr_SetString(PyExc_TypeError,
				"3rd parameter must be callable or None");
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_setConnected(c, e, connected);
	Py_END_ALLOW_THREADS

	if (rc == MQTTASYNC_SUCCESS)
	{
		ListElement* temp = NULL;
		if ((temp = ListFindItem(connected_callbacks, c, connectedCompare)) != NULL)
		{
			ListDetach(connected_callbacks, temp->content);
			free(temp->content);
		}
		ListAppend(connected_callbacks, e, sizeof(e));
		Py_XINCREF(e->co);
		Py_XINCREF(e->context);
	}

	return Py_BuildValue("i", rc);
}


typedef struct
{
	MQTTAsync c;
	PyObject *context;
	PyObject *onSuccess, *onFailure;
	enum msgTypes msgType;
} ResponseEntry;


void onSuccess(void* context, MQTTAsync_successData* response)
{
	PyObject *result = NULL;
	ResponseEntry* e = context;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();
	switch (e->msgType)
	{
	case CONNECT:
		result = PyObject_CallFunction(e->onSuccess, "O{sisiss}", (e->context) ? e->context : Py_None,
			"MQTTVersion", response->alt.connect.MQTTVersion,
			"sessionPresent", response->alt.connect.sessionPresent,
			"serverURI", response->alt.connect.serverURI);
		break;

	case PUBLISH:
		result = PyObject_CallFunction(e->onSuccess, "O{si ss s{ss# sisi}}", (e->context) ? e->context : Py_None,
			"token", response->token,
			"destinationName", response->alt.pub.destinationName,
			"message",
			"payload", response->alt.pub.message.payload,
			response->alt.pub.message.payloadlen,
			"qos", response->alt.pub.message.qos,
			"retained", response->alt.pub.message.retained);
		break;

	case SUBSCRIBE:
		result = PyObject_CallFunction(e->onSuccess, "O{sisi}", (e->context) ? e->context : Py_None,
			"token", response->token,
			"qos", response->alt.qos);
		break;

	case SUBSCRIBE_MANY:
		result = PyObject_CallFunction(e->onSuccess, "O{sis[i]}", (e->context) ? e->context : Py_None,
			"token", response->token,
			"qosList", response->alt.qosList[0]);
		break;

	case UNSUBSCRIBE:
		result = PyObject_CallFunction(e->onSuccess, "O{si}", (e->context) ? e->context : Py_None,
					"token", response->token);
		break;
	}
	if (result)
	{
		Py_DECREF(result);
		printf("decrementing reference count for result\n");
	}
	PyGILState_Release(gstate);
	free(e);
}


void onFailure(void* context, MQTTAsync_failureData* response)
{
	PyObject *result = NULL;
	PyObject *arglist = NULL;
	ResponseEntry* e = context;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();

	// TODO: convert response into Python structure
	if (e->context)
		arglist = Py_BuildValue("OO", e->context, response);
	else
		arglist = Py_BuildValue("OO", Py_None, response);

	result = PyEval_CallObject(e->onFailure, arglist);
	Py_DECREF(arglist);
	PyGILState_Release(gstate);
	free(e);
}


/* return true if ok, false otherwise */
int getResponseOptions(MQTTAsync c, PyObject *pyoptions, MQTTAsync_responseOptions* responseOptions,
		enum msgTypes msgType)
{
	PyObject *temp = NULL;

	if (!pyoptions)
		return 1;

	if (!PyDict_Check(pyoptions))
	{
		PyErr_SetString(PyExc_TypeError, "Response options must be a dictionary");
		return 0;
	}

	if ((temp = PyDict_GetItemString(pyoptions, "onSuccess")) != NULL)
	{
		if (PyCallable_Check(temp)) /* temp points to Python function */
			responseOptions->onSuccess = (MQTTAsync_onSuccess*)temp;
		else
		{
			PyErr_SetString(PyExc_TypeError,
					"onSuccess value must be callable");
			return 0;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "onFailure")) != NULL)
	{
		if (PyCallable_Check(temp))
			responseOptions->onFailure = (MQTTAsync_onFailure*)temp;
		else
		{
			PyErr_SetString(PyExc_TypeError,
					"onFailure value must be callable");
			return 0;
		}
	}

	responseOptions->context = PyDict_GetItemString(pyoptions, "context");

	if (responseOptions->onFailure || responseOptions->onSuccess)
	{
		ResponseEntry* r = malloc(sizeof(ResponseEntry));
		r->c = c;
		r->context = responseOptions->context;
		responseOptions->context = r;
		r->onSuccess = (PyObject*)responseOptions->onSuccess;
		responseOptions->onSuccess = onSuccess;
		r->onFailure = (PyObject*)responseOptions->onFailure;
		responseOptions->onFailure = onFailure;
		r->msgType = msgType;
	}

	return 1;  /* not an error, if we get here */
}


static PyObject* mqttv3_connect(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	PyObject *pyoptions = NULL, *temp;
	MQTTAsync_connectOptions connectOptions = MQTTAsync_connectOptions_initializer;
	MQTTAsync_willOptions willOptions = MQTTAsync_willOptions_initializer;
	int rc;

	if (!PyArg_ParseTuple(args, "k|O", &c, &pyoptions))
		return NULL;

	if (!pyoptions)
		goto skip;

	if (!PyDict_Check(pyoptions))
	{
		PyErr_SetString(PyExc_TypeError, "2nd parameter must be a dictionary");
		return NULL;
	}

	if ((temp = PyDict_GetItemString(pyoptions, "onSuccess")) != NULL)
	{
		if (PyCallable_Check(temp)) /* temp points to Python function */
			connectOptions.onSuccess = (MQTTAsync_onSuccess*)temp;
		else
		{
			PyErr_SetString(PyExc_TypeError,
					"onSuccess value must be callable");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "onFailure")) != NULL)
	{
		if (PyCallable_Check(temp))
			connectOptions.onFailure = (MQTTAsync_onFailure*)temp;
		else
		{
			PyErr_SetString(PyExc_TypeError,
					"onFailure value must be callable");
			return NULL;
		}
	}

	connectOptions.context = PyDict_GetItemString(pyoptions, "context");

	if (connectOptions.onFailure || connectOptions.onSuccess)
	{
		ResponseEntry* r = malloc(sizeof(ResponseEntry));
		r->c = c;
		r->context = connectOptions.context;
		connectOptions.context = r;
		r->onSuccess = (PyObject*)connectOptions.onSuccess;
		connectOptions.onSuccess = onSuccess;
		r->onFailure = (PyObject*)connectOptions.onFailure;
		connectOptions.onFailure = onFailure;
		r->msgType = CONNECT;
	}

	if ((temp = PyDict_GetItemString(pyoptions, "keepAliveInterval")) != NULL)
	{
		if (PyInt_Check(temp))
			connectOptions.keepAliveInterval = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError,
					"keepAliveLiveInterval value must be int");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "cleansession")) != NULL)
	{
		if (PyInt_Check(temp))
			connectOptions.cleansession = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "cleansession value must be int");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "will")) != NULL)
	{
		if (PyDict_Check(temp))
		{
			PyObject *wtemp = NULL;
			if ((wtemp = PyDict_GetItemString(temp, "topicName")) == NULL)
			{
				PyErr_SetString(PyExc_TypeError,
						"will topicName value must be set");
				return NULL;
			}
			else
			{
				if (PyString_Check(wtemp))
					willOptions.topicName = PyString_AsString(wtemp);
				else
				{
					PyErr_SetString(PyExc_TypeError,
							"will topicName value must be string");
					return NULL;
				}
			}
			if ((wtemp = PyDict_GetItemString(temp, "message")) == NULL)
			{
				PyErr_SetString(PyExc_TypeError,
						"will message value must be set");
				return NULL;
			}
			else
			{
				if (PyString_Check(wtemp))
					willOptions.message = PyString_AsString(wtemp);
				else
				{
					PyErr_SetString(PyExc_TypeError,
							"will message value must be string");
					return NULL;
				}
			}
			if ((wtemp = PyDict_GetItemString(temp, "retained")) != NULL)
			{
				if (PyInt_Check(wtemp))
					willOptions.retained = (int) PyInt_AsLong(wtemp);
				else
				{
					PyErr_SetString(PyExc_TypeError,
							"will retained value must be int");
					return NULL;
				}
			}
			if ((wtemp = PyDict_GetItemString(temp, "qos")) != NULL)
			{
				if (PyInt_Check(wtemp))
					willOptions.qos = (int) PyInt_AsLong(wtemp);
				else
				{
					PyErr_SetString(PyExc_TypeError,
							"will qos value must be int");
					return NULL;
				}
			}
			connectOptions.will = &willOptions;
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "will value must be dictionary");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "username")) != NULL)
	{
		if (PyString_Check(temp))
			connectOptions.username = PyString_AsString(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "username value must be string");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "password")) != NULL)
	{
		if (PyString_Check(temp))
			connectOptions.username = PyString_AsString(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "password value must be string");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "automaticReconnect")) != NULL)
	{
		if (PyInt_Check(temp))
			connectOptions.automaticReconnect = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "automatic reconnect value must be int");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "minRetryInterval")) != NULL)
	{
		if (PyInt_Check(temp))
			connectOptions.minRetryInterval = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "minRetryInterval value must be int");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "maxRetryInterval")) != NULL)
	{
		if (PyInt_Check(temp))
			connectOptions.maxRetryInterval = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "maxRetryInterval value must be int");
			return NULL;
		}
	}

skip:
	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_connect(c, &connectOptions);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_disconnect(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	MQTTAsync_disconnectOptions options = MQTTAsync_disconnectOptions_initializer;
	int rc;

	if (!PyArg_ParseTuple(args, "k|i", &c, &options.timeout))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_disconnect(c, &options);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_isConnected(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	int rc;

	if (!PyArg_ParseTuple(args, "k", &c))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_isConnected(c);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_subscribe(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	MQTTAsync_responseOptions response = MQTTAsync_responseOptions_initializer;
	PyObject *pyoptions = NULL;
	char* topic;
	int qos = 2;
	int rc;

	if (!PyArg_ParseTuple(args, "ks|iO", &c, &topic, &qos, &pyoptions))
		return NULL;

	if (!getResponseOptions(c, pyoptions, &response, SUBSCRIBE))
		return NULL;

	Py_BEGIN_ALLOW_THREADS;
	rc = MQTTAsync_subscribe(c, topic, qos, &response);
	Py_END_ALLOW_THREADS;
	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_subscribeMany(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	MQTTAsync_responseOptions response = MQTTAsync_responseOptions_initializer;
	PyObject* topicList;
	PyObject* qosList;
	PyObject *pyoptions = NULL;

	int count;
	char** topics;
	int* qoss;

	int i, rc = 0;

	if (!PyArg_ParseTuple(args, "kOO|O", &c, &topicList, &qosList, &pyoptions))
		return NULL;

	if (!getResponseOptions(c, pyoptions, &response, SUBSCRIBE))
		return NULL;

	if (!PySequence_Check(topicList) || !PySequence_Check(qosList))
	{
		PyErr_SetString(PyExc_TypeError,
				"3rd and 4th parameters must be sequences");
		return NULL;
	}

	if ((count = PySequence_Length(topicList)) != PySequence_Length(qosList))
	{
		PyErr_SetString(PyExc_TypeError,
				"3rd and 4th parameters must be sequences of the same length");
		return NULL;
	}

	topics = malloc(count * sizeof(char*));
	for (i = 0; i < count; ++i)
		topics[i] = PyString_AsString(PySequence_GetItem(topicList, i));

	qoss = malloc(count * sizeof(int));
	for (i = 0; i < count; ++i)
		qoss[i] = (int) PyInt_AsLong(PySequence_GetItem(qosList, i));

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_subscribeMany(c, count, topics,	qoss, &response);
	Py_END_ALLOW_THREADS

	for (i = 0; i < count; ++i)
		PySequence_SetItem(qosList, i, PyInt_FromLong((long) qoss[i]));

	free(topics);
	free(qoss);

	if (rc == MQTTASYNC_SUCCESS)
		return Py_BuildValue("iO", rc, qosList);
	else
		return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_unsubscribe(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	MQTTAsync_responseOptions response = MQTTAsync_responseOptions_initializer;
	PyObject *pyoptions = NULL;
	char* topic;
	int rc;

	if (!PyArg_ParseTuple(args, "ks|O", &c, &topic, &pyoptions))
		return NULL;

	if (!getResponseOptions(c, pyoptions, &response, UNSUBSCRIBE))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_unsubscribe(c, topic, &response);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_unsubscribeMany(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	MQTTAsync_responseOptions response = MQTTAsync_responseOptions_initializer;
	PyObject* topicList;
	PyObject *pyoptions = NULL;

	int count;
	char** topics;

	int i, rc = 0;

	if (!PyArg_ParseTuple(args, "kO|O", &c, &topicList, &pyoptions))
		return NULL;

	if (!getResponseOptions(c, pyoptions, &response, UNSUBSCRIBE))
		return NULL;

	if (!PySequence_Check(topicList))
	{
		PyErr_SetString(PyExc_TypeError, "3rd parameter must be sequences");
		return NULL;
	}

	count = PySequence_Length(topicList);
	topics = malloc(count * sizeof(char*));
	for (i = 0; i < count; ++i)
		topics[i] = PyString_AsString(PySequence_GetItem(topicList, i));

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_unsubscribeMany(c, count, topics, &response);
	Py_END_ALLOW_THREADS

	free(topics);

	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_send(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	char* destinationName;
	int payloadlen;
	void* payload;
	int qos = 0;
	int retained = 0;
	MQTTAsync_responseOptions response = MQTTAsync_responseOptions_initializer;
	PyObject *pyoptions = NULL;
	int rc;

	if (!PyArg_ParseTuple(args, "kss#|iiO", &c, &destinationName, &payload,
			&payloadlen, &qos, &retained, &pyoptions))
		return NULL;

	if (!getResponseOptions(c, pyoptions, &response, PUBLISH))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_send(c, destinationName, payloadlen, payload, qos, retained, &response);
	Py_END_ALLOW_THREADS

	if (rc == MQTTASYNC_SUCCESS && qos > 0)
		return Py_BuildValue("ii", rc, response);
	else
		return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_sendMessage(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	char* destinationName;
	PyObject *message, *temp;
	MQTTAsync_message msg = MQTTAsync_message_initializer;
	MQTTAsync_responseOptions response = MQTTAsync_responseOptions_initializer;
	PyObject *pyoptions = NULL;
	int rc;

	if (!PyArg_ParseTuple(args, "ksO|O", &c, &destinationName, &message, &pyoptions))
		return NULL;

	if (!getResponseOptions(c, pyoptions, &response, PUBLISH))
		return NULL;

	if (!PyDict_Check(message))
	{
		PyErr_SetString(PyExc_TypeError, "3rd parameter must be a dictionary");
		return NULL;
	}

	if ((temp = PyDict_GetItemString(message, "payload")) == NULL)
	{
		PyErr_SetString(PyExc_TypeError, "dictionary must have payload key");
		return NULL;
	}

	if (PyString_Check(temp))
		PyString_AsStringAndSize(temp, (char**) &msg.payload,
				(Py_ssize_t*) &msg.payloadlen);
	else
	{
		PyErr_SetString(PyExc_TypeError, "payload value must be string");
		return NULL;
	}

	if ((temp = PyDict_GetItemString(message, "qos")) == NULL)
		msg.qos = (int) PyInt_AsLong(temp);

	if ((temp = PyDict_GetItemString(message, "retained")) == NULL)
		msg.retained = (int) PyInt_AsLong(temp);

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_sendMessage(c, destinationName, &msg, &response);
	Py_END_ALLOW_THREADS

	if (rc == MQTTASYNC_SUCCESS && msg.qos > 0)
		return Py_BuildValue("ii", rc, response);
	else
		return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_waitForCompletion(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	unsigned long timeout = 1000L;
	MQTTAsync_token dt;
	int rc;

	if (!PyArg_ParseTuple(args, "ki|i", &c, &dt, &timeout))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_waitForCompletion(c, dt, timeout);
	Py_END_ALLOW_THREADS

	return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_getPendingTokens(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	MQTTAsync_token* tokens;
	int rc;

	if (!PyArg_ParseTuple(args, "k", &c))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	rc = MQTTAsync_getPendingTokens(c, &tokens);
	Py_END_ALLOW_THREADS

	if (rc == MQTTASYNC_SUCCESS)
	{
		int i = 0;
		PyObject* dts = PyList_New(0);

		while (tokens[i] != -1)
			PyList_Append(dts, PyInt_FromLong((long) tokens[i]));

		return Py_BuildValue("iO", rc, dts);
	}
	else
		return Py_BuildValue("i", rc);
}


static PyObject* mqttv3_destroy(PyObject* self, PyObject *args)
{
	MQTTAsync c;
	ListElement* temp = NULL;

	if (!PyArg_ParseTuple(args, "k", &c))
		return NULL;

	if ((temp = ListFindItem(callbacks, c, clientCompare)) != NULL)
	{
		ListDetach(callbacks, temp->content);
		free(temp->content);
	}

	if ((temp = ListFindItem(connected_callbacks, c, connectedCompare)) != NULL)
	{
		ListDetach(connected_callbacks, temp->content);
		free(temp->content);
	}

	MQTTAsync_destroy(&c);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef MqttV3Methods[] =
	{
		{ "create", mqttv3_create, METH_VARARGS, "Create an MQTTv3 client." },
		{ "setcallbacks", mqttv3_setcallbacks, METH_VARARGS,
				"Sets the callback functions for a particular client." },
		{ "setconnected", mqttv3_setconnected, METH_VARARGS,
				"Sets the connected callback function for a particular client." },
		{ "connect", mqttv3_connect, METH_VARARGS,
				"Connects to a server using the specified options." },
		{ "disconnect", mqttv3_disconnect, METH_VARARGS,
				"Disconnects from a server." },
		{ "isConnected", mqttv3_isConnected, METH_VARARGS,
				"Determines if this client is currently connected to the server." },
		{ "subscribe", mqttv3_subscribe, METH_VARARGS,
				"Subscribe to the given topic." },
		{ "subscribeMany", mqttv3_subscribeMany, METH_VARARGS,
				"Subscribe to the given topics." },
		{ "unsubscribe", mqttv3_unsubscribe, METH_VARARGS,
				"Unsubscribe from the given topic." },
		{ "unsubscribeMany", mqttv3_unsubscribeMany, METH_VARARGS,
				"Unsubscribe from the given topics." },
		{ "send", mqttv3_send, METH_VARARGS,
				"Publish a message to the given topic." },
		{ "sendMessage", mqttv3_sendMessage, METH_VARARGS,
				"Publish a message to the given topic." },
		{ "waitForCompletion", mqttv3_waitForCompletion, METH_VARARGS,
				"Waits for the completion of the delivery of the message represented by a delivery token." },
		{ "getPendingTokens", mqttv3_getPendingTokens, METH_VARARGS,
				"Returns the tokens pending of completion." },
		{ "destroy", mqttv3_destroy, METH_VARARGS,
				"Free memory allocated to a MQTT client. It is the opposite to create." },
		{ NULL, NULL, 0, NULL } /* Sentinel */
	};


PyMODINIT_FUNC initpaho_mqtt3a(void)
{
	PyObject *m;

	PyEval_InitThreads();

	callbacks = ListInitialize();
	connected_callbacks = ListInitialize();

	m = Py_InitModule("paho_mqtt3a", MqttV3Methods);
	if (m == NULL)
		return;

	MqttV3Error = PyErr_NewException("paho_mqtt3a.error", NULL, NULL);
	Py_INCREF(MqttV3Error);
	PyModule_AddObject(m, "error", MqttV3Error);

	PyModule_AddIntConstant(m, "SUCCESS", MQTTASYNC_SUCCESS);
	PyModule_AddIntConstant(m, "FAILURE", MQTTASYNC_FAILURE);
	PyModule_AddIntConstant(m, "DISCONNECTED", MQTTASYNC_DISCONNECTED);
	PyModule_AddIntConstant(m, "MAX_MESSAGES_INFLIGHT",	MQTTASYNC_MAX_MESSAGES_INFLIGHT);
	PyModule_AddIntConstant(m, "BAD_UTF8_STRING", MQTTASYNC_BAD_UTF8_STRING);
	PyModule_AddIntConstant(m, "BAD_NULL_PARAMETER", MQTTASYNC_NULL_PARAMETER);
	PyModule_AddIntConstant(m, "BAD_TOPICNAME_TRUNCATED", MQTTASYNC_TOPICNAME_TRUNCATED);
	PyModule_AddIntConstant(m, "PERSISTENCE_DEFAULT", MQTTCLIENT_PERSISTENCE_DEFAULT);
	PyModule_AddIntConstant(m, "PERSISTENCE_NONE", MQTTCLIENT_PERSISTENCE_NONE);
	PyModule_AddIntConstant(m, "PERSISTENCE_USER", MQTTCLIENT_PERSISTENCE_USER);
	PyModule_AddIntConstant(m, "PERSISTENCE_ERROR",
	MQTTCLIENT_PERSISTENCE_ERROR);
}
