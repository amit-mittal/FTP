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

int ftpServer::sendData(string str){
	if(DEBUG)
		cout << "Sending data: " << str << endl;
	return d_socket.sendMsg(str, 0);
}

int ftpServer::sendData(char* data, int num_bytes){
	
	int bytes_sent = send(d_socket.getSocket(), data, num_bytes, 0);
	if(DEBUG)
		cout << "Sending data: " << data << endl;
	return bytes_sent;
}

int ftpServer::sendControl(string str){
	if(DEBUG)
		cout << "Sending msg: " << str << endl;
	return c_socket.sendMsg(str, 0);
}

string ftpServer::recvControl(int toRead){
	string str = c_socket.recvMsg(0, toRead);
	if(DEBUG)
		cout << "Received msg: " << str << endl;
	return str;
}

int ftpServer::recvData(char* data){
	int bytes_recv = recv(d_socket.getSocket(), data, MAXBUFFSIZE-1, 0);
	return bytes_recv;
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
	printf("Successfully connected to a client (control port)\n");
	client_IP = c_socket.getOtherIp();
	return 0;
}

int ftpServer::allocateDataPort(){
	int new_port = atoi(recvControl(MAXBUFFSIZE).c_str());	
	if(DEBUG)
		cout << "Got port: " << new_port << endl;
	
	d_socket.setIp(client_IP);
	d_socket.setPort(new_port);
	d_socket.setHints(0);
	if(d_socket.connect() != 0)
		return 1;
	printf("Successfully connected to client again (data socket): %s\n", client_IP.c_str());
	
	return 0;
}

int ftpServer::sendResponse(){	
	string cmd = recvControl(MAXBUFFSIZE);
	FILE *pf;
	string str, responseControl;
	char output[MAXBUFFSIZE];
	if(DEBUG)
		cout << "Got command from client: " << cmd << endl;

	if(cmd.substr(0, 3) == "pwd"){
		
		pf = popen(cmd.append(" 2>&1").c_str(), "r");
		if(!pf){
			sendData(" ");
      			sendControl("Error occurred while opening pipe for output, calling fork or allocating memory.\n");
			cout << "Command could NOT be successfully executed for client\n";
			return(0); 
		}

		fgets(output, MAXBUFFSIZE-1, pf);
		str = output;
		sendData(str);
		if (pclose(pf) != 0){
			sendControl("Incorrect use of pwd \n");
			cout << "Command could NOT be successfully executed for client\n";
			return(0);
		}

		sendControl("pwd successfully executed in server \n");
		cout << "Command successfully executed for client\n";
	} 

	else if(cmd.substr(0, 2) == "ls"){
		pf = popen(cmd.append(" 2>&1").c_str(), "r");	//popen does not return stderr file handle. Only stdout. So we are redirecting stderr to stdout
		if(!pf){
			sendData(" ");
      			sendControl("Error occurred while opening pipe for output, calling fork or allocating memory.\n");
			cout << "Command could NOT be successfully executed for client\n";
			return(0); 
		}
	
		while(fgets(output, MAXBUFFSIZE-1, pf)){
			if(str.size() + strlen(output) >= MAXBUFFSIZE){
				sendData(str);
				str = "";
			}
			str.append(output);
		}
		
		str.append(" ");
		sendData(str);
		if (pclose(pf) != 0){
			sendControl("Incorrect use of ls \n");
			cout << "Command could NOT be successfully executed for client\n";
			return(0);
		}
		
		sendControl("ls successfully executed in server \n");
		cout << "Command successfully executed for client\n";
	} 

	else if(cmd.substr(0, 2) == "cd"){

		if(cmd.size() == 2){
			char *env = getenv("HOME");
			chdir(env);
			sendControl("Directory successfully changed \n");
			cout << "Command successfully executed for client\n";
			return(0);
		}

		if(cmd.substr(2, 2) == " ~"){
			char *env = getenv("HOME");
			cmd.replace(3, 1, env);
		}

		if(chdir(cmd.substr(3).c_str())){
			str = "Error occurred while changing directory using cd in server\ncd: ";
			str.append(cmd.substr(3)).append(": No such file or directory\n");
			sendControl(str);
			cout << "Command could NOT be successfully executed for client\n";
		}
		else {
			sendControl("Directory successfully changed \n");
			cout << "Command successfully executed for client\n";
		}
	}

	else if(cmd.substr(0, 3) == "put"){
		if(fileExists(cmd.substr(4).c_str())){
			sendControl("EXISTS");
			cout << "Command could NOT be successfully executed for client\n";
			return(0);
		}
		sendControl("DNE");	//Does Not Exist
		responseControl = recvControl(MAXBUFFSIZE);
		if(responseControl == "EMPTY"){
			FILE *fp = fopen(cmd.substr(4).c_str(), "w");
			fclose(fp);
			cout << "Command successfully executed for client\n";
			return 0;
		}
		if(receiveFile(cmd.substr(4))){
			sendControl("Error occurred while receiving file in server \n");
			cout << "Command could NOT be successfully executed for client\n";
		}
		else {
			sendControl("File successfully received in server \n");
			cout << "Command successfully executed for client\n";
		}
	}

	else if(cmd.substr(0, 3) == "get"){
		if(!fileExists(cmd.substr(4).c_str())){
			sendControl("DNE");
			cout << "Command could NOT be successfully executed for client\n";
			return(0);
		}
		sendControl("EXISTS");	//File Exists
		if(isEmpty(cmd.substr(4))){
			sendControl("EMPTY");	//Empty File
			cout << "Command could NOT be successfully executed for client\n";
			return(0);
		}
		sendControl("NEF");	//Not Empty File (NEF)
		if(transferFile(cmd.substr(4)) < 0){
			sendControl("Error occurred while sending file from server \n");
			cout << "Command could NOT be successfully executed for client\n";
		}
		else {
			sendControl("File successfully sent from server \n");
			cout << "Command successfully executed for client\n";
		}
	}

	else if(cmd == "quit"){
		d_socket.close();
		c_socket.close();
		cout << "Child Process exiting ... \n";
		cout << "Command successfully executed for client\n";
		exit(0);
	}

	else {
		sendControl("Invalid command sent to server \n");
	}

	return 0;
}

int ftpServer::receiveFile(string filename){
	
	char data[MAXBUFFSIZE];
	string msgControl;
	size_t bytes_recv;
	FILE* fp = fopen(filename.c_str(), "w");

	if (fp == NULL) {
		fputs("File not found\n", stderr); 
		return 6;	
	}
    
	while ((bytes_recv = recvData(data)) > 0) {
        	fwrite(data, 1, bytes_recv, fp);
		msgControl = recvControl(4);
		if(msgControl == "EOF")
			break;
	}

	if (bytes_recv < 0) {
        	perror("recv");
	}

	fclose(fp);
	return 0;	
}

int ftpServer::transferFile(string filename){

	FILE* fp;
	char data[MAXBUFFSIZE];
	size_t bytes_read, bytes_sent;
	long fSize, remaining;

	fp = fopen(filename.c_str(), "r");

	if (fp == NULL) {
		fputs("File not found\n", stderr); 
		return 7;	
	}

	// obtain file size:
  	fseek (fp, 0, SEEK_END);
	fSize = ftell(fp);
	rewind(fp);

	remaining = fSize;
	do {
		bytes_read = fread(data, 1, MAXBUFFSIZE-1, fp);
	
		if (bytes_read < 0) {
			fputs("Error in reading the file\n", stderr);
			return 8;
		}

		if ((bytes_sent = sendData(data, bytes_read)) < 0) {
			fputs("Error in send function\n", stderr);
			return 9;
		}
		
		if((unsigned long)remaining == bytes_sent)
			sendControl("EOF");

		else	sendControl("FSR");

		fseek(fp, bytes_sent-bytes_read, SEEK_CUR);	//As send() function may not send bytes_read amount of data
		remaining -= bytes_sent;		
		
	} while (remaining > 0);

	fclose(fp);
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
				printf("\nWaiting for request from client -- \n");
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

/*To determine if a file exists with a given name*/
int ftpServer::fileExists(const char* filename){

	struct stat buffer;
	if (stat(filename, &buffer) != -1)
		return 1;	//File exists
	return 0;		//File does not exist
}

int ftpServer::isEmpty(string filename){
	ifstream file(filename.c_str(), ios::binary | ios::ate);
	if(!file.tellg())
		return 1;	//File is empty;
	return 0;		//File is not empty
}


int main(){
	ftpServer server;
	
	cout << "*********************** \nServer Program for FTP \n***********************\n";
	cout << "Amit Mittal & Rahul Radhakrishnan, IIT Guwahati\n\n";

	if(server.handleClients() != 0){
		cout << "Error while handling clients" << endl;
		exit(1);
	}

	return 0;
}
