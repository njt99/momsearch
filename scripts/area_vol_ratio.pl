#!/usr/bin/perl
# For each qr word lists and counts all ids wehre it is a qr
while (<STDIN>) {
	if (/(.*)(M) (\S+) (.*)\((.*)\)/) {
        my $type = $2;
		my $id = $3;
        my $info_whole = $4;
        my $qrs = $5;
        my @info = split(/ /, $4);
        my ($vol) = $info[0] =~ /vol=(\S+)/; 
        my ($area) = $info[1] =~ /area=(\S+)/;
        my $area_over_vol = $area/$vol;
        print "$type $id $info_whole ($qrs) area_over_vol=$area_over_vol\n"; 
    }
}
