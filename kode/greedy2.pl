#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';


my $regex = ".*<B>((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)</B>.*";
my $regex2 = "|((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)";
my $startTime, my $endTime, my $result;

open(LIPSUM, "lipsum.txt");
my $lipsum = "<B>" . join('', <LIPSUM>) . "</B>";

# Generate the files
`./main '$regex' '$lipsum' > main.mbv`;
`./groupings_all '$regex' <  main.mbv > groupings_all.mbv`;
`./trace < groupings_all.mbv > trace.mbv`;
`./serialize '$regex2' <  trace.mbv > serialize.mbv`;

my $runs = 10;
my $i;
my $best;
my $first;

$first = 1;
for ($i = 0; $i < $runs; $i++){
    $startTime = time();
    `./main '$regex' '$lipsum'`;
    $endTime = time();

    if($first || ($endTime - $startTime) < $best){
	$first = 0;
	$best = $endTime - $startTime;
    }
}

printf("Main: takes %.3f seconds.\n", $best);

$first = 1;
for ($i = 0; $i < $runs; $i++){
    $startTime = time();
    `./groupings_all '$regex' < main.mbv`;
    $endTime = time();

    if($first || ($endTime - $startTime) < $best){
	$first = 0;
	$best = $endTime - $startTime;
    }
}

printf("groupings_all: takes %.3f seconds.\n", $best);

$first = 1;
for ($i = 0; $i < $runs; $i++){
    $startTime = time();
    `./trace < groupings_all.mbv`;
    $endTime = time();

    if($first || ($endTime - $startTime) < $best){
	$first = 0;
	$best = $endTime - $startTime;
    }
}

printf("trace: takes %.3f seconds.\n", $best);

$first = 1;
for ($i = 0; $i < $runs; $i++){
    $startTime = time();
    `./serialize '$regex2' < trace.mbv`;
    $endTime = time();

    if($first || ($endTime - $startTime) < $best){
	$first = 0;
	$best = $endTime - $startTime;
    }
}

printf("serialize: takes %.3f seconds.\n", $best);
