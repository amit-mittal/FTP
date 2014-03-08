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

class ftpServer{
	private:
		tcpSocket socket;

	public:
		//GENERAL FUNCTIONS
		void start();

		//GETTER
		tcpSocket getSocket();

		//SETTER
		void setSocket(tcpSocket tcpSocket);
};