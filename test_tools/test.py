import chat_pb2
import time
import socket                                                                                                                                                 
import string
import sys
import struct
m = chat_pb2.Request()
m.operation = chat_pb2.Request.CREATE_ROOM
m.roomName = str("NAZWA_LOSOWA")
m.roomPassword = str("HASLO_LOSOWE")
m.userName = str("USER_NAME")
m.userPassword = str("USER_PASSWORD")
m.content = str("TRESC...")
# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 18015)
print >>sys.stderr, 'connecting to %s port %s' % server_address
sock.connect(server_address)
try:
    m =  m.SerializeToString()
    size = len(m)
    m = str(struct.pack('!I', size)) + m
    while True:# Send data
        print >>sys.stderr, 'sending "%s\n"' % m
        time.sleep(5.0/1000.0);
        sock.sendall(m)
        print str("czekam...")
        aaa = sock.recv(4096);
        print str("odebralem: ")
        print aaa
finally:
    print >>sys.stderr, 'closing socket'
    sock.close()
