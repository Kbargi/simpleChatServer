/*Code based on: http://beej.us/guide/bgnet/output/html/multipage/advanced.html*/

#include "Listener.h"

Listener::Listener(const std::string& port, size_t poolSize)
: m_listener(-1), m_port(port), m_stop(false), m_pool(std::make_shared<ThreadPool>(poolSize)) {}

Listener::~Listener() {
	m_pool->stop();
}

void Listener::init() {
   struct addrinfo hints, *ai, *p = NULL;
   int yes = 1, rv;

   m_masterSet.clearAll();    // clear the master

   // get us a socket and bind it
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   if ((rv = getaddrinfo(NULL, m_port.c_str(), &hints, &ai)) != 0) {
      std::cout<<"1\n";
      throw std::runtime_error(std::string(gai_strerror(rv)));
   }
   for(p = ai; p; p = p->ai_next) {
      if ((m_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
         continue;
         // lose the pesky "address already in use" error message
         setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
         if (bind(m_listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(m_listener);
            continue;
         }
         break;
   }
   if(!p)
      throw std::runtime_error(std::string("failed to bind"));
   freeaddrinfo(ai); // all done with this

   if (listen(m_listener, 10) == -1)
      throw std::runtime_error(std::string("listen failed"));

   // add the listener to the master set
   m_masterSet.add(m_listener);
}
void Listener::run() {
	fd_set read_fds;  // temp file descriptor list for select()
	FD_ZERO(&read_fds);

	// keep track of the biggest file descriptor
	int fdmax = m_masterSet.getMaxFd(); // so far, it's this one

	while(!m_stop) {
		read_fds = m_masterSet.getCopy();
		fdmax = m_masterSet.getMaxFd();
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        	throw std::runtime_error(std::string("select failed"));
        // run through the existing connections looking for data to read
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
            	if (i == m_listener)
                    this->handleNewConnection();
            	else
            		this->handleDataFromClient(i);
            } // END got new incoming connection
        } //for through existing connections
    } //main while
} //run

void* Listener::get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET)
    	return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void Listener::handleNewConnection() {
	struct sockaddr_storage remoteaddr; // client address
	char remoteIP[INET6_ADDRSTRLEN];
    socklen_t addrlen = sizeof(remoteaddr);
    int newfd = accept(m_listener, (struct sockaddr *)&remoteaddr, &addrlen);
    if(newfd == -1)
        std::cout<<"accept failed\n";////zamienic na log
    else {
         m_masterSet.add(newfd); // add to master set
         std::cout<< "new connection from " <<
             inet_ntop(remoteaddr.ss_family,
                 get_in_addr((struct sockaddr*)&remoteaddr),
                 remoteIP, INET6_ADDRSTRLEN) <<
             " on socket " << newfd << "\n"; //zamienic na log
    }
}
void Listener::handleDataFromClient(int i) {
   m_pool->add(std::make_shared<HandlerTask>(i, m_masterSet, m_roomManager));
}

