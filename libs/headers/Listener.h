#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string>
#include <stdexcept>
#include <atomic>
#include <memory>

#include "ThreadPool.h"
#include "RoomManager.h"
#include "SpecificTasks.h"



//remove
#include <iostream>

class Listener {
    public:

	    Listener() = delete;

	    Listener(const std::string&, size_t);
	    virtual ~Listener();

	    virtual void init();
	    virtual void run();

    private:

	    void* get_in_addr(struct sockaddr*);
	    void handleNewConnection();
	    void handleDataFromClient(int it);

	    int m_listener;
	    const std::string m_port;
	    std::atomic<bool> m_stop;
	    FileDescriptorSharedWrapper m_masterSet; // master file descriptor list
	    std::shared_ptr<ThreadPool> m_pool;
	    RoomManager m_roomManager;
};
