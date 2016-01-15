#!/usr/bin/perl
# For each qr word lists and counts all ids wehre it is a qr
sub g_power
{
    my ($gs) = @_;
    $gs =~ s/[nNmM]+//g;
    length $gs 
}

%word_count = ();
%words_to_ids = ();
%ids_to_sorted_words = ();
%ids_to_shortest_words = ();

$shortest_only = 'F';
if ($ARGV[0] eq '-s') {
    $shortest_only = 'T';
}

while (<STDIN>) {
	if (/(.*)(HOLE) ([01]*) .*\((.*)\)/ || /(.*)(M) (\S+) .*\((.*)\)/) {
        $type = $2;
		$id = $3;
		@words = split(/,/, $4);
		foreach $word (@words) {
			if (!defined $ids_to_sorted_words{$id}) {
                my @words = sort {&g_power($a) <=> &g_power($b)} @words;
                my $min_power = &g_power($words[0]);
                my @short_list = grep {&g_power($_) <= $min_power} @words;
				$ids_to_sorted_words{$id} = \@words;
                $ids_to_shortest_words{$id} = \@short_list;
			}
            if ($shortest_only eq 'T' && ! grep {$_ eq $word} @{$ids_to_shortest_words{$id}}) {
                next;
            }
			if (!defined $word_count{$word}) {
				$word_count{$word} = 0;
			}
            $word_count{$word} += 1;
			if (!defined $words_to_ids{$word}) {
                my @ids = ();
				$words_to_ids{$word} = \@ids;
			}
            push(@{$words_to_ids{$word}}, $id);
        }
    }
}

foreach $word (sort {$word_count{$b} <=> $word_count{$a}} (keys(%word_count))) {
    my @ids = sort @{$words_to_ids{$word}};
    my $g_power = &g_power($word);
    print "$word has power $g_power and passes through $word_count{$word} ids:\n";
    foreach $id (@ids) {
        my @id_words = @{$ids_to_sorted_words{$id}};
        my ($idx) = grep { $id_words[$_] eq $word } 0 .. $#id_words;
        if ($g_power == &g_power($id_words[0])) {
            $idx = "shortest:$idx";
        }
        print "    $word $g_power $idx $id\n";
    }
}
