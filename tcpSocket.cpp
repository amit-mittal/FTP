#include "include/tcpSocket.h"

tcpSocket::tcpSocket(int newfd){
	sockfd = newfd;
}

string tcpSocket::getIp(){
	return ip;
}

unsigned short tcpSocket::getPort(){
	return port;
}

struct addrinfo tcpSocket::getHints(){
	return hints;
}

int tcpSocket::getSocket(){
	return sockfd;
}

void tcpSocket::setIp(string ip){
	ip = ip;
}

void tcpSocket::setPort(unsigned short port){
	port = port;
}

void tcpSocket::setHints(int flag){
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if(flag == 1)
		hints.ai_flags = AI_PASSIVE; // use my IP
}

int tcpSocket::connect(){
	struct addrinfo *p;
	int rv;
	stringstream port_str;
	port_str<<port;

	setHints(0);
	if ((rv = getaddrinfo(ip.c_str(), port_str.str().c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close();
			perror("client: connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	return 0;//no error
}

int tcpSocket::bind(){
	struct addrinfo *p;
	int rv;
	int yes = 1;
	stringstream port_str;
	port_str<<port;

	setHints(1);
	if ((rv = getaddrinfo(NULL, port_str.str().c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close();
			perror("server: bind");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	return 0;//no error
}

int tcpSocket::listen(){
	if (::listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		return 1;
	}
	return 0;//no error
}

tcpSocket tcpSocket::accept(){
	int newfd = ::accept(sockfd, (struct sockaddr *)&their_addr, &their_addr_size);
	return tcpSocket(newfd);
}

void tcpSocket::close(){
	::close(sockfd);
}