#pragma once


#include <sys/types.h>
#include <sys/socket.h>

#include "AbstractTask.h"
#include "RoomManager.h"
#include "SharedContainers.h"


class RequestHandlerTask : public AbstractTask {
public:
	RequestHandlerTask() = delete;
	RequestHandlerTask(int s, std::shared_ptr<RoomManager> roomManager, chat::Request&& request) :
                AbstractTask(TaskPriority::NORMAL), m_clientSocket(s), m_roomManager(roomManager), m_request(std::move(request)) {}

    virtual ~RequestHandlerTask(){};

    virtual void operator()();

private:

    int m_clientSocket;
    std::shared_ptr<RoomManager> m_roomManager;
    chat::Request m_request;
};

class OnUserEscapeTask : public AbstractTask {
public:
	OnUserEscapeTask() = delete;
	OnUserEscapeTask(int s, std::shared_ptr<RoomManager> roomManager) :
                AbstractTask(TaskPriority::HIGH), m_clientSocket(s), m_roomManager(roomManager) {}

    virtual ~OnUserEscapeTask(){};

    virtual void operator()();

private:

    int m_clientSocket;
    std::shared_ptr<RoomManager> m_roomManager;
};
