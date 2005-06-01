#!/usr/bin/perl
# Parses a log and generates some useless statistics.
# $Amigan: cidserv/parselog.pl,v 1.2 2005/06/01 00:13:20 dcp1990 Exp $
# (C)2004 Dan Ponte. BSD.
$logfile = $ARGV[0];
#%stat = ('me' => (phone => '123345', times => 1,),);
#%numstat = (123345 => \$stat{'me'});
open(LF, $logfile);
print "parselog.pl v2.\n";
foreach(<LF>) {
	if($_ =~ /^..:..:..: L[0-9]{2}\/[0-9]{2} (....):(....):(.+):(.+)$/) {
		my ($dat, $tim, $nam, $phn) = ($1, $2, $3, $4);
		$stat{$nam}{'name'} = $nam;
		$stat{$nam}{'phone'} = $phn;
		$numstat{$phn} = \$stat{$nam};
		$stat{$nam}{'times'}++;
		$tottimes++;
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
print "In tottal, people called here $tottimes times.\n";
close(LF)
