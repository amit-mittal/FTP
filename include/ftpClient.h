#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../tcpSocket.cpp"

#define IP "10.4.21.9"

class ftpClient{
	private:
		tcpSocket c_socket;//control socket
		tcpSocket d_socket;//data socket

	public:
		//GENERAL FUNCTIONS
		int connect();

		//GETTER
		tcpSocket getControlSocket();
		tcpSocket getDataSocket();

		//SETTER
		void setControlSocket(tcpSocket tcpSocket);
		void setDataSocket(tcpSocket tcpSocket);
};

int getIntFromStr(string str);
// // get sockaddr, IPv4 or IPv6:
// void *get_in_addr(struct sockaddr *sa);