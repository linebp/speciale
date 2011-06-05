#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';


my $regex = ".*<B>((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)</B>.*";
my $regex2 = "|((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)";
my $startTime, my $endTime, my $result;

open(LIPSUM, "lipsum.txt");
my $lipsum = "<B>" . join('', <LIPSUM>) . "</B>";

# Generate the files
#`./main '$regex' '$lipsum' > main.mbv`;
#`./groupings_all '$regex' <  main.mbv > groupings_all.mbv`;
#`./trace < groupings_all.mbv > trace.mbv`;
#`./serialize '$regex2' <  trace.mbv > serialize.mbv`;

`valgrind --tool=massif --stacks=yes ./main '$regex' '$lipsum'`;
`valgrind --tool=massif --stacks=yes ./groupings_all '$regex' < main.mbv`;
`valgrind --tool=massif --stacks=yes ./trace < groupings_all.mbv`;
`valgrind --tool=massif --stacks=yes ./serialize '$regex2' < trace.mbv`;
