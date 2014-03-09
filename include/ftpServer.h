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
#include "../tcpSocket.cpp"

using namespace std;

#define INITPORT 23456

class ftpServer{
	private:
		int lastPort;
		tcpSocket socket;
		tcpSocket c_socket;//control socket
		tcpSocket d_socket;//data socket

	public:
		//CONSTRUCTOR
		ftpServer();

		//GENERAL FUNCTIONS
		int getNewPort();
		int acceptClient();
		int allocateDataPort();
		int handleClients();

		//GETTER
		tcpSocket getSocket();
		tcpSocket getControlSocket();
		tcpSocket getDataSocket();

		//SETTER
		void setSocket(tcpSocket tcpSocket);
		void setControlSocket(tcpSocket tcpSocket);
		void setDataSocket(tcpSocket tcpSocket);
};

string getStrFromInt(int port);