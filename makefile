CC=g++ -std=c++11
FLAGS=-w

myclient: ftpClient.cpp myserver
	$(CC) $(FLAGS) ftpClient.cpp -o ftpClient

myserver: ftpServer.cpp
	$(CC) $(FLAGS) ftpServer.cpp -o ftpServer

clean: 
	-rm -f *.o 
	-rm ./ftpClient
	-rm ./ftpServer
