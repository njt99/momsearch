# For each qr word lists and counts all boxes wehre it is a qr
sub g_power
{
    my ($gs) = @_;
    $gs =~ s/[nNmN]+//g;
    length $gs 
}

%word_count = ();
%words_to_boxes = ();
%boxes_to_words = ();
while (<>) {
	if (/HOLE ([01]*) \((.*)\)/) {
		$box = $1;
		@words = split(/,/, $2);
		foreach $word (@words) {
			if (!defined $word_count{$word}) {
				$word_count{$word} = 0;
			}
            $word_count{$word} += 1;
			if (!defined $words_to_boxes{$word}) {
                my @boxes = ();
				$words_to_boxes{$word} = \@boxes;
			}
            push(@{$words_to_boxes{$word}}, $box);
			if (!defined $boxes_to_words{$box}) {
                my @words = ();
				$boxes_to_words{$box} = \@words;
			}
            push(@{$boxes_to_words{$box}}, $word);
        }
    }
}
%boxes_visited = ();
foreach $word (sort {$word_count{$b} <=> $word_count{$a}} (keys(%word_count))) {
    my @boxes = sort @{$words_to_boxes{$word}};
    my $g_power = &g_power($word);
    print "$word has power $g_power and passes through $word_count{$word} boxes:\n";
    foreach $box (@boxes) {
        if (!defined $boxes_visited{$box}) {
            my @sorted = sort {&g_power($a) <=> &g_power($b)} @{$boxes_to_words{$box}};
            $boxes_to_words{$box} = \@sorted;
            $boxes_visited{$box} = 1;
        }
        my @box_words = @{$boxes_to_words{$box}};
        my ($idx) = grep { $box_words[$_] ~~ $word } 0 .. $#box_words; 
        print "    $word $g_power $idx $box\n";
    }
}
