#! /usr/bin/perl -Wall

use strict;

my $nf, my $im, my $gs, my $ga, my $tr;

if($ARGV[0] =~ /simple/){
    $nf = 1;
}
if($ARGV[0] =~ /ismatch/){
    $im = 1;
}
if($ARGV[0] =~ /groupings_all/){
    $ga = 1;
}
if($ARGV[0] =~ /groupings_single/){
    $gs = 1;
}
if($ARGV[0] =~ /trace/){
    $tr = 1;
}
if($ARGV[0] =~ /^all$/){
    $gs = 1;
    $nf = 1;
    $im = 1;
    $ga = 1;
    $tr = 1;
}

open FILE, "<", "testhat.csv" or die $!;

my $regex, my $text, my $result;
my $nf_exp, my $im_exp, my $gs_exp, my $ga_exp, my $tr_exp;
my $i = 0;

while(<FILE>){
    $i++;
    chomp;

    if(/#/){
	print;
	print "";
	next;
    }

    if(/^\s*$/){
	#print "";
	next;
    }

    ($regex, $text, $nf_exp, $im_exp, $ga_exp, $gs_exp, $tr_exp) = split /\t/;

    if($nf){
	$result = `echo -n '$text' | ./main '$regex'`;
	print "nf $i: Wrong match: $regex $text got $result expected $nf_exp" unless $result eq $nf_exp;
	#print $result;
    }

    if($im){
	$result = `echo -n '$text' | ./main '$regex' 2>/dev/null | ./ismatch`;
	chomp $result;
	print "im $i: Wrong match: $regex $text got $result expected $im_exp" unless $result eq $im_exp;
    }

    if($ga){
	$result = `echo -n '$text' | ./main '$regex' 2>/dev/null | ./groupings_all '$regex'`;
	chomp $result;
	print "ga $i: Wrong match: $regex $text got $result expected $ga_exp" unless $result eq $ga_exp;
	#print $result;
    }

    if($gs){
	$result = `echo -n '$text' | ./main '$regex' 2>/dev/null | ./groupings_single '$regex'`;
	chomp $result;
	print "gs $i: Wrong match: $regex $text got $result expected $gs_exp" unless $result eq $gs_exp;
	#print $result;
    }

    if($tr){
	$result = `echo -n '$text' | ./main '$regex' 2>/dev/null | ./trace`;
	chomp $result;
	print "tr $i: Wrong match: $regex $text got $result expected $tr_exp" unless $result eq $tr_exp;
	#print $result;
    }
}
