#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';


my $regex = "(.*)";
my $regex2 = "|(.*)";
my $startTime, my $endTime, my $result;

# Generate the files
`cat ~/speciale/shorttrace | ./main '$regex' > ~/speciale/runtime.main.mbv`;
`./groupings_all '$regex' <  ~/speciale/runtime.main.mbv > ~/speciale/runtime.groupings_all.mbv`;
`./trace < ~/speciale/runtime.groupings_all.mbv > ~/speciale/runtime.trace.mbv`;
`./serialize '$regex2' <  ~/speciale/runtime.trace.mbv > ~/speciale/runtime.serialize.mbv`;

my $runs = 5;
my $i;
my $best;

for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    `cat ~/speciale/shorttrace | ./main '$regex'`;
    $endTime = time();

    if($i == 0 || ($endTime - $startTime) < $best){
	$best = $endTime - $startTime;
    }
}

printf("Main: takes %.3f seconds.\n", $best);

for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    `./groupings_all '$regex' <  ~/speciale/runtime.main.mbv`;
    $endTime = time();

    if($i == 0 || ($endTime - $startTime) < $best){
	$best = $endTime - $startTime;
    }
}

printf("groupings_all: takes %.3f seconds.\n", $best);

for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    `./ismatch <  ~/speciale/runtime.main.mbv`;
    $endTime = time();

    if($i == 0 || ($endTime - $startTime) < $best){
	$best = $endTime - $startTime;
    }
}

printf("ismatch: takes %.3f seconds.\n", $best);


for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    `./trace <  ~/speciale/runtime.groupings_all.mbv`;
    $endTime = time();

    if($i == 0 || ($endTime - $startTime) < $best){
	$best = $endTime - $startTime;
    }
}

printf("trace: takes %.3f seconds.\n", $best);


$startTime = time();
`./trace2 <  ~/speciale/runtime.groupings_all.mbv`;
$endTime = time();

$best = $endTime - $startTime;

printf("trace2: takes %.3f seconds.\n", $best);

for ($i = 0; $i < $runs; $i++){
    print $i;
    $startTime = time();
    `./serialize '$regex2' <  ~/speciale/runtime.trace.mbv`;
    $endTime = time();

    if($i == 0 || ($endTime - $startTime) < $best){
	$best = $endTime - $startTime;
    }
}

printf("serialize: takes %.3f seconds.\n", $best);

