/*
	SNTP Server
*/
#include <stdio.h> //printf
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <string>
#include <cstdlib>

#define MAXLEN 1024  //Max length of buffer

void die(char *s)
{
    perror(s);
    exit(1);
}

struct ntpMsg
{
	uint8_t settings;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	uint32_t rootDelay;
	uint32_t rootDisp;
	uint32_t refID;
	uint32_t refTimeS;
	uint32_t refTimeF;
	uint32_t orgTimeS;
	uint32_t orgTimeF;
	uint32_t recvTimeS;
	uint32_t recvTimeF;
	uint32_t transTimeS;
	uint32_t transTimeF;	
};

uint32_t ClockGetTime() {

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint32_t)ts.tv_sec * 1000000LL
		+ (uint32_t)ts.tv_nsec / 1000LL;
}
 
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "1 argument:  port " << std::endl;
		exit(1);
	}
	int portNr = atoi(argv[1]);
	
	struct ntpMsg msg = {};
	struct sockaddr_in sockaddr_local;
    struct sockaddr_in sockaddr_remote;
    int socket_handle;
    socklen_t slen = sizeof(sockaddr_remote);
    uint32_t timestamp;
    
    if ((socket_handle=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Unable to create socket");
        return 0;
    }
    
    memset((char *) &sockaddr_local, 0, sizeof(sockaddr_local));
    sockaddr_local.sin_family = AF_INET;
    sockaddr_local.sin_port = htons(portNr);
    sockaddr_local.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socket_handle, (struct sockaddr*) &sockaddr_local, sizeof(sockaddr_local)) == -1)
    {
        perror("Unable to bind socket");
        return 0;
    }
	
	while(1)
	{
		std::cout << "Waiting for message on port " << portNr << std::endl;
		std::cout << "The server will sleep 1 second before timing and 1 second befor transmitting" << std::endl;
		
		if (recvfrom(socket_handle, &msg, sizeof(msg), 0, (struct sockaddr*) &sockaddr_remote, &slen) == -1)
        {
            perror("Unable to receive packet");
            return 0;
        }
		
		std::cout <<  "\n***---SNTP Result---***\n" << std::endl;
		
		
		std::cout << "Client recive Timestamp Integer: " << ntohl(msg.transTimeS) << std::endl;
		std::cout << "Client recive Timestamp Fraction: " << ntohl(msg.transTimeF) << std::endl << std::endl; 

		//Get start time
		sleep(1);
		time_t time_rcv_sec = time(NULL);
		uint32_t timeS = time_rcv_sec + 2208988800;
		uint32_t timeF = ClockGetTime();
		
		//Respons message
		msg.settings = 0b11100100;
		msg.orgTimeS = msg.transTimeS;
		msg.orgTimeF = msg.transTimeF;
		msg.recvTimeS = htonl(timeS);
		msg.recvTimeF = htonl(timeF);
		
		//get send time
		sleep(1);
		time_rcv_sec = time(NULL);
		timeS = time_rcv_sec + 2208988800;
		timeF = ClockGetTime();
		
		msg.transTimeS = htonl(timeS);
		msg.transTimeF = htonl(timeF);
		
		//send the message
		if (sendto(socket_handle, &msg, sizeof(msg), 0, (struct sockaddr*) &sockaddr_remote, slen) == -1)
        {
            perror("Unable to send packet");
            return 0;
        }
		//close(s);
    }
	return 0;
}