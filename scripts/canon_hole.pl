#!/usr/bin/perl
# Given a line out of refine HOLE err file output with qr's, this will
# outputs the HOLE, box and canonical versions of the qr's

require 'qr_tools.pl';

while (<>) {
	if (/(.*)(HOLE) ([01]*) (.*)\((.*)\)/ || /(.*)(M) (\S+) (.*)\((.*)\)/) {
        $type = $2;
		$id = $3;
        $info = $4;
		@words = split(/,/, $5);
		%canonWords = ();
		%canonVersion = ();
		foreach $word (@words) {
			if (!defined $canonVersion{$word}) {
				my @v = &versions($word);
				$canonVersion{$word} = $v[0];
			}
			$canonWords{$canonVersion{$word}} = 1;
		}
		@canonWords = sort {&g_power($a) <=> &g_power($b)} keys %canonWords;
		print "$type $id $info(" . join(",", @canonWords) . ")\n";
	}
}
