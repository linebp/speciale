#! /usr/bin/perl -Wall

use strict;
use Time::HiRes 'time';


my $regex = ".*<B>((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)</B>.*";
my $regex2 = "|((?:(?:(?:<(?:/B<|/<|<)*(?:/B[^<>]|/[^<B]|[^</]))|[^<])*)[</B]*)";
#my $regex = "[^<]*<B>([^<]*)</B>[^<]*";
#my $regex2 = "|([^<]*)";
my $startTime, my $endTime, my $result;

my $n = 10; 

my $text = "<B>Integer mattis, felis at ornare semper, tortor est egestas nisl, sit amet imperdiet sapien lectus sollicitudin erat. Nunc dignissim blandit ligula, nec elementum nisi pharetra et. Aenean enim dolor, sodales non imperdiet varius, auctor id velit. Donec eu dolor quis turpis hendrerit euismod volutpat sit amet eros. Fusce et dui lorem, eget ultricies tortor. Integer quis viverra ipsum. Donec mollis luctus molestie. Donec quis eros non mi euismod commodo a ut neque. Aliquam non dui orci. Fusce vehicula, arcu vitae hendrerit blandit, odio ligula fringilla ligula, ut feugiat lorem eros eget leo. Donec non erat at metus interdum sodales consequat a neque. Aenean a est arcu. Maecenas commodo mi a ante iaculis in placerat metus tincidunt. Nam cursus sem quis eros egestas a cursus tellus interdum. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Duis congue molestie eros ut dictum. Curabitur vulputate, erat vitae egestas ultricies, dolor ipsum scelerisque nisl, eu aliquam turpis elit ut lorem. Aenean vel nibh leo. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Sed vel erat tellus, sit amet feugiat augue.</B>.";

open(LIPSUM, "lipsum.txt");
my $lipsum = "<B>" . join('', <LIPSUM>) . "</B>";

#$text = "dignissim <B>leo</B>.";

#$text = "<B>leo</B>";

my $runs = 200;

$startTime = time();
#`bash -c "for run in {1..$runs}; do ./main '$regex' '$text' | ./groupings_all '$regex' | ./trace | ./serialize '$regex2'; done"`;
#$result = `valgrind --tool=massif ./main '$regex' '$text' | valgrind --tool=massif ./groupings_all '$regex' | valgrind --tool=massif ./trace | valgrind --tool=massif ./serialize '$regex2'`;
#$result = `valgrind --tool=callgrind --simulate-cache=yes ./main '$regex' '$text' | valgrind --tool=callgrind --simulate-cache=yes ./groupings_all '$regex' | valgrind --tool=callgrind --simulate-cache=yes ./trace | valgrind --tool=callgrind --simulate-cache=yes ./serialize '$regex2'`;
#`./main '$regex' '$lipsum' > hat.mbv`;
$result = `./main '$regex' '$text' | ./groupings_all '$regex'  | ./trace | ./serialize '$regex2'`;
#valgrind --leak-check=yes
$endTime = time();
#print "main: $result";
printf("Main: takes %.3f seconds.\n", $endTime - $startTime);

$startTime = time();
#`bash -c "for run in {1..$runs}; do ./foo.pl '$regex' '$text'; done"`;
#$result = `valgrind --tool=massif ./foo.pl '$regex' '$text'`;
$endTime = time();
#print "perl: $result";
printf("perl: takes %.3f seconds.\n", $endTime - $startTime);

$startTime = time();
#`bash -c "for run in {1..$runs}; do ./foo '$regex' '$text'; done"`;
#`valgrind --tool=massif ./foo '$regex' '$text'`;
$endTime = time();
#print "re2: $result";
printf("re2: takes %.3f seconds.\n", $endTime - $startTime);


