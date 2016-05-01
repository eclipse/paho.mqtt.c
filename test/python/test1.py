import paho_mqtt3c as mqttv3, time, random

print dir(mqttv3)

host = "localhost"
clientid = "myclientid"
noclients = 4

def deliveryComplete(context, msgid):
	print "deliveryComplete", msgid

def connectionLost(context, cause):
	print "connectionLost"
	print "rc from reconnect is", mqttv3.connect(self.client)

def messageArrived(context, topicName, message):
	print "clientid", context
	print "topicName", topicName
	print "message", message
	return 1

print messageArrived

myclientid = None
clients = []
for i in range(noclients):
	myclientid = clientid+str(i)
	rc, client = mqttv3.create("tcp://"+host+":1883", myclientid)
	print "client is", hex(client)
	print "rc from create is", rc
	print "rc from setcallbacks is", mqttv3.setcallbacks(client, client, connectionLost, messageArrived, deliveryComplete)
	print "client is", hex(client)
	print "rc from connect is", mqttv3.connect(client, {})
	clients.append(client)
	
for client in clients:
	print "rc from subscribe is", mqttv3.subscribe(client, "$SYS/#")

for client in clients:
	print "rc from publish is", mqttv3.publish(client, "a topic", "a message")
	print "rc from publish is", mqttv3.publish(client, "a topic", "a message", 1)
	print "rc from publish is", mqttv3.publish(client, "a topic", "a message", 2)
	
print "about to sleep"
time.sleep(10)
print "finished sleeping"
	
for client in clients:
	print "rc from isConnected is", mqttv3.isConnected(client)
	print "rc from disconnect is", mqttv3.disconnect(client)
	mqttv3.destroy(client)

