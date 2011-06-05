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

my @email;
my $runs = 1;
my $i, my $j;
my $best;
my $factor = 25;

for($i = 1; $i <= 10; $i++){
    $n = $i * $factor;
    $email[$i] = $foo->randpattern("A" . ("B" . ("A" x $n)) x $n . "FA" . "D" . (("A" x $n) x $n) . "GEE" . ("E" x $n));
}


print "         main groupings_all         trace     serialize";
for($j = 1; $j <= 10; $j++){
    my $n = length($email[$j]);
    `echo -n '$email[$j]' | ./main '$regex' > main.mbv.tmp`;
    `./groupings_all '$regex' < main.mbv.tmp > groupings_all.mbv.tmp`;
    `./trace < groupings_all.mbv.tmp > trace.mbv.tmp`;
    `./serialize '$regex2' < trace.mbv.tmp > serialize.mbv.tmp`;
    

    printf("%13i %13i %13i %13i\n", -s 'main.mbv.tmp',
	   -s 'groupings_all.mbv.tmp', -s 'trace.mbv.tmp',
	   -s 'serialize.mbv.tmp');
}
