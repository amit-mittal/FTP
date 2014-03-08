#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define BACKLOG 10

class tcpSocket{
	private:
		int sockfd;
		string ip;
		unsigned short port;
		struct addrinfo hints;
		struct addrinfo *servinfo;
		struct sockaddr_storage their_addr;
		socklen_t their_addr_size;

	public:
		//GENERAL FUNCTIONS
		tcpSocket(int sockfd);
		int connect();
		int bind();
		int listen();
		tcpSocket accept();
		void close();

		//SETTERS
		void setIp(string ip);
		void setPort(unsigned short port);
		void setHints(int flag);

		//GETTERS
		string getIp();
		unsigned short getPort();
		struct addrinfo getHints();
		int getSocket();
};