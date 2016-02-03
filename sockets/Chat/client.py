#pyhon chat client 3.5
import sys
import socket
import select
import re

if(len(sys.argv) < 4):
	print("host port nickname")
	sys.exit()

host = sys.argv[1]
port = int(sys.argv[2])
nick = "NICK " + sys.argv[3]

#create socket
sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sock.settimeout(10)

# Conect socket
if sock.connect_ex((host, port)) > 0:
	print("Error on creating socket")
	sys.exit()

#Recv welcome msg
incomingMSG = sock.recv(256)
print(incomingMSG.decode("utf-8"))

#Send nick to server
sock.send(nick.encode("utf-8"))
incomingMSG = sock.recv(256)


#Check if nick is ok
incomingMSG = incomingMSG.decode("utf-8")
print(incomingMSG)
while(re.match("ERROR" ,incomingMSG)):
	nick = sys.stdin.readline()

sock.close()