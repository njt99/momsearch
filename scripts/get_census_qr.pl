#!/usr/bin/perl
# Reads a file from the get_params_from_manifold output in sage
# and outputs the associated quasirelators to each manifold
# M name=m003_0_2
# M vol=2.02988321281931
# M cusp_area=3.46410161513775458705489268301174473388561050762076125611161396
# M flips=['IS']
# M is_special=True
# M lattice_might_be_norm_one=True
# M possibly_on_box_edge=True
# M box=111101000010000000010010100101100011000101110100110000110101010000110100100000010000100010110001010000000100100010110011
# M box=110101000010000000010010100101100011000101110100110000110101010000110100100000010000100010110001010000000100100010110011
# lattice = 0.500000000....
# lox_sqrt = -1.7320508....
# parabolic = 0.5000000....

require 'qr_tools.pl';

$refine_dir = 'refine_census';
$area_bnd = 5.9;

while (<>) {
	if (/M (.*)=(.*)/ || /(.*) = (.*)/) {
		@boxes = () if ($1 eq 'name'); 
		$name = $2 if ($1 eq 'name'); 
		$vol = $2 if ($1 eq 'vol'); 
		$cusp_area = $2 if ($1 eq 'cusp_area'); 
        push(@boxes, $2) if ($1 eq 'box');

        if ($1 eq 'lattice') {
            for $box (@boxes) {
                &get_quasi_relators;
            }
        }
	}
}

sub get_quasi_relators {
    my $refine_code = `./refine_box_clean $name $area_bnd $box`; 
    my $err_file = "$refine_dir/$name.err";
    my $out_file = "$refine_dir/$name.out";
    my $canon_file = "$refine_dir/$name.holes";
    my $canon_code = `perl canon_hole.pl < $err_file > $canon_file`;
    my $hole_and_var_count = `grep "HOLE\\\|V" $out_file | wc -l | bc`;
    chomp $hole_and_var_count;

    open my $all_words, '>>', "../words" or die "Could not open words file";
    open my $err, $err_file or die "Could not open $err_file";
    while (<$err>) {
        if (/search .* found (.*)\(.*\)/) {
            my @v = &versions($1);
            say $all_words $v[0];
        }   
    }
    close $all_words;

    my %quasi_relators = ();
    open my $out, $out_file or die "Could not open $out_file";
    while (<$out>) {
        if (/V\((.+)\)/) {
            my @v = &versions($1);
            $quasi_relators{$v[0]} = 1;
        }   
    }
    open my $canon, $canon_file or die "Could not open $canon_file";
    while (<$canon>) {
        if (/HOLE .* \((.+)\)/) {
		    @words = split(/,/, $1);
            for $word (@words) {
                $quasi_relators{$word} = 1;
            }
        }   
    }
    @qr = sort keys %quasi_relators;
    if ($hole_and_var_count > 0) {
        if (@qr == 0) {
            if ($cusp_area <= $area_bnd) {
                print STDERR "$name : no quasirelators found (INSIDE area bound)!\n";
            } else {
                print STDERR "$name : no quasirelators found (outside area bound)!\n";
            }
        }
        print "M $name vol=$vol area=$cusp_area $box (" . join(",", @qr) . ")\n";
    } else {
        # We expect some boxes to be eliminated becase we might have manifolds whose coordinates appeared on edges
        print "M $name vol=$vol area=$cusp_area $box ELIMINATED\n";
    }
}
