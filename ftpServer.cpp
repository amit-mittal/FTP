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

int ftpServer::send(string str){
	if(DEBUG)
		cout << "Sending msg: " << str << endl;
	return d_socket.sendMsg(str, 0);
}

string ftpServer::recv(){
	string str = c_socket.recvMsg(0);
	if(DEBUG)
		cout << "Received msg: " << str << endl;
	return str;
}

int ftpServer::start(){
	if(socket.bind() != 0){
		return 1;
	}
	if(socket.listen() != 0){
		return 2;
	}
	printf("Successfully started on PORT: %d\n", socket.getPort());

	return 0;
}

int ftpServer::acceptClient(){
	c_socket = getSocket().accept();
	if(c_socket.getSocket() == -1){
		return 3;
	}
	printf("Successfully connected to a client(control port)\n");
	cout << "IP connected is: " << c_socket.getOtherIp() << endl;
	return 0;
}

int ftpServer::allocateDataPort(){
	int new_port = atoi(recv().c_str());
	if(DEBUG)
		cout << "Got port: " << new_port << endl;
	
	d_socket.setIp(IP);//TODO: set to client's ip - if not able to do this, get it from client
	d_socket.setPort(new_port);
	d_socket.setHints(0);
	if(d_socket.connect() != 0)
		return 1;
	printf("Successfully connected to client again(data socket): %s\n", IP);
	
	return 0;
}

int ftpServer::sendResponse(){
	string cmd = recv();
	if(DEBUG)
		cout << "Got command from client: " << cmd << endl;

	if(cmd == "pwd"){
		//TODO: will have to use popen in place of system
		system(cmd.c_str());

		send(temp.str());
	} else {
		send("Invalid command sent to server");
	}

	return 0;
}

int ftpServer::handleClients(){
	if(start() != 0)
		return 1;

	while(1){
		if(acceptClient() != 0)
			return 2;
		if(allocateDataPort() != 0)
			return 3;

		pid_t childPID = fork();
		if(childPID == 0) {//child process
			//start waiting for the commands and send data
			while(1){
				printf("\nWaiting for request from client\n");
				if(sendResponse() != 0){
					cout << "Error while sending a response to client" << endl;
					break;
				}
			}
			if(DEBUG){
				printf("Terminating one of the child of server\n");
				exit(0);
			}
		} else if(childPID < 0) {//error while forking
			perror("error while forking");
			return 4;
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
	
	if(server.handleClients() != 0){
		cout << "Error while handling clients" << endl;
		exit(1);
	}

	return 0;
}