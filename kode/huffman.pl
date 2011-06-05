#! /usr/bin/perl -Wall

use strict;


`cat 'lipsum.txt' | ./main '.*' > 'huffman/matchall'`;
`cat 'lipsum.txt' | ./main '(?:(?:(?:[a-zA-Z]+ ?)+[,.;:] ?)*..)*' -d 'debug.pdf' > 'huffman/matchwords'`;
#`cat 'lipsum.txt' | ./main '[0-9]+' > 'huffman/nomatch'`;

do_stats('huffman/matchall');
do_stats('huffman/matchwords');
#do_stats('huffman/nomatch');


sub do_stats {
    my $file = shift(@_);

    open FILE, "<", $file or die $!;
    
    my %count = (
	'0'    => 0,
	'1'    => 0,
	':'    => 0,
	'|'    => 0,
	'='    => 0,
	'\\'   => 0,
	't'    => 0,
	'b'    => 0,
	'char' => 0,
	);
    
    my $is_escape = 0;
    my $length = 0;
    while(my $line = <FILE>){
	my @chars = split("", $line);
	$length += @chars;
	foreach my $char (@chars){
	    if($is_escape){
		$count{'char'} += 1;
		$is_escape = 0;
	    } else {
		if($char eq '\\'){
		    $is_escape = 1;
		}
		$count{$char} += 1;
	    }
	}
    }
    
    print $file;
    printf("0   : %i (%i)\n", ($count{'0'}/$length)*100, $count{'0'});
    printf("1   : %i (%i)\n", ($count{'1'}/$length)*100, $count{'1'});
    printf(":   : %i (%i)\n", ($count{':'}/$length)*100, $count{':'});
    printf("|   : %i (%i)\n", ($count{'|'}/$length)*100, $count{'|'});
    printf("=   : %i (%i)\n", ($count{'='}/$length)*100, $count{'='});
    printf("\\   : %i (%i)\n", ($count{'\\'}/$length)*100, $count{'\\'});
    printf("b   : %i (%i)\n", ($count{'b'}/$length)*100, $count{'b'});
    printf("t   : %i (%i)\n", ($count{'t'}/$length)*100, $count{'t'});
    printf("char: %i (%i)\n", ($count{'char'}/$length)*100, $count{'char'});
    printf("Total: %i\n", $length);
    print '';
}
