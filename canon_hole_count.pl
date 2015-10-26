%canonCount = ();
%canonBoxes = ();
while (<>) {
	if (/HOLE ([01]*) \((.*)\)/) {
		$box = $1;
		@words = split(/,/, $2);
		foreach $word (@words) {
			if (!defined $canonCount{$word}) {
				$canonCount{$word} = 0;
			}
            $canonCount{$word} += 1;
			if (!defined $canonBoxes{$word}) {
                my @boxes = ();
				$canonBoxes{$word} = \@boxes;
			}
            push(@{$canonBoxes{$word}}, $box);
        }
    }
}
foreach $word (sort {$canonCount{$b} <=> $canonCount{$a}} (keys(%canonCount))) {
    my @boxes = sort @{$canonBoxes{$word}};
    print "$word $canonCount{$word} with boxes:\n";
    foreach $box (@boxes) {
        print "    $box\n";
    }
}
