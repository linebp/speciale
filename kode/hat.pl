#! /usr/bin/perl

use strict;
use Time::HiRes 'time';

#use re Debug => 'PARSE';

my $startTime, my $endTime, my $result;

open(LIPSUM, "lipsum.txt");
my $lipsum = join('', <LIPSUM>);

$lipsum =~ s/\./\\./g;

$startTime = time();
$result = `./main '$lipsum' '$lipsum' | ./trace`;
$endTime = time();

printf("trace: takes %.3f seconds.\n", $endTime - $startTime);

$startTime = time();
$result = `./main '$lipsum' '$lipsum' | ./trace2`;
$endTime = time();

printf("trace2: takes %.3f seconds.\n", $endTime - $startTime);
