#!/usr/bin/perl 
use strict;
use warnings;
use IO::Socket;
use Time::HiRes qw(time);

#UDP Client

my($socket, $msg);
my($server, $PORTNO) = @ARGV; 
my $MAXLEN = 1024;
if(@ARGV != 2)
{
	print "Two arguments should be sent: ip port\n";
}
else
{
$socket = IO::Socket::INET->new(
	PeerPort =>$PORTNO,
	PeerAddr =>$server,
	Proto => 'udp') or die "Creating socket\n";

#Header

$msg = pack("B8 C3 N9", '00100011', (0)x12);	
#Content Transmit Timestamp	
my $localTimeStart = time(); #Get timestamp befor query
my $localTimeNTP = $localTimeStart + 2208988800; # convert it to NTP timestamp 

#Convert time to NTP format
my $localTimeNTP_F = $localTimeNTP - int($localTimeNTP); #Fraction of timestamp
$localTimeNTP_F = sprintf("%.0f",$localTimeNTP_F * 10000000000);
print "--------Sending---------\n";
print "localTimeNTP: $localTimeNTP\n";
print "localTimeNTP_F: $localTimeNTP_F\n\n";

###$msg .= pack("N N N N2 N2 N2 N N", 0,0,0,0,0,0, int($localTimeNTP), int($localTimeNTP_F)); 
$msg .= pack("N N", int($localTimeNTP), int($localTimeNTP_F));

$socket->send($msg) or die "Sent to server: $msg\n";

$socket->recv($msg, $MAXLEN) or die "recv: $!";
#Measure timestamp after recv

my $localTimeEnd = time();

#Unpack from server 
my($head, $cSTRATUM, $cPOLL, $cPRECISION, $cROOTDELAY, $cROOTDISPERSION, $cID, $cREFTIME, $cREFTIME_F, $cORIGTIME, $cORIGTIME_F, $cRECTIME, $cRECTIME_F, $cTRANSTIME_NTP, $cTRANSTIME_F) = unpack("a c c c N N a4 N2 N N N N N N", $msg); 

#unpack header
my $cLI = unpack("C", $head & "\xC0") >> 6;
my $cVN = unpack("C", $head & "\x38") >> 3;
my $cMODE = unpack("C", $head & "\x07");

#What server sent
print "cCLI: $cLI\n";
print "cVN: $cVN\n";
print "cMode: $cMODE\n";
#print "cSTRATUM: $cSTRATUM\n";
#print "cPoll: $cPOLL\n";
#print "cPRECISION: $cPRECISION\n";
#print "cROOTDELAY: $cROOTDELAY\n";
#print "cROOTDISPERSION: $cROOTDISPERSION\n";
#print "cID: $cID\n";
#print "cREFTIME: $cREFTIME\n";
#print "cREFTIME: $cREFTIME_F\n";
#print "cORIGTIME: $cORIGTIME\n";
#print "cORIGTIME_f: $cORIGTIME_F\n";
#print "cRECTIME: $cRECTIME\n";
#print "cRECTIME_F: $cRECTIME_F\n";
#print "cTRANSTIME_NTP: $cTRANSTIME_NTP\n";
#print "cTRANSTIME_FB: $cTRANSTIME_F\n";

#Calculate localTime End
$localTimeEnd = $localTimeEnd + 2208988800;
#Convert it to right format
my $localTimeEnd_F = $localTimeEnd - int($localTimeEnd); #Fraction of timestamp
$localTimeEnd = int($localTimeEnd);
$localTimeEnd_F = sprintf("%.0f",$localTimeEnd_F * 10000000000);

print "\n***---SNTP Result---***\n";
print "Originate Timestamp Integer: $cORIGTIME\n";
print "Originate Timestamp Fraction: $cORIGTIME_F\n\n";

print "Server Receive Timestamp Integer: $cRECTIME\n";
print "Server Receive Timestamp Fraction: $cRECTIME_F\n\n";

print "Server Transmit Timestamp Integer: $cTRANSTIME_NTP\n";
print "Server Transmit Timestamp Fraction: $cTRANSTIME_F\n\n";

print "Client Receive Timestamp Integer: $localTimeEnd\n";
print "Client Receive Timestamp Fraction: $localTimeEnd_F\n";
print "\n***---SNTP Result---***\n";
}