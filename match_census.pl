#!/usr/bin/perl

$census_params_file = 'qr_census_manifolds_up_to_area_6';
if ($#ARGV > -1) {
    $census_params_file = $ARGV[0];
}
while (<STDIN>) {
	if (/(.*) ([10]+)$/) { 
        my $box = $2;
        my $grep_match = `grep ' $box' $census_params_file`; 
        my @manifolds = ();
        if (length($grep_match) > 0) {
            my @census_data = split(/\n/, $grep_match); 
            foreach (@census_data) {
                my ($mfld) = /M (\S+) .*/;
                push(@manifolds, $mfld);
            }
            print "$1 $2 " . join(" ", @manifolds) . "\n";		
        } else {
            print "$_";
        }
    } else {
        print "$_";
    }
}
