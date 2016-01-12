#!/usr/bin/perl
# For each qr word lists and counts all boxes wehre it is a qr
sub g_power
{
    my ($gs) = @_;
    $gs =~ s/[nNmM]+//g;
    length $gs 
}

%word_count = ();
%words_to_boxes = ();
%boxes_to_sorted_words = ();
%boxes_to_shortest_words = ();

$shortest_only = 'F';
if ($ARGV[0] eq '-s') {
    $shortest_only = 'T';
}

while (<STDIN>) {
	if (/HOLE ([01]*) \((.*)\)/) {
		$box = $1;
		@words = split(/,/, $2);
		foreach $word (@words) {
			if (!defined $boxes_to_sorted_words{$box}) {
                my @words = sort {&g_power($a) <=> &g_power($b)} @words;
                my $min_power = &g_power($words[0]);
                my @short_list = grep {&g_power($_) <= $min_power} @words;
				$boxes_to_sorted_words{$box} = \@words;
                $boxes_to_shortest_words{$box} = \@short_list;
			}
            if ($shortest_only eq 'T' && ! grep {$_ eq $word} @{$boxes_to_shortest_words{$box}}) {
                next;
            }
			if (!defined $word_count{$word}) {
				$word_count{$word} = 0;
			}
            $word_count{$word} += 1;
			if (!defined $words_to_boxes{$word}) {
                my @boxes = ();
				$words_to_boxes{$word} = \@boxes;
			}
            push(@{$words_to_boxes{$word}}, $box);
        }
    }
}

foreach $word (sort {$word_count{$b} <=> $word_count{$a}} (keys(%word_count))) {
    my @boxes = sort @{$words_to_boxes{$word}};
    my $g_power = &g_power($word);
    print "$word has power $g_power and passes through $word_count{$word} boxes:\n";
    foreach $box (@boxes) {
        my @box_words = @{$boxes_to_sorted_words{$box}};
        my ($idx) = grep { $box_words[$_] eq $word } 0 .. $#box_words;
        if ($g_power == &g_power($box_words[0])) {
            $idx = "shortest:$idx";
        }
        print "    $word $g_power $idx $box\n";
    }
}
