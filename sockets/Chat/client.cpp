#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>

using namespace std;

#define STDIN 0 //Standard input
int main(int argc, char *argv[])
{
	int sockfd, portNr;
	string nickname = "NICK ";
	int valread = 0;
	
	const int bufsize = 256;
	char buffer[bufsize];
	string clientMsg = "";
	struct hostent *server;
	
	struct sockaddr_in server_addr;
	
	//Prepare select()
	fd_set read_fds;
	FD_ZERO(&read_fds);
	struct timeval tv;
	tv.tv_sec = 90;
	int max_sd = 0;
	
	if(argc < 4){
		cout << "Host port nickname" << endl;
		exit(1);
	}	
	server = gethostbyname(argv[1]);
	portNr = atoi(argv[2]);
	nickname += argv[3];
	
	//---Init  socket---//
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout << "Error on creating socket" << endl;
		exit(1);
	}
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
	server_addr.sin_port = htons(portNr);
	//connecting
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		cout << "Error on connecting" << endl;
		exit(1);
	}
	
	if( read(sockfd, buffer, bufsize - 1) < 0)
	{
		cout << "Error reading from socket" << endl;
	}
	cout << buffer << endl;
	
	//Sending nick 
	while(true)
	{
		fflush(NULL);
		memset(buffer, 0, 255); //Reset the buffer arrays
		if( send(sockfd , nickname.c_str() , nickname.size() , 0 ) != nickname.size() ){
			cout << "Error" << endl;
		}
		read(sockfd, buffer, bufsize - 1);
		//Checking if server think nick is ok
		if(buffer[0] == 'O' && buffer[1] == 'K')
			break;
		else{
			cout << buffer << endl;
			cout << "Nickname: ";
			getline(cin, nickname);
			nickname = "NICK " + nickname;
		}
	}
	
	cout << buffer; //Lets the user know that the nick is ok
	memset(buffer, 0, 255); //Reset the buffer arrays
	//Add socket to w8 list
	if(sockfd > STDIN)
		max_sd = sockfd;
	else	
		max_sd = STDIN;
		
	while(true)
	{
		FD_SET(sockfd , &read_fds);
		FD_SET(STDIN, &read_fds);
		
		//Blocking call 
		fflush(NULL);
		if (select( max_sd + 1 , &read_fds , NULL , NULL , &tv) < 0)
			cout << "select error" << endl;
		
		if( FD_ISSET( STDIN, &read_fds) )
		{
			getline(cin, clientMsg);
			clientMsg = "MSG " + clientMsg + "\n";
			if(clientMsg.size() > 0)
			{
				if( send(sockfd , clientMsg.c_str() , clientMsg.size() , 0 ) !=clientMsg.size() ){
					cout << "Error" << endl;
				}
			}
		}	
		else if( FD_ISSET(sockfd, &read_fds) )
		{
			valread = read(sockfd, buffer, bufsize - 1);
			buffer[valread] = '\0';
			cout << buffer;
			memset(buffer, 0, 255); //Reset the buffer arrays
		}
	}
	close(sockfd);
}