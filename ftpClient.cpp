#include "include/ftpClient.h"

tcpSocket ftpClient::getControlSocket(){
	return c_socket;
}

tcpSocket ftpClient::getDataSocket(){
	return d_socket;
}

void ftpClient::setControlSocket(tcpSocket socket){
	this->c_socket = socket;
}

void ftpClient::setDataSocket(tcpSocket socket){
	this->d_socket = socket;
}

int ftpClient::connect(){
	c_socket.setIp(IP);
	c_socket.setPort(CONTROL_PORT);
	c_socket.setHints(0);
	if(c_socket.connect() != 0)
		return 1;
	printf("CLIENT: successfully connected to server(control socket): %s\n", IP);

	int port = atoi(c_socket.recvMsg(0).c_str());
	cout << port << endl;

	d_socket.setIp(IP);
	d_socket.setPort(port);
	d_socket.setHints(0);
	if(d_socket.connect() != 0)
		return 1;
	printf("CLIENT: successfully connected to server again(data socket): %s\n", IP);

	return 0;
}

int main(){
	ftpClient client;
	if(client.connect() != 0)
		exit(1);

	return 0;
}