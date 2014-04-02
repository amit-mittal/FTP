#include "include/tcpSocket.h"

tcpSocket::tcpSocket(){
	their_addr_size = sizeof their_addr;
}

tcpSocket::tcpSocket(int newfd){
	sockfd = newfd;
	their_addr_size = sizeof their_addr;
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
	this->ip = ip;
}

void tcpSocket::setPort(unsigned short port){
	this->port = port;
}

void tcpSocket::setHints(int flag){
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if(flag == 1)//server
		hints.ai_flags = AI_PASSIVE; // use my IP
}

string tcpSocket::getOtherIp(){

	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];

	len = sizeof addr;
	getpeername(getSocket(), (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
    		struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
   		 struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	string str(ipstr);
	return str;
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
			perror("socket");
			continue;
		}
		if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close();
			perror("connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "failed to connect\n");
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
	if(newfd == -1)
		perror("accept");
	return tcpSocket(newfd);
}

void tcpSocket::close(){
	::close(sockfd);
}

int tcpSocket::sendMsg(string msg, int flags){
	int bytes_sent = send(sockfd, msg.c_str(), msg.size(), flags);
	if (bytes_sent < 0)
		perror("send");
	return bytes_sent;
}

string tcpSocket::recvMsg(int flags, int toRead){//toRead holds the number of bytes to be read from the socket
	char buffer[toRead];
	int bytes_recv = recv(sockfd, buffer, toRead-1, flags);
	if (bytes_recv < 0){
		perror("recv");
		return "";//error
	}

	buffer[bytes_recv] = '\0';
	string str(buffer);
	return str;
}
