File Transfer Protocol (FTP)
============================
Implementation of FTP in C++

***************************
I. Implementation Outline :
***************************
1. 'C++' language was used to implement both the client and the server.
2. The following features were implemented :
	- Communication can happen between any host (not just localhost)
	- The server can handle multiple clients.
	- We have used fork() to spawn a new child process to take care of the clients' needs instead of creating new threads for each new client.
	- Connection is opened using TCP stream socket
	- All 9 required functionalities along with possible error handling capabilities have been implemented. Additional 'help' has also been provided.
	- A command other than these 9 would result in an error.
	- Two different sockets have been implemented, one for control flow and another for data flow. The data port would be provided by the client to the server. 		  Both the sockets are handled simultaneously by both the client and the server.
	- All possible errors with the above 9 commands have been taken care of. In case of an error at the server, it would be sent to the client through the 		  control socket and the client will in turn display it on the screen (stdout). 
	- Every kind of transfer other than data happens through the control socket. Data flow is through the data socket.
	- End of file transfer has been indicated as follows. We consider the situation of 'get'.
		- After every read from the file, the server sends the read data through the data socket along with some control information through the control 			  socket.
		- If end of file has not been reached, the server sends "FSR" which stands for 'File Still Remaining' to the client through the control socket.
		- The client receives the data and the control information simultaneously. It reads from the data socket, writes to the file and reads from the 		  control socket and checks if the file is still remaining or not. If it reads 'FSR', it knows that file is still yet to come.
		- When end of file is reached, the server sends "EOF" through the control socket along with some final piece of data (through data socket).
		- When the client received 'EOF', it writes the last piece of data received from the data stream and closes the file.
	- Trimming of commands has been done (removing the whitespaces etc.) both internally and externally.


*****************
II. Limitations :
*****************
1. We have not followed the RFC 959 documentation by  J. Postel & J. Reynolds for reply codes etc. As a result, our code may not be work with a general ftp server.


*****************
III. How to Run :
*****************
1. In the compressed file being submitted, these are the file lists :

	---------
	File list
	---------
	tcpSocket.h			Declaration of functions & constants related to tcpSocket class
	tcpSocket.cpp		Implementation of functions of tcpSocket class
	ftpClient.h			Declaration of functions & constants related to ftpClient class
	ftpClient.cpp		Implementation of functions of ftpClient & contains main function
	ftpServer.h			Declaration of functions & constants related to ftpServer class
	ftpServer.cpp		Implementation of functions of ftpServer & contains main function

   The header files (.h) are inside the 'include' folder.
2. Building and Running :

	-----
	Build
	-----
	Simply run "make"(without quotes)
	~ $ make

	- Typing "make" will compile the codes and create two executables named 'ftpClient' and 'ftpServer'. If you want to remove the exexutables, type "make clean".
	

		~ $ ./ftpServer
		*********************** 
		Server Program for FTP 
		***********************
		Amit Mittal & Rahul Radhakrishnan, IIT Guwahati

		Successfully started on PORT: 8182

	- Now, the server is up and running and listening for a connection.
	- For running the client program, for eg,

		~ $ ./ftpClient
		*********************** 
		Client Program for FTP 
		***********************
		Amit Mittal & Rahul Radhakrishnan, IIT Guwahati

		Usage : ./ftpClient Hostname Port

		-- *Hostname* refers to the hostname/IP address of the server
		-- *Port* refers to the server's control port (8182 in our case)
				
	- To connect the client to the running and listening FTP server, type (for eg.)

		  ~$ ./ftpClient 10.4.32.17 8182 
	
	- This will connect the client to the server and various operations like cd, !cd, get, put, pwd, ls etc. can be performed.

	In short,
	-----------------------------
	Command Line Arguments to Run
	-----------------------------
	1. Run FTP Server
	~ $ ./ftpServer

	2. Run FTP Client
	~ $ ./ftpClient server_address server_port


*************************************
IV. Changing parameters of the code :
*************************************
1. There will not be much need to change the parameters of the programs. But if needed, several values are defined using the "#define" macro, in the various header files, which can be changed. They are listed as follows :

	- #define CLIENT_PORT 20000 : The client's port
	- #define INITPORT 23456 : Initial port of server
	- #define MAX_CMD_LEN 1024 : Max. length of a command (get, put, cd etc.)
	- #define DEBUG 0 : Can be set to 1 to see additional statements which are otherwise not printed.
	- #define BACKLOG 10 : This is used in the listen function (max. length of queue).
	- #define CONTROL_PORT 8182 : This is the value of the control port for exchanging control information between client and server.
	- #define MAXBUFFSIZE 1024  : This is the maximum length of the buffer

2. Other than these parameters listed, nothing else needs to be changed.


********************
V. Details of Code :
********************

A. General
----------
The general implementation details have been given in the beginning.

B. ftp socket
-------------
This code has been implemented in file tcpSocket.cpp and the configuration of the 
socket is in file tcpSocket.h

C. ftp server
--------------
This code has been implemented in file ftpServer.cpp and the configuration of the 
ftp server is in file tcpSocket.h

D. ftp client
--------------
This code has been implemented in file ftpClient.cpp and the configuration of the 
ftp client is in file tcpSocket.h


*************************
VI. Sample Input/Output :
*************************

A. GENERAL
----------
server_address = 10.4.21.9
server_port = 8182

1. Build the 2 files
INPUT:
	$make

2. Run the ftp server
INPUT:
	$./ftpServer
OUTPUT(on server side):	
	Successfully started on PORT: 8182

3. Now, run the ftp client
INPUT:
	$./ftpClient 10.4.21.9 8182
OUTPUT(on client side):
	Successfully connected to server (control socket): 10.4.21.9
	Successfully started on PORT: 20000
	Successfully connected to server(data port)
	Type 'help' for more info
	

B. help
-------
INPUT:
	ftp> help
OUTPUT(on client side):
	Options available:
	1. !ls
	2. !cd
	3. !pwd
	4. ls
	5. cd
	6. pwd
	7. get
	8. put
	9. quit
	

C. !pwd
--------
INPUT:
	ftp> !pwd
OUTPUT(on client side):
	/home/pirate/Dropbox/PROJECTS/network_lab/assignment3/assignment3 - modified
	
	
D. !cd(directory specified)
---------------------------
INPUT:
	ftp> !cd include
OUTPUT(on client side):
	Changing directory to: include
	Directory successfully changed. 

	
E. !cd(directory NOT specified)
-------------------------------
INPUT:
	ftp> !cd
OUTPUT(on client side):
	Changing directory to: /home/pirate
	Directory successfully changed.
	
	
F. !ls(with arguments)
----------------------
INPUT:
	ftp> !ls -l
OUTPUT(on client side):
	Result is
	total 72
	-rwxr-xr-x 1 pirate pirate 24190 Mar 11 13:40 ftpClient
	-rw-r--r-- 1 pirate pirate  2978 Mar 11 13:52 ftpClient.cpp
	-rwxr-xr-x 1 pirate pirate 24383 Mar 11 13:40 ftpServer
	-rw-r--r-- 1 pirate pirate  2919 Mar 11 13:52 ftpServer.cpp
	drwxr-xr-x 2 pirate pirate  4096 Mar  9 22:02 include
	-rw-r--r-- 1 pirate pirate   158 Mar  8 11:22 makefile
	-rw-r--r-- 1 pirate pirate    86 Mar  8 09:35 README.md
	-rw-r--r-- 1 pirate pirate  3379 Mar 11 10:21 tcpSocket.cpp


G. ls(no arguments)
----------------------
INPUT:
	ftp> ls
OUTPUT(on client side):
	ftpClient
	ftpClient.cpp
	ftpServer
	ftpServer.cpp
	include
	makefile
	README
	README~
	tcpSocket.cpp
	ls successfully executed in server 


H. pwd(with extra whitespaces)
------------------------------
INPUT:
	ftp>                   pwd                  
OUTPUT(on client side):
	/home/pirate/Dropbox/PROJECTS/network_lab/assignment3/assignment3 - modified
	pwd successfully executed in server 
	

I. put(a pdf file)
------------------
INPUT:
	ftp> put Assignment_3.pdf                 
OUTPUT(on client side):
	Size of file transferred : 128.33691 KiB
	File transfer to server successful 
	File successfully received in server


J. get(an image file)
------------------
INPUT:
	ftp> get sample.jpeg                
OUTPUT(on client side):
	Size of file received : 39.71777 KiB
	File successfully received from server 
	File successfully sent from server
