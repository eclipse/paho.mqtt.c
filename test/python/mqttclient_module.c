#include <Python.h>

#include "MQTTClient.h"
#include "LinkedList.h"

static PyObject *MqttV3Error;

static PyObject* mqttv3_create(PyObject* self, PyObject *args)
{
	MQTTClient c;
	char* serverURI;
	char* clientId;
	int persistence_option = MQTTCLIENT_PERSISTENCE_DEFAULT;
	int rc;

	if (!PyArg_ParseTuple(args, "ss|i", &serverURI, &clientId,
			&persistence_option))
		return NULL;

	if (persistence_option != MQTTCLIENT_PERSISTENCE_DEFAULT
			&& persistence_option != MQTTCLIENT_PERSISTENCE_NONE)
	{
		PyErr_SetString(PyExc_TypeError, "persistence must be DEFAULT or NONE");
		return NULL;
	}

	rc = MQTTClient_create(&c, serverURI, clientId, persistence_option, NULL);

	printf("create MQTTClient pointer %p\n", c);

	return Py_BuildValue("ik", rc, c);
}

static List* callbacks = NULL;

typedef struct
{
	MQTTClient c;
	PyObject *context;
	PyObject *cl, *ma, *dc;
} CallbackEntry;

int clientCompare(void* a, void* b)
{
	CallbackEntry* e = (CallbackEntry*) a;
	return e->c == (MQTTClient) b;
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
		MQTTClient_message* message)
{
	PyObject *result = NULL;
	CallbackEntry* e = context;
	int rc = -99;
	PyGILState_STATE gstate;

	//printf("messageArrived %s %s %d %.*s\n", PyString_AsString(e->context), topicName, topicLen,
	//	message->payloadlen, (char*)message->payload);

	gstate = PyGILState_Ensure();
	if (topicLen == 0)
		result = PyEval_CallFunction(e->ma, "Os{ss#sisisisi}", e->context,
				topicName, "payload", message->payload, message->payloadlen,
				"qos", message->qos, "retained", message->retained, "dup",
				message->dup, "msgid", message->msgid);
	else
		result = PyEval_CallFunction(e->ma, "Os#{ss#sisisisi}", e->context,
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
	MQTTClient_free(topicName);
	MQTTClient_freeMessage(&message);
	return rc;
}

void deliveryComplete(void* context, MQTTClient_deliveryToken dt)
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
	MQTTClient c;
	CallbackEntry* e = NULL;
	int rc;

	e = malloc(sizeof(CallbackEntry));

	if (!PyArg_ParseTuple(args, "kOOOO", &c, (PyObject**) &e->context, &e->cl,
			&e->ma, &e->dc))
		return NULL;
	e->c = c;

	printf("setCallbacks MQTTClient pointer %p\n", c);

	if ((e->cl != Py_None && !PyCallable_Check(e->cl))
			|| (e->ma != Py_None && !PyCallable_Check(e->ma))
			|| (e->dc != Py_None && !PyCallable_Check(e->dc)))
	{
		PyErr_SetString(PyExc_TypeError,
				"3rd, 4th and 5th parameters must be callable or None");
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_setCallbacks(c, e, connectionLost,
			messageArrived, deliveryComplete);
	Py_END_ALLOW_THREADS

	if (rc == MQTTCLIENT_SUCCESS)
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

static PyObject* mqttv3_connect(PyObject* self, PyObject *args)
{
	MQTTClient c;
	PyObject *pyoptions = NULL, *temp;
	MQTTClient_connectOptions options = MQTTClient_connectOptions_initializer;
	MQTTClient_willOptions woptions = MQTTClient_willOptions_initializer;
	int rc;

	if (!PyArg_ParseTuple(args, "k|O", &c, &pyoptions))
		return NULL;

	printf("connect MQTTClient pointer %p\n", c);
	if (!pyoptions)
		goto skip;

	if (!PyDict_Check(pyoptions))
	{
		PyErr_SetString(PyExc_TypeError, "2nd parameter must be a dictionary");
		return NULL;
	}

	if ((temp = PyDict_GetItemString(pyoptions, "keepAliveInterval")) != NULL)
	{
		if (PyInt_Check(temp))
			options.keepAliveInterval = (int) PyInt_AsLong(temp);
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
			options.cleansession = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "cleansession value must be int");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "reliable")) != NULL)
	{
		if (PyInt_Check(temp))
			options.reliable = (int) PyInt_AsLong(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "reliable value must be int");
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
					woptions.topicName = PyString_AsString(wtemp);
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
					woptions.message = PyString_AsString(wtemp);
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
					woptions.retained = (int) PyInt_AsLong(wtemp);
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
					woptions.qos = (int) PyInt_AsLong(wtemp);
				else
				{
					PyErr_SetString(PyExc_TypeError,
							"will qos value must be int");
					return NULL;
				}
			}
			options.will = &woptions;
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
			options.username = PyString_AsString(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "username value must be string");
			return NULL;
		}
	}

	if ((temp = PyDict_GetItemString(pyoptions, "password")) != NULL)
	{
		if (PyString_Check(temp))
			options.username = PyString_AsString(temp);
		else
		{
			PyErr_SetString(PyExc_TypeError, "password value must be string");
			return NULL;
		}
	}

	skip: Py_BEGIN_ALLOW_THREADS rc = MQTTClient_connect(c, &options);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_disconnect(PyObject* self, PyObject *args)
{
	MQTTClient c;
	int timeout = 0;
	int rc;

	if (!PyArg_ParseTuple(args, "k|i", &c, &timeout))
		return NULL;
	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_disconnect(c, timeout);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_isConnected(PyObject* self, PyObject *args)
{
	MQTTClient c;
	int rc;

	if (!PyArg_ParseTuple(args, "k", &c))
		return NULL;
	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_isConnected(c);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_subscribe(PyObject* self, PyObject *args)
{
	MQTTClient c;
	char* topic;
	int qos = 2;
	int rc;

	if (!PyArg_ParseTuple(args, "ks|i", &c, &topic, &qos))
		return NULL;
	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_subscribe(c, topic, qos);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_subscribeMany(PyObject* self, PyObject *args)
{
	MQTTClient c;
	PyObject* topicList;
	PyObject* qosList;

	int count;
	char** topics;
	int* qoss;

	int i, rc = 0;

	if (!PyArg_ParseTuple(args, "kOO", &c, &topicList, &qosList))
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

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_subscribeMany(c, count, topics,
			qoss);
	Py_END_ALLOW_THREADS

	for (i = 0; i < count; ++i)
		PySequence_SetItem(qosList, i, PyInt_FromLong((long) qoss[i]));

	free(topics);
	free(qoss);

	if (rc == MQTTCLIENT_SUCCESS)
		return Py_BuildValue("iO", rc, qosList);
	else
		return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_unsubscribe(PyObject* self, PyObject *args)
{
	MQTTClient c;
	char* topic;
	int rc;

	if (!PyArg_ParseTuple(args, "ks", &c, &topic))
		return NULL;
	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_unsubscribe(c, topic);
	Py_END_ALLOW_THREADS
	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_unsubscribeMany(PyObject* self, PyObject *args)
{
	MQTTClient c;
	PyObject* topicList;

	int count;
	char** topics;

	int i, rc = 0;

	if (!PyArg_ParseTuple(args, "kOO", &c, &topicList))
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

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_unsubscribeMany(c, count, topics);
	Py_END_ALLOW_THREADS

	free( topics);

	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_publish(PyObject* self, PyObject *args)
{
	MQTTClient c;
	char* topicName;
	int payloadlen;
	void* payload;
	int qos = 0;
	int retained = 0;
	MQTTClient_deliveryToken dt;
	int rc;

	if (!PyArg_ParseTuple(args, "kss#|ii", &c, &topicName, &payload,
			&payloadlen, &qos, &retained))
		return NULL;

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_publish(c, topicName, payloadlen,
			payload, qos, retained, &dt);
	Py_END_ALLOW_THREADS

	if (rc == MQTTCLIENT_SUCCESS && qos > 0)
		return Py_BuildValue("ii", rc, dt);
	else
		return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_publishMessage(PyObject* self, PyObject *args)
{
	MQTTClient c;
	char* topicName;
	PyObject *message, *temp;
	MQTTClient_message msg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken dt;
	int rc;

	if (!PyArg_ParseTuple(args, "ksO", &c, &topicName, &message))
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

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_publishMessage(c, topicName, &msg,
			&dt);
	Py_END_ALLOW_THREADS

	if (rc == MQTTCLIENT_SUCCESS && msg.qos > 0)
		return Py_BuildValue("ii", rc, dt);
	else
		return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_waitForCompletion(PyObject* self, PyObject *args)
{
	MQTTClient c;
	unsigned long timeout = 1000L;
	MQTTClient_deliveryToken dt;
	int rc;

	if (!PyArg_ParseTuple(args, "ki|i", &c, &dt, &timeout))
		return NULL;

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_waitForCompletion(c, dt, timeout);
	Py_END_ALLOW_THREADS

	return Py_BuildValue("i", rc);
}

static PyObject* mqttv3_getPendingDeliveryTokens(PyObject* self, PyObject *args)
{
	MQTTClient c;
	MQTTClient_deliveryToken* tokens;
	int rc;

	if (!PyArg_ParseTuple(args, "k", &c))
		return NULL;

	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_getPendingDeliveryTokens(c, &tokens);
	Py_END_ALLOW_THREADS

	if (rc == MQTTCLIENT_SUCCESS)
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

static PyObject* mqttv3_yield(PyObject* self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	MQTTClient_yield();
	Py_END_ALLOW_THREADS

	Py_INCREF( Py_None);
	return Py_None;
}

static PyObject* mqttv3_receive(PyObject* self, PyObject *args)
{
	MQTTClient c;
	unsigned long timeout = 1000L;
	int rc;
	PyObject* temp = NULL;

	char* topicName;
	int topicLen;
	MQTTClient_message* message;

	if (!PyArg_ParseTuple(args, "k|k", &c, &timeout))
		return NULL;
	Py_BEGIN_ALLOW_THREADS rc = MQTTClient_receive(c, &topicName, &topicLen,
			&message, timeout);
	Py_END_ALLOW_THREADS
	if (message)
	{
		temp = Py_BuildValue("is#{ss#sisisisi}", rc, topicName, topicLen,
				"payload", message->payload, message->payloadlen, "qos",
				message->qos, "retained", message->retained, "dup",
				message->dup, "msgid", message->msgid);
		free(topicName);
		MQTTClient_freeMessage(&message);
	}
	else
		temp = Py_BuildValue("iz", rc, NULL);

	return temp;
}

static PyObject* mqttv3_destroy(PyObject* self, PyObject *args)
{
	MQTTClient c;
	ListElement* temp = NULL;

	if (!PyArg_ParseTuple(args, "k", &c))
		return NULL;

	if ((temp = ListFindItem(callbacks, c, clientCompare)) != NULL)
	{
		ListDetach(callbacks, temp->content);
		free(temp->content);
	}

	MQTTClient_destroy(&c);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef MqttV3Methods[] =
		{
		{ "create", mqttv3_create, METH_VARARGS, "Create an MQTTv3 client." },
		{ "setcallbacks", mqttv3_setcallbacks, METH_VARARGS,
				"Sets the callback functions for a particular client." },
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
				{ "publish", mqttv3_publish, METH_VARARGS,
						"Publish a message to the given topic." },
				{ "publishMessage", mqttv3_publishMessage, METH_VARARGS,
						"Publish a message to the given topic." },
				{ "waitForCompletion", mqttv3_waitForCompletion, METH_VARARGS,
						"Waits for the completion of the delivery of the message represented by a delivery token." },
				{ "getPendingDeliveryTokens", mqttv3_getPendingDeliveryTokens,
						METH_VARARGS,
						"Returns the delivery tokens pending of completion." },
				{ "yield", mqttv3_yield, METH_VARARGS,
						"Single-thread keep alive but don't receive message." },
				{ "receive", mqttv3_receive, METH_VARARGS,
						"Single-thread receive message if available." },
				{ "destroy", mqttv3_destroy, METH_VARARGS,
						"Free memory allocated to a MQTT client. It is the opposite to create." },
				{ NULL, NULL, 0, NULL } /* Sentinel */
		};

PyMODINIT_FUNC initpaho_mqtt3c(void)
{
	PyObject *m;

	PyEval_InitThreads();

	callbacks = ListInitialize();

	m = Py_InitModule("paho_mqtt3c", MqttV3Methods);
	if (m == NULL)
		return;

	MqttV3Error = PyErr_NewException("paho_mqtt3c.error", NULL, NULL);
	Py_INCREF(MqttV3Error);
	PyModule_AddObject(m, "error", MqttV3Error);

	PyModule_AddIntConstant(m, "SUCCESS", MQTTCLIENT_SUCCESS);
	PyModule_AddIntConstant(m, "FAILURE", MQTTCLIENT_FAILURE);
	PyModule_AddIntConstant(m, "DISCONNECTED", MQTTCLIENT_DISCONNECTED);
	PyModule_AddIntConstant(m, "MAX_MESSAGES_INFLIGHT",	MQTTCLIENT_MAX_MESSAGES_INFLIGHT);
	PyModule_AddIntConstant(m, "BAD_UTF8_STRING", MQTTCLIENT_BAD_UTF8_STRING);
	PyModule_AddIntConstant(m, "BAD_NULL_PARAMETER", MQTTCLIENT_NULL_PARAMETER);
	PyModule_AddIntConstant(m, "BAD_TOPICNAME_TRUNCATED", MQTTCLIENT_TOPICNAME_TRUNCATED);
	PyModule_AddIntConstant(m, "PERSISTENCE_DEFAULT", MQTTCLIENT_PERSISTENCE_DEFAULT);
	PyModule_AddIntConstant(m, "PERSISTENCE_NONE", MQTTCLIENT_PERSISTENCE_NONE);
	PyModule_AddIntConstant(m, "PERSISTENCE_USER", MQTTCLIENT_PERSISTENCE_USER);
	PyModule_AddIntConstant(m, "PERSISTENCE_ERROR",
	MQTTCLIENT_PERSISTENCE_ERROR);
}
