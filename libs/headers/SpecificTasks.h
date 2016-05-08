#pragma once


#include <sys/types.h>
#include <sys/socket.h>

#include "AbstractTask.h"
#include "RoomManager.h"
#include "SharedContainers.h"

#define CLIENT_DATA_BUFFER_SIZE 25
#define CLEAR_BUF(a) memset((a), (0), (sizeof(a)));

class HandlerTask : public AbstractTask {
public:
    HandlerTask() = delete;
    HandlerTask(int s, FileDescriptorSharedWrapper& fdSet, RoomManager& roomManager) :
                AbstractTask(TaskPriority::NORMAL), mClientSocket(s), fdSet(fdSet), m_roomManager(roomManager) {}

    virtual ~HandlerTask(){};

    virtual void operator()();

private:

    int mClientSocket;
    FileDescriptorSharedWrapper& fdSet;
    RoomManager& m_roomManager;
};
