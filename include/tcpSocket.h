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
#define CONTROL_PORT 8182
#define MAXBUFFSIZE 1024

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
		//CONSTRUCTORS
		tcpSocket();
		tcpSocket(int sockfd);
		
		//GENERAL FUNCTIONS
		int connect();
		int bind();
		int listen();
		tcpSocket accept();
		void close();
		int sendMsg(string msg, int flags);
		string recvMsg(int flags);

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