#! /usr/bin/perl -Wall

use POSIX;
use strict;
use Time::HiRes 'time';


my $result;

my $i, my $j;
my $n;

for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "(a|b)*a" . "(a|b)" x $n;
    my $text = "a" . "a" x $n;

    $result = `echo -n '$text' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='dfaworstcase/main.$n.out' ./main '$regex' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='dfaworstcase/ismatch.$n.out' ./ismatch`;
    chomp $result;
    print "ARGH" unless $result eq 't';
}

for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "(a|b)*a" . "(a|b)" x $n;
    my $text = "a" . "a" x $n;

    $result = `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='dfaworstcase/perl.$n.out' ./perlmatchonly.pl '$regex' '$text'`;

    chomp $result;
    print "ARGH" unless $result eq 't';
}

for($i = 1; $i <= 10; $i++){
    $n = $i * 500;
    my $regex = "(a|b)*a" . "(a|b)" x $n;
    my $text = "a" . "a" x $n;

    $result = `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='dfaworstcase/re2.$n.out' ./re2matchonly '$regex' '$text'`;

    chomp $result;
    print "ARGH" unless $result eq 't';
}

for($i = 1; $i <= 10; $i++){
    $n = $i * 60;
    my $regex = "(a|b)*a" . "(a|b)" x $n;
    my $text = "a" . "a" x $n;

    $result = `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='dfaworstcase/tcl.$n.out' ./tclmatchonly.tcl '$regex' '$text'`;

    chomp $result;
    print "ARGH" unless $result eq 't';
}
