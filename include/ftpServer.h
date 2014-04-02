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
#include <sys/stat.h>	
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
		string client_IP; //To store the IP address of the client

	public:
		//CONSTRUCTOR
		ftpServer();

		//GENERAL FUNCTIONS
		int getNewPort();
		int sendData(string str);	//For sending through the data port
		int sendData(char* data, int num_bytes);//Overloaded function
		int sendControl(string str);	//For sending through the control port
		int receiveFile(string str);	//For receiving a file through the data socket with filename str
		int transferFile(string str);	//For transferring a file through the data socket with filename str
		int recvData(char* data);	//For receiving data
		string recvControl(int toRead);	//For receiving control information
		int fileExists(const char* filename);//To determine if a file exists with a given name
		int isEmpty(string filename);	//To determine if a file is empty
		int start();
		int acceptClient();
		int allocateDataPort();
		int handleClients();
		int sendResponse();

		//GETTER
		tcpSocket getSocket();
		tcpSocket getControlSocket();
		tcpSocket getDataSocket();

		//SETTER
		void setSocket(tcpSocket tcpSocket);
		void setControlSocket(tcpSocket tcpSocket);
		void setDataSocket(tcpSocket tcpSocket);
};

string getStrFromInt(int val);
