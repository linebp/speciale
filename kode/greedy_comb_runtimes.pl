#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';


my $regex = ".*<B>((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)</B>.*";
my $regex2 = "|((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)";
my $startTime, my $endTime, my $result;

open(LIPSUM, "lipsum.txt");
my $lipsum = "<B>" . join('', <LIPSUM>) . "</B>";

my $runs = 10;
my $i;
my $best;
my $first;

$first = 1;
for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    $result = `./main '$regex' '$lipsum' | ./groupings_all '$regex' | ./trace | ./serialize '$regex2'`;
    $endTime = time();

    if($first || ($endTime - $startTime) < $best){
	$first = 0;
	$best = $endTime - $startTime;
    }
}

print "hatten er fin" if $lipsum eq "<B>$result</B>";
printf("Takes %.3f seconds.\n", $best);
