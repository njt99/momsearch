# Counts the G,g power or a word
while ($word = <>) {
	chomp $word;
	$gs = '';
	$len = 0;
	for ($i = 0; $i < length $word; ++$i) {
		$ch = substr $word, $i, 1;
		if ($ch eq 'g' || $ch eq 'G') {
			$gs .= $ch;
	    }
    }
    $len = length $gs;
    print "$len\n";
}
