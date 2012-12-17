#include "Socket.h"

Socket::Socket(string hostname, int port){
	this->hostname=hostname;
	this->port=port;
}

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
	int len=read(socketfd,buf,256);
	msg=buf;
	return len;
}

 void Socket::closes(){
 	close(socketfd);
 }

/*
int main(){
	Socket s1("127.0.0.1",6300); 
	cout << s1.opens() << endl;

	s1.writes("[beginGame,[bot2,0],[bot1,0],2,newBoard].\n");
	string msg;
	while(s1.reads(msg)) cout << msg;
	
	return 0;
}
*/