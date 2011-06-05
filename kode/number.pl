#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';

my $regex = '([+-]?(?:[0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)(?:[eE][+-]?[0-9]+)?)';
my $regex2 = '|([+-]?(?:[0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)(?:[eE][+-]?[0-9]+)?)';
my $startTime, my $endTime, my $result;

my $n;

my @number;
my $runs = 1;
my $i, my $j;
my $best;
my $factor = 1000;

for($j = 1; $j <= 10; $j++){
    $n = $factor * $j;
    $number[$j] = '+' . int(rand($n)) x $n . '.' . int(rand($n)) x $n . 'E-' . int(rand($n)) x $n;
}

print "main";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./main '$regex' '$number[$j]' | ./groupings_all '$regex' | ./trace | ./serialize '$regex2' `;
	$endTime = time();
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    print "ARGH" unless $result eq $number[$j];
    printf("%7i %.3f\n", length($number[$j]), $best);
}


print "tcl";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./tclmatch.tcl '$regex' '$number[$j]'`;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    chomp($result);
    print "ARGH" unless $result eq $number[$j];
    printf("%7i %.3f\n", length($number[$j]), $best);
}


print "re2";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./re2match '$regex' '$number[$j]'`;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    print "ARGH" unless $result eq $number[$j];
    printf("%7i %.3f\n", length($number[$j]), $best);
}


print "perl";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./perlmatch.pl '$regex' '$number[$j]'`;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    print "ARGH" unless $result eq $number[$j];
    printf("%7i %.3f\n", length($number[$j]), $best);
}
