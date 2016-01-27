#!/usr/bin/perl 
use strict;
use warnings;
use IO::Socket;
use Time::HiRes qw(time);

#UDP SERVER

my($socket, $msg, $clientMsg, $clientAddr);
my $PORTNO = 123;
my $MAXLEN = 1024;

$socket = IO::Socket::INET->new(
	LocalPort => $PORTNO,
	Proto => 'udp') or die "Could not create socket";
	
print "Waiting for UDP message on port $PORTNO\n";
       
while($socket->recv($clientMsg, $MAXLEN))
{
	my $localTimeStart = time(); #Get Recive Timestamp
	
	my($port, $ipaddr) = sockaddr_in($socket->peername);
	
							
	my($head, $cSTRATUM, $cPOLL, $cPRECISION, $cROOTDELAY, $cROOTDISPERSION, $cID, $cREFTIME, $cREFTIME_F, $cORIGTIME, $cORIGTIME_F, $cRECTIME, $cRECTIME_F, $cTRANSTIME_NTP, $cTRANSTIME_FB) = unpack("a c c c N N a4 N2 N N N N N N", $clientMsg); 
	#Unpack the head that is a string with arbitrary binary data
	my $cLI = unpack("C", $head & "\xC0") >> 6;
	my $cVN = unpack("C", $head & "\x38") >> 3;
	my $cMODE = unpack("C", $head & "\x07");
	
	#print "***---Client sent---***\n";
	#print "LI: $cLI\n";
	#print "VN: $cVN\n";
	#print "Mode: $cMODE\n";
	#print "Originate Timestamp\n";
	#print "cTRANSTIME_NTP: $cTRANSTIME_NTP\n";
	#print "cTRANSTIME_FB: $cTRANSTIME_FB\n";
	
	if( $cLI == 3 or $cLI == 0 ){
		$head = '11100'; # LI, Unicast server respons VN = 4; 
		$cSTRATUM = 1; #To show that it is a server 
		if($cMODE eq "3") #Client mode 
		{
			$head .= '100';
		} 
		elsif($cMODE == "001") #Symmetric active
		{	
			$head .= '010';
		}
		else
		{
			print "Client mode is: $cMODE which is invalid\n";
			next; #Skips this request
		}
		
		my $msg = pack("B8 c c c", $head,  $cSTRATUM, $cPOLL, $cPRECISION);
		
		$msg .= pack("N N", $cROOTDELAY, $cROOTDISPERSION);
		$msg .= pack("a4","LOCL"); #uncalibrated local clock
		$msg .= pack("N2", 0); #Reference timestamp
		$msg .= pack("N N", $cTRANSTIME_NTP, $cTRANSTIME_FB), #Originate Timestamp

		#Recv timestamp
		my $localTimeNTP = $localTimeStart + 2208988800; # convert it to NTP timestamp 
		#Convert to right format
		my $localTimeNTP_F = $localTimeNTP - int($localTimeNTP); #Fraction of timestamp
		$localTimeNTP_F = sprintf("%.0f",$localTimeNTP_F * 10000000000);
		
		$msg .= pack("N N", int($localTimeNTP), int($localTimeNTP_F));
		
		#Transmit timestamp	
		my $localTimeSend = time(); 
		my $localTimeSend_NTP = $localTimeSend + 2208988800; # convert it to NTP timestamp 
		#Convert to right format
		my $localTimeSend_NTP_F = $localTimeSend_NTP - int($localTimeSend_NTP);
		$localTimeSend_NTP_F = sprintf("%.0f",$localTimeSend_NTP_F * 10000000000);
		
		$msg .= pack("N N", int($localTimeSend_NTP), int($localTimeSend_NTP_F));
		$socket->send($msg);
	} 
	else
	{
		print "Client sent: $cLI , It should be 0\n";	
	}
}