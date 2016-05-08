#include "SpecificTasks.h"

void HandlerTask::operator()() {
   int nbytes = -1;

   /*dodaj tu kanarki*/
   unsigned char buffer[CLIENT_DATA_BUFFER_SIZE];
   CLEAR_BUF(buffer);

   if((nbytes = recv(mClientSocket, buffer, sizeof(buffer), 0)) <= 0) {
       // got error or connection closed by client
      if (nbytes == 0) {// connection closed
         std::cout << "socket " << mClientSocket << " hung up\n";//zamien na log
      } else {
         std::cout<<"recv failed\n"; //zamien na log
      }
      close(mClientSocket); // bye!
      fdSet.remove(mClientSocket); // remove from master set
      m_roomManager.notifyUserEscape(mClientSocket);
      return;
   } else {
      // we got some data from a client
      chat::Request request;
      std::cout<< "Otrzymalem: \n" << buffer << " " << nbytes << " bajtow\n";
      if( !request.ParseFromArray(buffer,nbytes) ) {
         std::cout << "ParseFromArray failed\n";
         return;
      } else { // process received message
    	  m_roomManager.processRequest(mClientSocket, request);
      }
   }
}
