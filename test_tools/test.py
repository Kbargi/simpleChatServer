import chat_pb2
import time
import socket                                                                                                                                                 
import string
import sys

m = chat_pb2.Wiadomosc()
m.type = chat_pb2.Wiadomosc.TALK
m.content = str("TO JEST MOJA WIADOMOSC:)")
#m.RecipientId = 123
# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 18015)
print >>sys.stderr, 'connecting to %s port %s' % server_address
sock.connect(server_address)
try:
    m =  m.SerializeToString()
    while True:# Send data
        print >>sys.stderr, 'sending "%s\n"' % m
        time.sleep(5.0/1000.0);
        sock.sendall(m)
finally:
    print >>sys.stderr, 'closing socket'
    sock.close()
