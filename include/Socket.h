#ifndef SOCKET_H
#define SOCKET_H
/****************************************************************************
 * Author:      - André Freitas, p.andrefreitas@gmail.com
 * Author:      - Paulo Araújo, paulojmaraujo@gmail.com
 * Copyright:   - 14/10/2012, LAIG, FEUP
 *****************************************************************************/
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#ifdef __LINUX__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#else

#include <winsock2.h>
#include <ctype.h>

#endif

using namespace std;

/*
	Socket - A class for creating a socket to a remote computer in internet
*/
 class Socket{
 protected:
 	string hostname;
 	int port;
 	int socketfd;
 public:
 	Socket(string hostname, int port);
 	int opens();
 	int writes(string msg);
 	int reads(string &msg);
 	void closes();
 	~Socket(){
 		closes();
 	}
 };

 #endif
