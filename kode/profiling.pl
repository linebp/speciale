#! /usr/bin/perl -W

use strict;


my $regex = '(.*)';
my $regex2 = '|(.*)';


`cat ~/speciale/xac | ./main '(.*)'`;
`mv gmon.out gmon.main.out`;
`cat ~/speciale/xac | ./main '(.*)' | ./groupings_all '(.*)'`;
`mv gmon.out gmon.groupings_all.out`;
`cat ~/speciale/shorttrace | ./main '(.*)' | ./groupings_all '(.*)' | ./trace2`;
`mv gmon.out gmon.trace.out`;
`cat ~/speciale/xac | ./main '(.*)' | ./groupings_all '(.*)' | ./trace | ./serialize '|(.*)'`;
`mv gmon.out gmon.serialize.out`;
