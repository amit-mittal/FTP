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

int ftpClient::sendData(char* data, int num_bytes){
	
	int bytes_sent = send(d_socket.getSocket(), data, num_bytes, 0);
	if(DEBUG)
		cout << "Sending data: " << data << endl;
	return bytes_sent;
}

int ftpClient::sendControl(string str){
	if(DEBUG)
		cout << "Sending msg: " << str << endl;
	return c_socket.sendMsg(str, 0);
}

int ftpClient::recvData(char* data){
	int bytes_recv = recv(d_socket.getSocket(), data, MAXBUFFSIZE-1, 0);
	return bytes_recv;
}

string ftpClient::recvData(){
	string str = d_socket.recvMsg(0, MAXBUFFSIZE);
	if(DEBUG)
		cout << "Received data: " << str << endl;
	return str;
}

string ftpClient::recvControl(int toRead){
	string str = c_socket.recvMsg(0, toRead);
	if(DEBUG)
		cout << "Received msg: " << str << endl;
	return str;
}


int ftpClient::transferFile(string filename){
	
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
		
		if(remaining == bytes_sent)
			sendControl("EOF");

		else	sendControl("FSR");

		fseek(fp, bytes_sent-bytes_read, SEEK_CUR);	//As send() function may not send bytes_read amount of data
		remaining -= bytes_sent;		
		
	} while (remaining > 0);

	fclose(fp);
	cout << "\nSize of file transferred : " << calculateSize(fSize);

	return 0;
}

int ftpClient::receiveFile(string filename){

	char data[MAXBUFFSIZE];
	string msgControl;
	size_t bytes_recv;
	long filesize = 0;
	FILE* fp = fopen(filename.c_str(), "w");

	if (fp == NULL) {
		fputs("File not found\n", stderr); 
		return 6;	
	}
    
	while ((bytes_recv = recvData(data)) > 0) {
        	fwrite(data, 1, bytes_recv, fp);
		msgControl = recvControl(4);
		filesize += bytes_recv;	
		if(msgControl == "EOF")
			break;
	}

	if (bytes_recv < 0) {
        	perror("recv");
	}

	fclose(fp);
	cout << "\nSize of file received : " << calculateSize(filesize);

	return 0;	
}

int ftpClient::connect(string serverIP, int controlPort){
	c_socket.setIp(serverIP);
	c_socket.setPort(controlPort);
	c_socket.setHints(0);
	if(c_socket.connect() != 0)
		return 1;
	cout << "Successfully connected to server (control socket): " << serverIP << endl;

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
	if(sendControl(getStrFromInt(CLIENT_PORT)) == -1){
		return 4;
	}

	d_socket = d_socket_init.accept();
	if(d_socket.getSocket() == -1){
		return 5;
	}
	
	printf("Successfully connected to server(data port)\n");
	printf("Type 'help' for more info\n\n");

	return 0;
}

string ftpClient::getInput(){//takes the input
	char input[MAX_CMD_LEN];
	string cmd;

	fgets(input, MAX_CMD_LEN-1, stdin);
	input[strlen(input)-1] = '\0';
	stringstream s;
	s << input;
	cmd = s.str();
	if(DEBUG)
		cout << "Command entered: " << cmd << endl;
	cmd = reduce(cmd);

	return cmd;
}

string ftpClient::trim(const string& str,
                       const string& whitespace) {

    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

string ftpClient::reduce(const string& str,
                   const string& fill,
                   const string& whitespace) {
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != string::npos) {

        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }
    return result;
}

int ftpClient::sendRequest(){
	string cmd = getInput();
	string responseData, responseControl;
	string str, response;
	FILE *pf;
	char input[5];	//For overwrite response
	char output[MAXBUFFSIZE];
	int pos = 0;

	if(cmd.substr(0, 4) == "!pwd"){
		cout << "Result is:\n";
		pf = popen(cmd.append(" 2>&1").substr(1).c_str(), "r");	//popen is used instead of system because we want to change pwd to !pwd
		if(!pf){
      			fputs("Error occurred while opening pipe for output, calling fork or allocating memory.\n", stderr);
			return(0);
		}
	
		while(fgets(output, MAXBUFFSIZE-1, pf)){
			str.append(output);
			if((pos = str.find("pwd", pos)) != string::npos){	//Search for pwd in the output and if present, replace with !pwd
				str.replace(pos, 3, "!pwd");
				pos += 2;			
			}
		}
		
		cout << str;
		if (pclose(pf) != 0){
			fputs("Incorrect use of !pwd \n", stderr);
			return(0);
		}
		
		if(DEBUG)
			cout << "!pwd successfully executed \n";
	} else if(cmd.substr(0, 3) == "!ls"){
		cout << "Result is\n";
		pf = popen(cmd.append(" 2>&1").substr(1).c_str(), "r");	
		if(!pf){
      			fputs("Error occurred while opening pipe for output, calling fork or allocating memory.\n", stderr);
			return(0);
		}
	
		while(fgets(output, MAXBUFFSIZE-1, pf)){
			str.append(output);
			if((pos = str.find("ls", pos)) != string::npos){	//Find occurrences of ls and replace them with !ls
				str.replace(pos, 2, "!ls");
				pos += 2;			
			}
		}
		
		cout << str << endl;
		if (pclose(pf) != 0){
			fputs("Incorrect use of !ls \n", stderr);
			return(0);
		}
		if(DEBUG)
			cout << "!ls successfully executed \n";
	} else if(cmd.substr(0, 3) == "!cd"){

		if(cmd.size() == 3){
			char *env = getenv("HOME");
			cout << "Changing directory to: "  << env << endl;
			chdir(env);
			cout << "Directory successfully changed. " << endl;
			return(0);
		}

		if(cmd.substr(3, 2) == " ~"){
			char *env = getenv("HOME");
			cmd.replace(4, 1, env);
		}
		cout << "Changing directory to: "  << cmd.substr(4) << endl;
		if(chdir(cmd.substr(4).c_str())){
			str = "Error occurred while changing directory using !cd\n!cd: ";
			str.append(cmd.substr(4)).append(": No such file or directory\n");
			cout << str;
		}
		else	cout << "Directory successfully changed. " << endl;
		if(DEBUG)
			cout << "!cd successfully executed \n";
	} else if(cmd == "quit"){
		cout << "Client process exiting ...\n";
		sendControl(cmd);
		d_socket.close();
		c_socket.close();
		exit(0);
	} else if(cmd.substr(0, 3) == "pwd"){
		sendControl(cmd);
		responseData = recvData();
		responseControl = recvControl(MAXBUFFSIZE);
		cout << responseData;
		cout << responseControl;
	} else if(cmd.substr(0, 2) == "ls"){
		sendControl(cmd);
		while(1){
			responseData = recvData();
			if(responseData.substr(responseData.size()-1) == " "){
				cout << responseData;
				break;
			}
			cout << responseData;
		}
		responseControl = recvControl(MAXBUFFSIZE);
		cout << responseControl;
	} else if(cmd.substr(0, 2) == "cd"){
		sendControl(cmd);
		responseControl = recvControl(MAXBUFFSIZE);
		cout << responseControl;
	} else if(cmd.substr(0, 3) == "put"){
		if(!fileExists(cmd.substr(4).c_str())){
			cout << cmd.substr(4) << ": No such file to put\n";
			return(0);
		}
		else if(access(cmd.substr(4).c_str(), R_OK) == -1){
			cout << "No permission to read the file\n";
			return(0);
		}
		sendControl(cmd);
		responseControl = recvControl(MAXBUFFSIZE);
		if(responseControl == "EXISTS"){	//Don't allow overwriting on the server
			cout << cmd.substr(4) << ": File already exists on the server. Cannot overwrite.\n";
			return(0);
		}
		if(isEmpty(cmd.substr(4))){
			sendControl("EMPTY");	//Empty File
			cout << "\nSize of file transferred : 0 B\n";
			cout << "File transfer to server successful  \n";
			cout << "File successfully received in server \n";
			return(0);
		}
		sendControl("NEF");	//Not Empty File (NEF)
		if(transferFile(cmd.substr(4)) < 0)
			cout << "\nError occurred while transferring file to server \n";
		else cout << "\nFile transfer to server successful \n";
		responseControl = recvControl(MAXBUFFSIZE);
		cout << responseControl;		
	} else if(cmd.substr(0, 3) == "get"){
		if(access("./", W_OK) == -1){
			cout << "No permission to write\n";
			return(0);
		}
		if(fileExists(cmd.substr(4).c_str())){
			cout << cmd.substr(4) << ": File already exists\nDo you want to overwrite? (y/n): ";
			while(1){
				fgets(input, 5, stdin);
				response = input;
				if(response == "n\n")
					return(0);
				else if(response == "y\n")
					break;
				else{
					cout << "Please enter either y or n : ";
					continue;
				}
			}
		}
		sendControl(cmd);
		responseControl = recvControl(MAXBUFFSIZE);
		if(responseControl == "DNE"){
			cout << cmd.substr(4) << ": No such file to get\n";
			return(0);
		}
		responseControl = recvControl(MAXBUFFSIZE);
		if(responseControl == "EMPTY"){
			FILE *fp = fopen(cmd.substr(4).c_str(), "w");
			fclose(fp);
			cout << "\nSize of file received : 0 B\n";
			cout << "File successfully received from server \n";
			cout << "File successfully sent from server \n";
			return 0;
		}
		if(receiveFile(cmd.substr(4)))
			cout << "\nError occurred while receiving file from server \n";
		else cout << "\nFile successfully received from server \n";
		responseControl = recvControl(MAXBUFFSIZE);
		cout << responseControl;
	} else if(cmd == "help"){
		printf("\nOptions available:\n1. !ls\n2. !cd\n3. !pwd\n4. ls\n5. cd\n6. pwd\n7. get\n8. put\n9. quit\n\n");
	} else if(cmd.size() == 0){
		return(0);
	} else {
		cout << "An invalid FTP Command. Type 'help'\n";
	}

	return 0;
}

/*Function for expressing the size of a file, given the number of bytes, in the appropriate format - KiB, MiB etc.*/
char * ftpClient::calculateSize(long size)
{   
    char *result = (char *) malloc(sizeof(char) * 20);
    unsigned long  multiplier = exbibytes;
    int i;

    for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
    {   
        if (size < multiplier)
            continue;
        if (size % multiplier == 0)
            sprintf(result, "%lu %s", (unsigned long) size / multiplier, sizes[i]);
        else
            sprintf(result, "%.5f %s", (float) size / multiplier, sizes[i]);
        return result;
    }
    strcpy(result, "0");
    return result;
}

int ftpClient::isEmpty(string filename){
	ifstream file(filename.c_str(), ios::binary | ios::ate);
	if(!file.tellg())
		return 1;	//File is empty;
	return 0;		//File is not empty
}

void help(){
	cout << "*********************** \nClient Program for FTP \n***********************\n";
	cout << "Amit Mittal & Rahul Radhakrishnan, IIT Guwahati\n\n";
	cout << "Usage : ./ftpClient Hostname Port\n\n";
	cout << "-- *Hostname* refers to the hostname/IP address of the server\n";
	cout << "-- *Port* refers to the server's control port (8182 in our case)\n";
	exit(1);
}

string getStrFromInt(int val){
	stringstream str;
	str << val;

	return str.str();
}

/*To determine if a file exists with a given name*/
int ftpClient::fileExists(const char* filename){

	struct stat buffer;
	if (stat(filename, &buffer) != -1)
		return 1;	//File exists
	return 0;		//File does not exist
}

int main(int argc, char *argv[]){

	if(argc != 3){
		help();
	}

	string IP(argv[1]);
	ftpClient client;
	if(client.connect(IP, atoi(argv[2])) != 0){
		cout << "Error while connecting to server" << endl;
		exit(1);
	}

	while(1){
		printf("ftp> ");
		if(client.sendRequest() != 0){
			cout << "Error while sending a request to server" << endl;
			break;
		}
	}

	return 0;
}
