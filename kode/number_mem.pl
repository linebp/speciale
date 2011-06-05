#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';

my $regex = '([+-]?(?:[0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)(?:[eE][+-]?[0-9]+)?)';
my $regex2 = '|([+-]?(?:[0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)(?:[eE][+-]?[0-9]+)?)';
my $startTime, my $endTime, my $result;

my $n;

my @number;
my $i, my $j;
my $factor = 1000;

for($j = 1; $j <= 10; $j++){
    $n = $factor * $j;
    $number[$j] = '+' . int(rand($n)) x $n . '.' . int(rand($n)) x $n . 'E-' . int(rand($n)) x $n;
}

for($j = 1; $j <= 10; $j++){
    my $n = length($number[$j]);
    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/main.$n.out' ./main '$regex' '$number[$j]' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/groupings_all.$n.out' ./groupings_all '$regex' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/trace.$n.out' ./trace | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/serialize.$n.out' ./serialize '$regex2' `;
}


for($j = 1; $j <= 10; $j++){
    my $n = length($number[$j]);
    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/tcl.$n.out' ./tclmatch.tcl '$regex' '$number[$j]'`;
}


for($j = 1; $j <= 10; $j++){
    my $n = length($number[$j]);
    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/re2.$n.out' ./re2match '$regex' '$number[$j]'`;
}


for($j = 1; $j <= 10; $j++){
    my $n = length($number[$j]);
    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='number/perl.$n.out' ./perlmatch.pl '$regex' '$number[$j]'`;
}
