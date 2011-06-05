#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';


open(LIPSUM, "lipsum.txt");
my $lipsum = "<B>" . join('', <LIPSUM>) . "</B>";

my $regex = ".*<B>((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)</B>.*";
my $regex2 = "|((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)";
my $startTime, my $endTime, my $result;

my $runs = 5;
my $i;
my $best;

for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    $result = `echo '$lipsum' | ./main '$regex' | ./groupings_all '$regex' | ./trace | ./serialize '$regex2'`;
    $endTime = time();

    if($i == 0 || ($endTime - $startTime) < $best){
	$best = $endTime - $startTime;
    }
}

print "hatten er fin" if $lipsum eq "<B>$result</B>";
printf("Takes %.3f seconds.\n", $best);
