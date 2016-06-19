import paho_mqtt3a as mqttv3, time, random
import contextlib

print dir(mqttv3)

hostname = "localhost"
clientid = "myclientid"
topic = "test2_topic"

def deliveryComplete(context, msgid):
	print "deliveryComplete", msgid

def connectionLost(context, cause):
	print "connectionLost"
	print "rc from reconnect is", mqttv3.connect(self.client)

def messageArrived(context, topicName, message):
	print "messageArrived", message
	#print "clientid", context
	#print "topicName", topicName
	return 1

def onSuccess(context, successData):
	print "onSuccess for", context["clientid"], context["state"], successData
	responseOptions = {"context": context, "onSuccess": onSuccess, "onFailure" : onFailure}
	#responseOptions = {"context": context}
	if context["state"] == "connecting":
		context["state"] = "subscribing"
		print "rc from subscribe is", mqttv3.subscribe(client, topic, 2, responseOptions)
	elif context["state"] == "subscribing":
		context["state"] = "publishing qos 0"
		print "rc from publish is", mqttv3.send(client, topic, "a QoS 0 message", 0, 0, responseOptions)
	elif context["state"] == "publishing qos 0":
		context["state"] = "publishing qos 1"
		print "rc from publish is", mqttv3.send(client, topic, "a QoS 1 message", 1, 0, responseOptions)
	elif context["state"] == "publishing qos 1":
		context["state"] = "publishing qos 2"
		print "rc from publish is", mqttv3.send(client, topic, "a QoS 2 message", 2, 0, responseOptions)
	elif context["state"] == "publishing qos 2":
		context["state"] = "finished"
	print "leaving onSuccess"
	
def onFailure(context, failureData):
	print "onFailure for", context["clientid"]
	context["state"] = "finished"

noclients = 1
myclientid = None
clients = []
for i in range(noclients):
	myclientid = clientid+str(i)
	rc, client = mqttv3.create("tcp://"+hostname+":1883", myclientid)
	#print "client is", hex(client)
	print "rc from create is", rc
	print "rc from setcallbacks is", mqttv3.setcallbacks(client, client, connectionLost, messageArrived, deliveryComplete)
	
	context = {"client" : client, "clientid" : clientid, "state" : "connecting"}
	
	print "rc from connect is", mqttv3.connect(client, {"context": context, "onSuccess": onSuccess, "onFailure": onFailure})
	
	clients.append(context)

while [x for x in clients if x["state"] != "finished"]:
	print [x for x in clients if x["state"] != "finished"]
	time.sleep(1)

for client in clients:
	if mqttv3.isConnected(client["client"]):
		print "rc from disconnect is", mqttv3.disconnect(client["client"], 1000)
	time.sleep(1)
	mqttv3.destroy(client["client"])
	print "after destroy"

