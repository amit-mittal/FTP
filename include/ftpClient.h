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
#define CLIENT_PORT 20000
#define MAX_CMD_LEN 1024

class ftpClient{
	private:
		tcpSocket c_socket;//control socket
		tcpSocket d_socket;//data socket

	public:
		//GENERAL FUNCTIONS
		int send(string str);
		string recv();
		int connect();
		string getInput();
		int sendRequest();

		//GETTER
		tcpSocket getControlSocket();
		tcpSocket getDataSocket();

		//SETTER
		void setControlSocket(tcpSocket tcpSocket);
		void setDataSocket(tcpSocket tcpSocket);
};

string getStrFromInt(int val);