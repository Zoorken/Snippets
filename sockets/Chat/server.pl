#!/usr/bin/perl

use warnings;
use strict;
use IO::Socket::INET;
use IO::Select;
use IO::Pipe;

#Variables:
my($port) = @ARGV;

my ($socket, $clientSocket);
my ($clientAddr, $clientPort);
my ($data, $readHand, @readyClients, $clientSendTo, $outMSG);
my (%nicknames);

#Creating TCP SOCKET
$socket = new IO::Socket::INET(
	LocalHost => '0.0.0.0',
	LocalPort => $port,
	Proto =>'tcp',
	Listen => 3,
	Reuse => 1
) or die "Error in socket creaton\n";

#Make ready for multiple
$readHand = IO::Select->new($socket) or die "IO::Select";
print "Server is waiting for clients to connect to port $port ok\n";

#This is where the clients will connect
while(1)
{
	@readyClients = $readHand->can_read();
	
	foreach my $client (@readyClients)
	{
		if($client == $socket)
		{
			#add the new client
			my $newSock = $socket->accept();
			$readHand->add($newSock);
			print $newSock "Hello V3\n";
		}
		else
		{
			#$data = <$client>;
			$client->recv($data, 256);
			if(length($data) == 256)
			{
				$client->recv(my $data2, 5);
				if(length($data2) != 0)
				{
					$client->send("To long, max 256 charters\n");
				}
			}
			
			# Print message to server
			if($data)
			{
				chomp($data);
			
				if($data =~s/^NICK //)
				{	
					if(length($data) > 12)
					{
						$outMSG = "ERROR max 12 chars\n";
					}
					elsif($data =~/^[a-zA-Z0-9_]+$/)
					{	
						if(exists $nicknames{$client})
						{
							$outMSG = "ERROR $nicknames{$client}\n"
						}
						else 
						{
							#Nick is ok
							chomp($data);
							$nicknames{$client} = $data; #Store the nickname
							$outMSG = "OK Nick\n";
						}
					}
					else
					{
						$outMSG = "ERROR only alphanumerics and underscore\n";		
					}
					$client->send($outMSG);	
				}
				elsif($data =~s/^MSG //)
				{
				
					if(exists $nicknames{$client})
					{
						print "$nicknames{$client} : $data";
				
						# send message to user
						my @sockets = $readHand->can_write();
						foreach my $sendTo (@sockets)
						{
							$sendTo->send("MSG $nicknames{$client} $data")
							#print $sendTo "$nicknames{$client}: $data";
						}
					}
					else 
					{
						print $client "ERROR Get yourself a nickname next time: NICK nick\n";
					}
				}
				else
				{	
					if(exists $nicknames{$client})
					{
						print $client "ERROR Send MSG first \n";
					}
					else
					{
						print $client "ERROR Send NICK nickname first \n";
					}
				}
			}
			else
			{
				#Remove nickname
				if(exists $nicknames{$client})
				{
					print "Disconnecting $nicknames{$client}\n";
					$readHand->remove($client);
				}
				else
				{
					print "Removing a lurker\n";
				}
				close($client);
			}	
			
		}			
	}
}

$socket->close();



