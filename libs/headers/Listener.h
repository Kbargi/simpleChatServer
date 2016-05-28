#pragma once
#define HANDLER_ARRAY_SIZE 3
#define HEADER_BYTE_SIZE 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <string>
#include <stdexcept>
#include <atomic>
#include <memory>

#include "ThreadPool.h"
#include "SpecificTasks.h"

#define CLIENT_DATA_BUFFER_SIZE 4096

// remove
#include <iostream>

class Handler {
 public:
  Handler(int socket, std::shared_ptr<ThreadPool> t,
          std::shared_ptr<RoomManager> r)
      : m_readPipeSocket(socket),
        fd_max(socket),
        m_stop(false),
        m_pool(t),
        m_roomManager(r) {
    FD_ZERO(&m_master);
    FD_SET(m_readPipeSocket, &m_master);
  }
  ~Handler() {}
  void operator()();

 private:
  int recvHeader(int socket, size_t* buffer, size_t size);
  int recvBody(int socket, void* buffer, size_t size);
  void handleDataFromClient(int clientSocket);
  void handleNewConnection();
  int m_readPipeSocket;
  fd_set m_master;
  int fd_max;
  std::atomic<bool> m_stop;
  std::shared_ptr<ThreadPool> m_pool;
  std::shared_ptr<RoomManager> m_roomManager;
};
class Listener {
 public:
  Listener() = delete;

  Listener(const std::string&, size_t);
  virtual ~Listener();

  virtual void init();
  virtual void run();
  void stop();

 private:
  void* get_in_addr(struct sockaddr*);
  void handleNewConnection();

  int m_listener;
  int m_fdmax;
  fd_set m_master;
  const std::string m_port;
  bool m_stop;

  struct InternalHandler {
    std::shared_ptr<Handler> m_handler;
    int pipe[2];
    std::unique_ptr<std::thread> m_handlerExecutor;
  } m_handlers[HANDLER_ARRAY_SIZE];
};
