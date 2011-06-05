#! /usr/bin/perl -Wall

use POSIX;
use strict;
use Time::HiRes 'time';


my $startTime, my $endTime, my $result;

my $runs = 10;
my $i, my $j;
my $best;
my $n;

print "main";
print "   n   sec";
for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    for ($j = 0; $j < $runs; $j++){
	$startTime = time();
	$result = `echo -n  '$text' | ./main '$regex' | ./ismatch`;
	$endTime = time();
	
	if($j == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    chomp $result;
    print "ARGH" unless $result eq 't';
    printf("%4i %.3f\n", $n, $best);
}

print "perl";
print "   n   sec";
for($i = 1; $i <= 10; $i++){
    $n = ceil($i * 2.5);
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    for ($j = 0; $j < $runs; $j++){
	$startTime = time();
	$result = `./perlmatchonly.pl '$regex' '$text'`;
	$endTime = time();
	
	if($j == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    chomp $result;
    print "ARGH" unless $result eq 't';
    printf("%4i %.3f\n", $n, $best);
}

print "re2";
print "   n   sec";
for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    for ($j = 0; $j < $runs; $j++){
	$startTime = time();
	$result = `./re2matchonly '$regex' '$text'`;
	$endTime = time();
	
	if($j == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    chomp $result;
    print "ARGH" unless $result eq 't';
    printf("%4i %.3f\n", $n, $best);
}


print "tcl";
print "   n   sec";
for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    for ($j = 0; $j < $runs; $j++){
	$startTime = time();
	$result = `./tclmatchonly.tcl '$regex' '$text'`;
	$endTime = time();
	
	if($j == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    chomp $result;
    print "ARGH" unless $result eq 't';
    printf("%4i %.3f\n", $n, $best);
}


