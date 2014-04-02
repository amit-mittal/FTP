#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define DEBUG 0
#define BACKLOG 10
#define CONTROL_PORT 8182
#define MAXBUFFSIZE 1024

void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
    		return &(((struct sockaddr_in*)sa)->sin_addr);
	}	
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
		string getOtherIp();
		int connect();
		int bind();
		int listen();
		tcpSocket accept();
		void close();
		int sendMsg(string msg, int flags);
		string recvMsg(int flags, int toRead);	//toRead holds the number os bytes to read from the stream

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
