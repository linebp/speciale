#! /usr/bin/perl -Wall

use strict;

my $regex = '(.*)';
my $regex2 = '|(.*)';

# Generate the files
`cat lipsum.txt | ./main '$regex' > ~/speciale/memory.main.mbv`;
`./groupings_all '$regex' <  ~/speciale/memory.main.mbv > ~/speciale/memory.groupings_all.mbv`;
`./trace < ~/speciale/memory.groupings_all.mbv > ~/speciale/memory.trace.mbv`;
`./serialize '$regex2' <  ~/speciale/memory.trace.mbv > ~/speciale/memory.serialize.mbv`;

# Collect memory usage data using massif
`cat lipsum.txt | valgrind --tool=massif --stacks=yes ./main '$regex'`;
`valgrind --tool=massif --stacks=yes ./ismatch < ~/speciale/memory.main.mbv`;
`valgrind --tool=massif --stacks=yes ./groupings_all '$regex' < ~/speciale/memory.main.mbv`;
`valgrind --tool=massif --stacks=yes ./trace2 < ~/speciale/memory.groupings_all.mbv`;
`valgrind --tool=massif --stacks=yes ./trace < ~/speciale/memory.groupings_all.mbv`;
`valgrind --tool=massif --stacks=yes ./serialize '$regex2' < ~/speciale/memory.trace.mbv`;
