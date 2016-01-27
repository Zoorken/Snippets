#include <stdio.h>
#include <string.h>  
#include <string>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   
#include <arpa/inet.h>   
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <regex>
  
#define TRUE   1
#define FALSE  0
#define PORT 8888

bool notAlphaNummeric(std::string string)
{
	int i = 0;
	for(char & c : string)
	{
		if(!isalnum(c))
		{
			std::cout << i << std::endl;
			std::cout << c << std::endl;
			return true;
		}
		i++;
	}
	return false;
}
 
int main(int argc , char *argv[])
{
	const std::regex regMSG("\\b(MSG)([^ ]*)");
	const std::regex regNICK("\\b(NICK)([^ ]*)");
	std::string tempMSG = "";
	std::string tempReg = "";
	
	const int max_clients = 10;
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[max_clients], activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
	std::string clientName[max_clients] = {};
    std::string temp;
    char buffer[256];
	
	if(argc < 2){
		std::cout << "port" << std::endl;
		exit(1);
	}
	int portNr = atoi(argv[1]);
      
    //set of socket descriptors
    fd_set readfds;
      
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) 
    {
        client_socket[i] = 0;
    }
      
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
  
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portNr);
      
    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", portNr);
     
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
      
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
     
    while(TRUE) 
    {
		
        //clear the socket set
        FD_ZERO(&readfds);
  
        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
         
        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++) 
        {
            //socket descriptor
            sd = client_socket[i];
             
            //if ok socket add 
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //need highest socket for select
            if(sd > max_sd)
                max_sd = sd;
        }
	
  
         //W8 for sockets
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
          
        
        if (FD_ISSET(master_socket, &readfds)) 
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        
            //send new connection
			tempMSG = "Hello V1.1\n";
            if( send(new_socket, tempMSG.c_str(), tempMSG.size(), 0) != tempMSG.size() ) 
            {
                perror("send");
            }
              
            std::cout << "Welcome message sent successfully\n";	
			
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    std::cout << "New connection" << std::endl;
					break;
                }
            }
        }
          
        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 256)) == 0)
                {
					//Disconnecting
                    std::cout << clientName[i] << " left" << std::endl;
                      
                    //Close the socket and mark as 0 in list for reuse
                    FD_CLR(client_socket[i] , &readfds);
					close( sd );
					clientName[i] = ""; //removes nickname
                    client_socket[i] = 0;
                }
                  
                //Send out incoming message
                else
                {
                    std::cout << valread << std::endl;
					buffer[valread] = '\0';
					temp = std::string(buffer);
					
					//handles the message data
					temp.erase(temp.find_last_not_of(" \n\r\t")+1);
					if(std::regex_search(buffer, regMSG)){		
						//Only if the user has a nick
						if(clientName[i] != "")
						{
							temp = std::string(buffer);
							temp.erase(0,4);
							tempMSG = "MSG " + clientName[i] + " " + temp;
							if(temp.back() != '\n'){
								tempMSG += "\n";
							}
							std::cout << tempMSG;
							if(valread > 255)
							{
								tempMSG = "ERROR To long\n";
								send(client_socket[i] , tempMSG.c_str() , tempMSG.size() , 0 );
							}
							
							for(int k = 0; k < max_sd; k++)
							{
								if(clientName[k] != "") //Not sending to lurkers
									send(client_socket[k] , tempMSG.c_str() , tempMSG.size() , 0 );
							}
							memset(buffer, 0, 255); //Should not be needed 
							tempMSG = ""; //Should not be needed 
						}
						else
						{
							tempMSG = "ERROR Nick you need\n";
							send(client_socket[i] , tempMSG.c_str() , tempMSG.size() , 0 );
						}
					}
					else if(std::regex_search(buffer, regNICK)) //Checking user nick
					{
						temp.erase(0,5);
						if(temp.size() > 12)
						{
							tempMSG = "ERROR Nick to long\n";
						}
						else if(notAlphaNummeric(temp))
						{
							tempMSG = "ERROR only alphaNummeric\n";
							std::cout << "ERROR only alphaNummeric: " << temp;
						}
						else
						{
							if(clientName[i] != "")
							{
								tempMSG = "ERROR Nick you have!\n";
							}
							else
							{
								clientName[i] = temp;
								tempMSG = "OK\n";
							}
						}
						
						send(client_socket[i] , tempMSG.c_str() , tempMSG.size() , 0 );
					}
					else 
					{
						tempMSG = "ERROR send MSG or NICK\n";
						send(client_socket[i] , tempMSG.c_str() , tempMSG.size() , 0 );
					}
				}
				memset(buffer, 0, 255); //Reset the buffer arrays
            }
        }
    }
    return 0;
}