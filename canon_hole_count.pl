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
$unique = 'F';
for $opt (@ARGV) {
    $shortest_only = 'T' if ($opt eq '-s');
    $unique = 'T' if ($opt eq '-u');
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

%ids_already_printed = ();
foreach $word (sort {$word_count{$b} <=> $word_count{$a}} (keys(%word_count))) {
    my @ids = sort @{$words_to_ids{$word}};
    my $g_power = &g_power($word);
    my $id_count = 0;
    my $printout = "";
    foreach $id (@ids) {
        my @id_words = @{$ids_to_sorted_words{$id}};
        my ($idx) = grep { $id_words[$_] eq $word } 0 .. $#id_words;
        if ($g_power == &g_power($id_words[0])) {
            $idx = "shortest:$idx";
        }
        if (!defined $ids_already_printed{$id}) {
            $id_count += 1;
            $ids_already_printed{$id} = 1;
            $printout .= "    $word $g_power $idx $id\n";
        }
        elsif ($unique eq 'F') {
            $id_cout +=1;
            $printout .= "    $word $g_power $idx $id less_popular\n";
        }
    }
    if ($id_count > 0) {
        print "$word has power $g_power and passes through $id_count ids:\n$printout";
    }
}
