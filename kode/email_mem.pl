#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';
use String::Random;


my $regex = '([A-Za-z0-9](?:(?:[_.-]?[a-zA-Z0-9]+)*)@(?:[A-Za-z0-9]+)(?:(?:[.-]?[a-zA-Z0-9]+)*)\.(?:[A-Za-z][A-Za-z]+))';
my $regex2 = '|([A-Za-z0-9](?:(?:[_.-]?[a-zA-Z0-9]+)*)@(?:[A-Za-z0-9]+)(?:(?:[.-]?[a-zA-Z0-9]+)*)\.(?:[A-Za-z][A-Za-z]+))';
my $result;

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
my $i, my $j;
my $factor = 25;

for($i = 1; $i <= 10; $i++){
    $n = $i * $factor;
    $email[$i] = $foo->randpattern("A" . ("B" . ("A" x $n)) x $n . "FA" . "D" . (("A" x $n) x $n) . "GEE" . ("E" x $n));
}

for($j = 1; $j <= 10; $j++){
    my $n = length($email[$j]);
    $result = `echo -n '$email[$j]' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/main.$n.out' ./main '$regex' | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/groupings_all.$n.out' ./groupings_all '$regex'  | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/trace.$n.out' ./trace | valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/serialize.$n.out' ./serialize '$regex2' `;
    print "ARGH" unless $result eq $email[$j];
}


for($j = 1; $j <= 10; $j++){
    my $n = length($email[$j]);
    $result = `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/tcl.$n.out' ./tclmatch.tcl '$regex' '$email[$j]'`;
    chomp($result);
    print "ARGH" unless $result eq $email[$j];
}


for($j = 1; $j <= 10; $j++){
    my $n = length($email[$j]);
    $result = `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/re2.$n.out' ./re2match '$regex' '$email[$j]'`;
    print "ARGH" unless $result eq $email[$j];
}


for($j = 1; $j <= 10; $j++){
    my $n = length($email[$j]);
    $result = `valgrind --tool=massif --pages-as-heap=yes --massif-out-file='email/perl.$n.out' ./perlmatch.pl '$regex' '$email[$j]'`;
    print "ARGH" unless $result eq $email[$j];
}
