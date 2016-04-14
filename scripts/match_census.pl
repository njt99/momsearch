#!/usr/bin/perl

$census_params_file = 'qr_census_manifolds_up_to_area_6';
if ($#ARGV > -1) {
    $census_params_file = $ARGV[0];
}

open my $fp, $census_params_file or die "Could not open $census_params_file";
chomp (my @census_params = <$fp>);
close $fp;

while (<STDIN>) {
	if (/^(.*) ([10]+)$/) {
        my $orig = "$1 $2"; 
        my $box = $2;
        my @census_data = grep(/.* $box.*/, @census_params); 
        my @manifolds = ();
        if ($#census_data > -1) {
            foreach (@census_data) {
                my ($mfld) = /M (\S+) .*/;
                push(@manifolds, $mfld);
            }
            print "$1 $2 " . join(" ", @manifolds) . "\n";		
        } else {
            print "$orig\n";
        }
    } else {
        print "$_";
    }
}
