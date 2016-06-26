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

#include "ThreadPool.hpp"
#include "SpecificTasks.hpp"

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
  Listener() : m_listener(-1), m_fdmax(-1), m_port(std::string("27015")), m_stop(false) {}

  virtual ~Listener();

  virtual void init(const std::string&, size_t);
  virtual void run();
  void stop();

 private:
  void initHandlers( size_t poolSize);
  void* get_in_addr(struct sockaddr*);
  void handleNewConnection();

  int m_listener;
  int m_fdmax;
  fd_set m_master;
  std::string m_port;
  std::atomic<bool> m_stop;

  struct InternalHandler {
    std::shared_ptr<Handler> m_handler;
    int pipe[2];
    std::unique_ptr<std::thread> m_handlerExecutor;
  } m_handlers[HANDLER_ARRAY_SIZE];
};
