#!/usr/bin/perl

%name_to_num_cusps = ();
$file_to_edit = shift @ARGV;
while (<STDIN>) {
	if (/M (.*)=(.*)/) {
		$name = $2 if ($1 eq 'name'); 
		if ($1 eq 'num_cusps') {
            $name_to_num_cusps{$name} = $2;
        }
	}
}
open my $fp, $file_to_edit or die "Could not open $file_to_edit";
while (<$fp>) {
    if (/^(.*) (\S+_\d+_\d+) (.*)$/) {
        my $pre = $1;
        my $post = $3;
        my $name = $2;
        my $num_cusps = $name_to_num_cusps{$name};
        if ($num_cusps) {
            if ($num_cusps > 1) { 
                print "$pre ${name}_has_${num_cusps}_cusps $post\n";
            } else {
                print "$pre ${name}_has_${num_cusps}_cusp $post\n";
            }
        } else {
            print STDERR "No cusp count for $name\n";
            print "$pre ${name} $post\n";
        }
    }
}
