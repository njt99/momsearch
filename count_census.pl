#!/usr/bin/perl

$total_count = 0;
$with_census_count = 0;
$total_census_count = 0;
while (<STDIN>) {
	if (/(.*) ([10]+) ([^:]*)$/) {
        $total_count += 1;
        $with_census_count += 1;
        my @census_data = split(/ /, $3); 
        $total_census_count += $#census_data + 1; 
    } elsif (/(.*) ([10]+)$/) {
        $total_count += 1;
    }
}
$percent_w_census = 100. * $with_census_count / $total_count;
$avg_census_count = $total_census_count / $with_census_count;

print "We have $with_census_count with census out of $total_count ($percent_w_census %)\n";
print "Average census count is $avg_census_count\n";
