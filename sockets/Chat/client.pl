#!/usr/bin/perl

use warnings;
use strict;
use IO::Socket::INET;
use IO::Select;

my($ipAdress, $port, $nickname) = @ARGV;
if(@ARGV != 3)
{
	print "3 argument should be sent: ipAdress port nickname\n";
	exit; 
}
else
{
my $read_set = IO::Select->new();
$read_set->add(\*STDIN);



my ($socket, $clientSocket);
my ($data, $post);

#Creating TCP SOCKET
$socket = new IO::Socket::INET(
	PeerHost => $ipAdress,
	PeerPort => $port,
	Proto => 'tcp'
) or die "Error in socket creaton\n";

$read_set ->add($socket) or die "IO::Select";

$socket->recv($data, 256);
print "$data";

#Trying to get nickname
$socket->send("NICK $nickname\n");
$socket->recv($data, 256);
while($data !~/^OK/)
{
	print "$data";
	print "Nickname: ";
	$nickname = <STDIN>;
	
	$socket->send("NICK $nickname\n");
	$socket->recv($data, 256);
}	
print "$data";
while(1)
{
	my @readyClients = $read_set->can_read();
	
	foreach my $sel (@readyClients)
	{
		#See if there is a userinput waiting
		if($sel == \*STDIN) 
		{
			$post = <STDIN>;
			$socket->send("MSG $post\n");
			
		}
		elsif($sel == $socket) #See if there is info from the socket
		{
			$socket->recv($data, 256);
			print "$data";
		}
		else
		{		
			print "Error occured\n";
			die; 
		}
	}
}
	
$socket->close();
}
