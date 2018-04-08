#!/usr/bin/perl

$err_dir = '/home/users/ayarmola/mom_search/momsearch/output';
$bin_dir = '/home/users/ayarmola/mom_search/momsearch/bin';
$script_dir = '/home/users/ayarmola/mom_search/momsearch/scripts';

system('find' . " $err_dir " . '-type f -name \*err -exec grep VAR {} + > holes_err');
system("$bin_dir/treecat " . '--all_holes -r' . " $err_dir " . '\'\' > holes');

open(my $fp, 'holes') or die 'Could not open holes';
chomp (my @open_holes = <$fp>);
close $fp;

%holes_err_qr = ();
%holes_err_no_qr = ();

open(my $fp, 'holes_err') or die 'Could not open holes_err';
while (<$fp>) {
	if (/(.*)(HOLE) ([01]*) (.*)\((.*)\)/) {
        my $pre = $1;
        my $post = $4;
        my $box = $3;
		my $box_words = $5;
        if (length($box_words) > 0) {
            $holes_err_qr{$box} = "$post ($box_words)";
        } else {
            $holes_err_no_qr{$box} = "$post ($box_words)";
        } 
    } else {
        print "Error : could not parse $_";
    }
}

open(my $true_qr, '>', 'true_holes_quasi') or die 'Could not open true_holes_quasi';
open(my $true_no_qr, '>', 'true_holes_no_quasi') or die 'Could not open true_holes_no_quasi';
open(my $missing, '>', 'holes_missing_from_err') or die 'Could not open holes_missing_from_err';
for $hole (@open_holes) {
    if (defined(my $data = $holes_err_qr{$hole})) {
        print $true_qr "HOLE $hole $data\n";
    } elsif (defined(my $data = $holes_err_no_qr{$hole})) {
        print $true_no_qr "HOLE $hole $data\n";
    } else {
        print $missing "$hole\n";
    }
}
close $true_qr;
close $true_no_qr;
close $missing;

system('sort -k6n,6 true_holes_no_quasi > true_holes_no_quasi_sorted');
system('perl' . " $script_dir/canon_hole.pl " . '< true_holes_quasi > true_holes_quasi_canon');
system('perl' . " $script_dir/canon_hole_count.pl " . '-s -u < true_holes_quasi_canon > sorted_words_canon');
