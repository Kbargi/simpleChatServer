import chat_pb2
import time
import socket                                                                                                                                                 
import string
import sys
import struct
import random
import string
def custom_content(i):
        return str('Czesc: ') + str(i)
def content_generator(chars=string.ascii_uppercase + string.digits + string.ascii_lowercase):
        return ''.join(random.choice(chars) for _ in range(random.randrange(0, 4000)))
class Room:
    def __init__(self, name, password):
        self.name = name
        self.password = password
    def getName(self):
        return self.name
    def getPassword(self):
        return self.password
class User:
    def __init__(self, name, password, address = 'localhost', port = 18015):
        self.name = name
        self.password = password
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((address, port))
        self.room = None
    def sendRequest(self, message):
        message = str(struct.pack('!I', len(message))) + message
        self.sock.sendall(message)
        resp = chat_pb2.Response()
        resp.ParseFromString(self.sock.recv(4096))
        return resp
    def createRoom(self, name, password):
        if self.room is None:
            self.room = Room(name, password)
        message = chat_pb2.Request()
        message.operation = chat_pb2.Request.CREATE_ROOM
        message.roomName = self.room.getName()
        message.roomPassword = self.room.getPassword()
        message.userName = self.name
        message.userPassword = self.password
        return self.sendRequest(message.SerializeToString())
    def join_2_room(self, roomName, roomPassword):
        if self.room is None:
            self.room = Room(roomName, roomPassword)
        message = chat_pb2.Request()
        message.operation = chat_pb2.Request.JOIN_2_ROOM
        message.roomName = self.room.getName()
        message.roomPassword = self.room.getPassword()
        message.userName = self.name
        message.userPassword = self.password
        return self.sendRequest(message.SerializeToString())
    def deliver(self, roomName, roomPassword):
        if self.room is None:
            self.room = Room(roomName, roomPassword)
        message = chat_pb2.Request()
        message.operation = chat_pb2.Request.DELIVER
        message.roomName = self.room.getName()
        message.roomPassword = self.room.getPassword()
        message.userName = self.name
        message.userPassword = self.password
        message.content = custom_content(self.name)#content_generator()
        return self.sendRequest(message.SerializeToString())
    def recvDelivery(self):
        resp = chat_pb2.Response()
        resp.ParseFromString(self.sock.recv(4096))
        return resp
