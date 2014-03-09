#include "include/ftpServer.h"

ftpServer::ftpServer(){
	lastPort = INITPORT;
	socket.setPort(CONTROL_PORT);
	socket.setHints(1);
}

int ftpServer::getNewPort(){
	lastPort++;
	return lastPort;
}

tcpSocket ftpServer::getSocket(){
	return socket;
}

tcpSocket ftpServer::getControlSocket(){
	return c_socket;
}

tcpSocket ftpServer::getDataSocket(){
	return d_socket;
}

void ftpServer::setSocket(tcpSocket socket){
	this->socket = socket;
}

void ftpServer::setControlSocket(tcpSocket socket){
	this->c_socket = socket;
}

void ftpServer::setDataSocket(tcpSocket socket){
	this->d_socket = socket;
}

int ftpServer::acceptClient(){
	if(socket.bind() != 0){
		return 1;
	}
	if(socket.listen() != 0){
		return 2;
	}
	printf("SERVER: successfully started on PORT: %d\n", socket.getPort());

	c_socket = getSocket().accept();
	if(c_socket.getSocket() == -1){
		return 3;
	}
	printf("SERVER: successfully connected to a client(control port)");
	
	return 0;
}

int ftpServer::allocateDataPort(){
	int new_port = getNewPort();
	cout << new_port << endl;

	tcpSocket d_socket_init;
	d_socket_init.setPort(new_port);
	d_socket_init.setHints(1);
	
	if(d_socket_init.bind() != 0){
		return 1;
	}
	if(d_socket_init.listen() != 0){
		return 2;
	}
	printf("SERVER: successfully started on PORT: %d\n", d_socket_init.getPort());

	d_socket = d_socket_init.accept();
	if(d_socket.getSocket() == -1){
		return 3;
	}
	printf("SERVER: successfully connected to a client(data port)");

	c_socket.sendMsg(getStrFromInt(new_port), 0);//TODO: remove it from here, just for testing
	
	return 0;
}

int ftpServer::handleClients(){
	while(1){
		if(acceptClient() != 0)
			return 1;

		//sending this new_port to the client
		//TODO: make new function for sending messages from client(use control port)
		

		if(allocateDataPort() != 0)
			return 2;

		pid_t childPID = fork();
		if(childPID == 0) {//child process
			//start waiting for the commands and send data
		} else if(childPID < 0) {//error while forking
			perror("error while forking");
			return 3;
		} else {//parent process
			continue;
		}
	}
}

string getStrFromInt(int val){
	stringstream str;
	str << val;

	return str.str();
}

int main(){
	ftpServer server;
	
	if(server.handleClients() != 0)
		exit(1);

	return 0;
}