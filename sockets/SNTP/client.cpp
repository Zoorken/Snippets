/*
	SNTP Client
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<iostream>
#include <time.h>
#include <sys/timeb.h>
//#include <vector>

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
	if (argc != 3)
	{
		std::cout << "2 argument: ip port " << std::endl;
		exit(1);
	}
	int portNr = atoi(argv[2]);
	
	struct ntpMsg respons = {};
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
	struct ntpMsg request = {}; //Initate with zeros
	
	//Initate the message
	request.settings = 0b00100011;
	time_t time_rcv_sec = time(NULL);
	uint32_t startTimeS = time_rcv_sec + 2208988800;
	uint32_t startTimeF = ClockGetTime();
	request.transTimeS = htonl(startTimeS); //safe to transfer
	request.transTimeF = htonl(startTimeF); //Safe for transfer
	
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(portNr);
     
    if (inet_aton(argv[1] , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

	//send the message
	if (sendto(s, &request, sizeof(request) , 0 , (struct sockaddr *) &si_other, slen)==-1)
	{
		die("sendto()");
	}
	 
	//try to receive some data, this is a blocking call
	if (recvfrom(s, &respons, sizeof(respons), 0, (struct sockaddr *) &si_other, &slen) == -1)
	{
		die("recvfrom()");
	}
	//Get end time
	time_rcv_sec = time(NULL);
	startTimeS = time_rcv_sec + 2208988800;
	startTimeF = ClockGetTime();
	
	std::cout <<  "\n***---SNTP Result---***\n" << std::endl;
	
	std::cout << "Originate Timestamp Integer: " << ntohl(respons.orgTimeS) << std::endl;
	std::cout << "Originate Timestamp Fraction: " << ntohl(respons.orgTimeF) << std::endl << std::endl;
	
	std::cout << "Server Receive Timestamp Integer: " << ntohl(respons.recvTimeS) << std::endl;
	std::cout << "Server Receive Timestamp Fraction: " << ntohl(respons.recvTimeF) << std::endl << std::endl;
	
	std::cout << "Server Transmit Timestamp Integer: " << ntohl(respons.transTimeS) << std::endl;
	std::cout << "Server Transmit Timestamp Fraction: " << ntohl(respons.transTimeS) << std::endl << std::endl; 
	
	std::cout << "Client recive Timestamp Integer: " << startTimeS << std::endl;
	std::cout << "Client recive Timestamp Fraction: " <<startTimeF << std::endl << std::endl; 
	//std::cout << respons.recvTimeS;
    //close(s);
    return 0;
}