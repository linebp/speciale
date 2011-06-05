#! /usr/bin/perl -Wall

use POSIX;
use strict;
use Time::HiRes 'time';


my $startTime, my $endTime, my $result;

my $i, my $j;
my $best;
my $n;

for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    `echo -n '$text' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='backtrackingworstcase/main.$n.out' ./main '$regex' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='backtrackingworstcase/ismatch.$n.out' ./ismatch`;

}

for($i = 1; $i <= 10; $i++){
    $n = ceil($i * 2.5);
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='backtrackingworstcase/perl.$n.out' ./perlmatchonly.pl '$regex' '$text'`;
}

for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='backtrackingworstcase/re2.$n.out' ./re2matchonly '$regex' '$text'`;
}


for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "a?" x $n . "a" x $n;
    my $text = "a" x $n;

    `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='backtrackingworstcase/tcl.$n.out' ./tclmatchonly.tcl '$regex' '$text'`;
}


