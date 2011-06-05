#! /usr/bin/perl -Wall

use strict;

if($#ARGV < 0){
    simple();
}
else {
    if($ARGV[0] =~ /simple/){
	simple();
    }
    if($ARGV[0] =~ /groupings_star/){
	groupings_star();
    }
    if($ARGV[0] =~ /groupings_nostar/){
	groupings_nostar();
    }
}

sub groupings_star
{
    open FILE, "<", "groupings_star.txt" or die $!;
    
    my $regex, my $text, my $exp_result, my $result;

    while(<FILE>){
	chomp;

	if(/#/){
	    print;
	    next;
	}

	if(/^$/){
	    next;
	}

	($regex, $text, $exp_result) = split /\t/;
	
	$result = `./main $regex $text | ./groupings_star $regex`;
	chomp $result;
    
	print "Wrong match: $regex $text got $result expected $exp_result" unless $result eq $exp_result;
    }
}

sub groupings_nostar
{
    open FILE, "<", "groupings_nostar.txt" or die $!;
    
    my $regex, my $text, my $exp_result, my $result;

    while(<FILE>){
	chomp;

	if(/#/){
	    print;
	    next;
	}

	if(/^$/){
	    next;
	}

	($regex, $text, $exp_result) = split /\t/;
	
	$result = `./main $regex $text | ./groupings_nostar $regex`;
	chomp $result;
    
	print "Wrong match: $regex $text got $result expected $exp_result" unless $result eq $exp_result;
    }
}

sub simple
{
    open FILE, "<", "simple_tests.txt" or die $!;
    
    my $regex, my $text, my $exp_result, my $result;

    while(<FILE>){
	chomp;

	if(/#/){
	    print;
	    next;
	}

	if(/^$/){
	    next;
	}

	($regex, $text, $exp_result) = split /\t/;
	
	$result = `./main $regex $text | ./ismatch 2>/dev/null`;
	chomp $result;
    
	print "Wrong match: $regex $text got $result expected $exp_result" unless $result eq $exp_result;
    }
}
