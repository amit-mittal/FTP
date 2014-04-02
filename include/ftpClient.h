#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <cstddef>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>	
#include <inttypes.h>	
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../tcpSocket.cpp"

#define CLIENT_PORT 20000
#define MAX_CMD_LEN 1024
#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const char  *sizes[]   = { "GiB", "MiB", "KiB", "B" };
static const long  exbibytes = 1024ULL * 1024ULL * 1024ULL;

class ftpClient{
	private:
		tcpSocket c_socket;//control socket
		tcpSocket d_socket;//data socket

	public:
		//GENERAL FUNCTIONS
		int sendData(char* data, int num_bytes);//For sending through the data port
		int sendControl(string str);	//For sending through the control port
		int recvData(char* data);	//Overloaded function
		string recvData();		//For receiving data
		string recvControl(int toRead);	//For receiving control information
		int receiveFile(string str);	//For receiving a file through the data socket with filename str
		int transferFile(string str);	//For transferring a file through the data socket with filename str
		int connect(string IP, int port);//IP is the ip address of the server to which the clients connect to. Port is the control port
		string getInput();
		string getOwnIP(tcpSocket);	//To get the client's own IP
		int sendRequest();
		char * calculateSize(long size);//To calculate the size of the file in appropriate format (like KB, MB etc. in place of bytes)
		int fileExists(const char* filename);//To determine if a file exists with a given name
		int isEmpty(string filename);	//To determine if a file is empty
		string trim(const string& str, const string& whitespace = " \t");  //To trim a string externally
		string reduce(const string& str, const string& fill = " ", const string& whitespace = " \t");  //To trim a string internally and externally

		//GETTER
		tcpSocket getControlSocket();
		tcpSocket getDataSocket();

		//SETTER
		void setControlSocket(tcpSocket tcpSocket);
		void setDataSocket(tcpSocket tcpSocket);
};

string getStrFromInt(int val);
void help();	//When wrong format is entered
