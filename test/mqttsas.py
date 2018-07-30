"""
*******************************************************************
  Copyright (c) 2013, 2018 IBM Corp.

  All rights reserved. This program and the accompanying materials
  are made available under the terms of the Eclipse Public License v1.0
  and Eclipse Distribution License v1.0 which accompany this distribution.

  The Eclipse Public License is available at
     http://www.eclipse.org/legal/epl-v10.html
  and the Eclipse Distribution License is available at
    http://www.eclipse.org/org/documents/edl-v10.php.

  Contributors:
     Ian Craggs - initial implementation and/or documentation
     Ian Craggs - add MQTTV5 support
*******************************************************************
"""
from __future__ import print_function

import socket, sys, select, traceback, datetime, os
try:
  import socketserver
  import MQTTV311    # Trace MQTT traffic - Python 3 version
  import MQTTV5
except:
  traceback.print_exc()
  import SocketServer as socketserver
  import MQTTV3112 as MQTTV311   # Trace MQTT traffic - Python 2 version
  import MQTTV5

MQTT = MQTTV311
logging = True
myWindow = None


def timestamp():
  now = datetime.datetime.now()
  return now.strftime('%Y%m%d %H%M%S')+str(float("."+str(now.microsecond)))[1:]

suspended = []

class MyHandler(socketserver.StreamRequestHandler):

  def handle(self):
    global MQTT
    if not hasattr(self, "ids"):
      self.ids = {}
    if not hasattr(self, "versions"):
      self.versions = {}
    inbuf = True
    i = o = e = None
    try:
      clients = self.request
      brokers = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      brokers.connect((brokerhost, brokerport))
      terminated = False
      while inbuf != None and not terminated:
        (i, o, e) = select.select([clients, brokers], [], [])
        for s in i:
          if s in suspended:
             print("suspended")
          if s == clients and s not in suspended:
            inbuf = MQTT.getPacket(clients) # get one packet
            if inbuf == None:
              break
            try:
              # if connect, this could be MQTTV3 or MQTTV5
              if inbuf[0] >> 4 == 1: # connect packet
                protocol_string = b'MQTT'
                pos = inbuf.find(protocol_string)
                if pos != -1:
                  version = inbuf[pos + len(protocol_string)]
                  if version == 5:
                    MQTT = MQTTV5
                  else:
                    MQTT = MQTTV311
              packet = MQTT.unpackPacket(inbuf)
              if hasattr(packet.fh, "MessageType"):
                packet_type = packet.fh.MessageType
                publish_type = MQTT.PUBLISH
                connect_type = MQTT.CONNECT
              else:
                packet_type = packet.fh.PacketType
                publish_type = MQTT.PacketTypes.PUBLISH
                connect_type = MQTT.PacketTypes.CONNECT
              if packet_type == publish_type and \
                  packet.topicName == "MQTTSAS topic" and \
                  packet.data == b"TERMINATE":
                print("Terminating client", self.ids[id(clients)])
                brokers.close()
                clients.close()
                terminated = True
                break
              elif packet_type == publish_type and \
                  packet.topicName == "MQTTSAS topic" and \
                  packet.data == b"TERMINATE_SERVER":
                print("Suspending client ", self.ids[id(clients)])
                suspended.append(clients)
              elif packet_type == connect_type:
                self.ids[id(clients)] = packet.ClientIdentifier
                self.versions[id(clients)] = 3
              print(timestamp() , "C to S", self.ids[id(clients)], str(packet))
              #print([hex(b) for b in inbuf])
              #print(inbuf)
            except:
              traceback.print_exc()
            brokers.send(inbuf)       # pass it on
          elif s == brokers:
            inbuf = MQTT.getPacket(brokers) # get one packet
            if inbuf == None:
              break
            try:
              print(timestamp(), "S to C", self.ids[id(clients)], str(MQTT.unpackPacket(inbuf)))
            except:
              traceback.print_exc()
            clients.send(inbuf)
      print(timestamp()+" client "+self.ids[id(clients)]+" connection closing")
    except:
      print(repr((i, o, e)), repr(inbuf))
      traceback.print_exc()
    if id(clients) in self.ids.keys():
      del self.ids[id(clients)]
    elif id(clients) in self.versions.keys():
      del self.versions[id(clients)]

class ThreadingTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
  pass

def run():
  global brokerhost, brokerport
  myhost = '127.0.0.1'
  if len(sys.argv) > 1:
    brokerhost = sys.argv[1]
  else:
    brokerhost = '127.0.0.1'

  if len(sys.argv) > 2:
    brokerport = int(sys.argv[2])
  else:
    brokerport = 1883

  if len(sys.argv) > 3:
    myport = int(sys.argv[3])
  else:
    if brokerhost == myhost:
      myport = brokerport + 1
    else:
      myport = 1883

  print("Listening on port", str(myport)+", broker on port", brokerport)
  s = ThreadingTCPServer(("127.0.0.1", myport), MyHandler)
  s.serve_forever()

if __name__ == "__main__":
  run()
