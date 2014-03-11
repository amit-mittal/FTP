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

int ftpClient::send(string str){
	if(DEBUG)
		cout << "Sending msg: " << str << endl;
	return c_socket.sendMsg(str, 0);
}

string ftpClient::recv(){
	string str = d_socket.recvMsg(0);
	if(DEBUG)
		cout << "Received msg: " << str << endl;
	return str;
}

int ftpClient::connect(){
	c_socket.setIp(IP);
	c_socket.setPort(CONTROL_PORT);
	c_socket.setHints(0);
	if(c_socket.connect() != 0)
		return 1;
	printf("Successfully connected to server(control socket): %s\n", IP);

	tcpSocket d_socket_init;
	d_socket_init.setPort(CLIENT_PORT);
	d_socket_init.setHints(1);
	if(d_socket_init.bind() != 0){
		return 2;
	}
	if(d_socket_init.listen() != 0){
		return 3;
	}
	printf("Successfully started on PORT: %d\n", d_socket_init.getPort());

	//sending this new_port to the server
	if(send(getStrFromInt(CLIENT_PORT)) == -1){
		return 4;
	}

	d_socket = d_socket_init.accept();
	if(d_socket.getSocket() == -1){
		return 5;
	}
	printf("Successfully connected to server(data port)\n");

	return 0;
}

string ftpClient::getInput(){//takes the input and trims it
	char input[MAX_CMD_LEN];
	string cmd;

	cout << "Enter command: ";
	fgets(input, MAX_CMD_LEN - 1, stdin);
	input[strlen(input) - 1] = '\0';
	stringstream s;
	s << input;
	cmd = s.str();
	if(DEBUG)
		cout << "Command entered: " << cmd << endl;
	//TODO: trim the command

	return cmd;
}

int ftpClient::sendRequest(){
	string cmd = getInput();

	//TODO: see popen, system, close and other system commands
	if(cmd == "!pwd"){
		cout << "Result is:\n";
		close(2);
		if(system("pwd -z") < 0){//command not successful
			cout << "Incorrect use of !pwd\n";
		}
	} else if(cmd.substr(0, 3) == "!ls"){
		cout << "Result is\n";
		system(cmd.substr(1).c_str());
	} else if(cmd.substr(0, 3) == "!cd"){
		cout << "Changing directory to: "  << cmd.substr(4) << endl;
		chdir(cmd.substr(4).c_str());
	} else if(cmd == "exit"){//TODO: should we send something to server to terminate child process & close sockets there also
		//TODO closee all sockets and all
		cout << "Client exiting\n";
		exit(0);
	} else if(cmd == "pwd"){
		send(cmd);
		string response = recv();
	} else {
		cout << "Invalid Command\n";
	}

	return 0;
}

string getStrFromInt(int val){
	stringstream str;
	str << val;

	return str.str();
}

int main(){
	ftpClient client;
	if(client.connect() != 0){
		cout << "Error while connecting to server" << endl;
		exit(1);
	}

	while(1){
		//TODO: tell syntax also how to write command
		printf("\nOptions available:\n1. !ls\n2. !cd\n3. !pwd\n4. ls\n5. cd\n6. pwd\n7. get\n8. put\n9. exit\n");
		if(client.sendRequest() != 0){
			cout << "Error while sending a request to server" << endl;
			break;
		}
	}

	return 0;
}