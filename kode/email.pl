#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';
use String::Random;


my $regex = '([A-Za-z0-9](?:(?:[_.-]?[a-zA-Z0-9]+)*)@(?:[A-Za-z0-9]+)(?:(?:[.-]?[a-zA-Z0-9]+)*)\.(?:[A-Za-z][A-Za-z]+))';
my $regex2 = '|([A-Za-z0-9](?:(?:[_.-]?[a-zA-Z0-9]+)*)@(?:[A-Za-z0-9]+)(?:(?:[.-]?[a-zA-Z0-9]+)*)\.(?:[A-Za-z][A-Za-z]+))';
my $startTime, my $endTime, my $result;

my $foo = new String::Random;
$foo->{'A'} = [ 'A'..'Z', 'a'..'z', '0'..'9' ];
$foo->{'B'} = [ '_', '.', '-' ];
$foo->{'D'} = [ '.', '-' ];
$foo->{'E'} = [ 'A'..'Z', 'a'..'z' ];
$foo->{'F'} = [ '@' ];
$foo->{'G'} = [ '.' ];

my $n;

#my $email = $foo->randpattern("A" . ("B" x int(rand(2)) . "A" x int(rand($n))) x int(rand($n)) . "FA" . ("D" x int(rand(2)) . "A" x int(rand($n))) x int(rand($n)) . "GEE" . "E" x int(rand($n)));
#my $email = $foo->randpattern("A" . ("B" . ("A" x $n)) x $n . "FA" . "D" . (("A" x $n) x $n) . "GEE" . ("E" x $n));
my @email;
my $runs = 10;
my $i, my $j;
my $best;
my $factor = 25;

for($i = 1; $i <= 10; $i++){
    $n = $i * $factor;
    $email[$i] = $foo->randpattern("A" . ("B" . ("A" x $n)) x $n . "FA" . "D" . (("A" x $n) x $n) . "GEE" . ("E" x $n));
}


print "main";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `echo -n '$email[$j]' | ./main '$regex' | ./groupings_all '$regex'  | ./trace | ./serialize '$regex2' `;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    print "ARGH" unless $result eq $email[$j];
    printf("%7i %.3f\n", length($email[$j]), $best);
}


print "tcl";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./tclmatch.tcl '$regex' '$email[$j]'`;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    chomp($result);
    print "ARGH" unless $result eq $email[$j];
    printf("%7i %.3f\n", length($email[$j]), $best);
}


print "re2";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./re2match '$regex' '$email[$j]'`;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    print "ARGH" unless $result eq $email[$j];
    printf("%7i %.3f\n", length($email[$j]), $best);
}


print "perl";
print "    len   sec";
for($j = 1; $j <= 10; $j++){
    for($i = 0; $i < $runs; $i++){
	$startTime = time();
	$result = `./perlmatch.pl '$regex' '$email[$j]'`;
	$endTime = time();
	
	if($i == 0 || ($endTime - $startTime) < $best){
	    $best = $endTime - $startTime;
	}
    }
    print "ARGH" unless $result eq $email[$j];
    printf("%7i %.3f\n", length($email[$j]), $best);
}
