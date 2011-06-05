#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';

my $regex = '([+-]?(?:[0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)(?:[eE][+-]?[0-9]+)?)';
my $regex2 = '|([+-]?(?:[0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)(?:[eE][+-]?[0-9]+)?)';

my $n;

my @number;
my $i, my $j;
my $factor = 1000;

for($j = 1; $j <= 10; $j++){
    $n = $factor * $j;
    $number[$j] = '+' . int(rand($n)) x $n . '.' . int(rand($n)) x $n . 'E-' . int(rand($n)) x $n;
}

print "         main groupings_all         trace     serialize";
for($j = 1; $j <= 10; $j++){
    `echo -n '$number[$j]' | ./main '$regex' > main.mbv.tmp`;
    `./groupings_all '$regex' < main.mbv.tmp > groupings_all.mbv.tmp`;
    `./trace < groupings_all.mbv.tmp > trace.mbv.tmp`;
    `./serialize '$regex2' < trace.mbv.tmp > serialize.mbv.tmp`;
    

    printf("%13i %13i %13i %13i\n", -s 'main.mbv.tmp',
	   -s 'groupings_all.mbv.tmp', -s 'trace.mbv.tmp',
	   -s 'serialize.mbv.tmp');
}
