#include "Socket.h"

Socket::Socket(string hostname, int port){
	this->hostname=hostname;
	this->port=port;
}

#ifdef __linux__

int Socket::opens(){
	int sockfd, len;
	struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(hostname.c_str()); /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */

    /*open an TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /*connect to the server*/
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
   		return -1;

    socketfd=sockfd;

    return 0;
}

int Socket::writes(string msg){
	const char *buf = msg.c_str();
	return write(socketfd,buf,strlen(buf));
}

int Socket::reads(string &msg){
	char *buf = new char[256];
	int len = read(socketfd,buf,256);
	msg = buf;
	return len;
}

 void Socket::closes(){
 	close(socketfd);
 }

#else

int Socket::opens(){// Initialize Winsock.
	SOCKET m_socket;
    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != NO_ERROR)
		printf("Client: Error at WSAStartup().\n");
    else
        printf("Client: WSAStartup() is OK.\n");

	// Create a socket.
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        printf("Client: socket() - Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
	else
       printf("Client: socket() is OK.\n");

    // Connect to a server.
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    // Just test using the localhost, you can try other IP address
    clientService.sin_addr.s_addr = inet_addr(hostname.c_str());
    clientService.sin_port = htons(port);

    if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        printf("Client: connect() - Failed to connect.\n");
        WSACleanup();
        return -1;
    }
    else {
       printf("Client: connect() is OK.\n");
       printf("Client: Can start sending and receiving data...\n");
    }

    socketfd=m_socket;

    // Send and receive data.
	printf("Connected\n");

	return 0;
}

int Socket::writes(string msg){
	const char *buf = msg.c_str();
	return send(socketfd, buf, strlen(buf)+1, 0);
}

int Socket::reads(string &msg){
	char *buf = new char[256];
	int len = recv(socketfd, buf, 256, 0);
	buf[len] = '\0';
	msg = (string)buf;
	return len;
}

 void Socket::closes(){
	closesocket(socketfd);
 }

#endif
