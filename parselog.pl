#!/usr/bin/perl
# Parses a log and generates some useless statistics.
# $Amigan: cidserv/parselog.pl,v 1.1 2004/12/23 23:46:01 dcp1990 Exp $
# (C)2004 Dan Ponte. BSD.
$logfile = $ARGV[0];
%stat = ('me' => (phone => '123345', times => 1,),);
%numstat = (123345 => \$stat{'me'});
open(LF, $logfile);
@cont = <LF>;
close(LF);
print "parselog.pl v1.\n";
foreach(@cont) {
	if($_ =~ /^Phone Number: ([0-9]+)$/) {
		$stat{$lnam}{'phone'} = $1;
		$numstat{$1} = \$stat{$lnam};
	} elsif ($_ =~ /^Name: (.+)$/) {
		$stat{$1}{'times'}++;
		$tottimes++;
		$stat{$1}{'name'} = $1;
		$lnam = $1;
	}
}
while(($k, $v) = each(%stat)) {
#	print $k . "\n";
	while(($ok, $ov) = each(%{$v})) {
		if($ok eq 'times' and $ov > $lg) { $lg = $ov; $hn = ${$v}{'phone'}; }
#		print $ok . ' = ' . $ov . "\n";
	}
}
#print "be " . ${${$numstat{$hn}}}{'times'} . "\n";
print ${${$numstat{$hn}}}{'name'} . " ($hn) called the most times, at $lg.\n";
print "In total, people called here $tottimes times.\n";
