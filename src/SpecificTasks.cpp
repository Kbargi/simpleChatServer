#include "SpecificTasks.hpp"

void RequestHandlerTask::operator()() {
  m_roomManager->processRequest(m_clientSocket, m_request);
}
void OnUserEscapeTask::operator()() {
  m_roomManager->processEscape(m_clientSocket);
}
