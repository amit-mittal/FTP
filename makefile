CC=g++
FLAGS=Wall

myclient: ftpClient.cpp myserver
	$(CC) -FLAGS ftpClient.cpp -o ftpClient

myserver: ftpServer.cpp
	$(CC) -FLAGS ftpServer.cpp -o ftpServer