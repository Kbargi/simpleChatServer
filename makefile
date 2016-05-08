export PKG_CONFIG_PATH=${PWD}
CPP=g++
CPPFLAGS=--std=c++11 -Wall -lpthread -lboost_system -lboost_thread -pedantic -pthread #-Wextra 
HEADERS=${PWD}/libs/headers/
SOURCES=${PWD}/libs/src/
PBFLAGS=`pkg-config --cflags --libs protobuf`
SECURE_CPP=-fPIE -pie -fstack-protector-all
all: server

server: chat.pb.o RoomManager.o ThreadPool.o Listener.o Config.o SpecificTasks.o main.o
	$(CPP) $(CPPFLAGS) $(SECURE_CPP) *.o -o server.out $(PBFLAGS)

chat.pb.o: chat.proto
	protoc --cpp_out=. chat.proto
	$(CPP) -c chat.pb.cc -o chat.pb.o $(PBFLAGS) $(SECURE_CPP) 

ThreadPool.o: $(SOURCES)/ThreadPool.cpp $(HEADERS)/ThreadPool.h
	$(CPP) -c $(CPPFLAGS) $(SECURE_CPP) $(SOURCES)/ThreadPool.cpp -I $(HEADERS) -I ./

Listener.o: $(SOURCES)/Listener.cpp $(HEADERS)/Listener.h
	$(CPP) -c $(CPPFLAGS) $(SECURE_CPP) $(SOURCES)/Listener.cpp -I $(HEADERS) -I ./

Config.o: $(SOURCES)/Config.cpp $(HEADERS)/Config.h
	$(CPP) -c $(CPPFLAGS) $(SECURE_CPP) $(SOURCES)/Config.cpp -I $(HEADERS) 

SpecificTasks.o: $(SOURCES)/SpecificTasks.cpp $(HEADERS)/SpecificTasks.h
	$(CPP) -c $(CPPFLAGS) $(SECURE_CPP) $(SOURCES)/SpecificTasks.cpp -I $(HEADERS) -I ./

RoomManager.o: $(SOURCES)/RoomManager.cpp $(HEADERS)/RoomManager.h
	$(CPP) -c $(CPPFLAGS) $(SECURE_CPP) $(SOURCES)/RoomManager.cpp -I $(HEADERS) -I ./

main.o: main.cpp 
	$(CPP) -c $(CPPFLAGS) $(SECURE_CPP) main.cpp -I $(HEADERS) -I ./

clean:
	rm -fr ./*.o
	rm -fr ./chat.pb.*
	rm -fr ./*.out
