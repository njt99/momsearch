# perl script to search for possible impossible identities.

while ($word = <>) {
	chomp $word;
	@possiblePowers = &getPossiblePowers($word);
	foreach $line (@possiblePowers) {
		print $line, "\n";
	}
}

sub getPossiblePowers
{
	local ($word) = @_;
	local @x = ();
	local @y = ();
	local $m = 0;
	local $n = 0;
	local $i = 0;
	local $gs = '';
	local $ch = '';
    # The list gs will containing the G,g's of the word
    # The vecotrs x,y will contain the m,n powers between the G,g's of the word.
    # The m,n powers at the begining+end of the word are at x[0],y[0]
    # Note: words are usually in canonical form with G,g at the front
	for ($i = 0; $i < length $word; ++$i) {
		$ch = substr $word, $i, 1;
		if ($ch eq 'g' || $ch eq 'G') {
			push(@x, $m);
			push(@y, $n);
			$gs .= $ch;
			$m = $n = 0;
		} elsif ($ch eq 'm') {
			++$m;
		} elsif ($ch eq 'M') {
			--$m;
		} elsif ($ch eq 'n') {
			++$n;
		} elsif ($ch eq 'N') {
			--$n;
		}
	}

	$x[0] += $m;
	$y[0] += $n;
    print "gs is $gs\n";
    print "x vector @x\n";
    print "y vector @y\n";

	local ($numG, $n, $j, $base, $k, $numDiscrepancies, $discPos, $offset, $power, $sub, $subID) = ();
	local %found = ();
	$numG = length $gs;
    # Note: length of gs = length of x = length of y
	local @result = ();
N:	for ($n = 1; $n+$n <= $numG; ++$n) {
		# only consider divisors of $numG
		next if $numG % $n > 0;

		# first, check to make sure g has the correct sign
		for ($j = 1; $j*$n < $numG; ++$j) {
            print "j is $j and n is $n\n";
            print substr($gs, 0, $n);
            print "\n";
            print substr($gs, $n*$j, $n);
            print "\n";
			next N if substr($gs, 0, $n) ne substr($gs, $n*$j, $n);
		}

        # If we are at this point, then gs is some power of a shorter words in g's
        # TODO: do we need to cyclically permute gs to make sure?

		for ($base = $n; $base <= 2*$n && $base < $numG; $base += $n) {
			$numDiscrepancies = 0;
			$discPos = 0;
			for ($j = 0; $j < $numG; $j += $n) {
				for ($k = 0; $k < $n; ++$k) {
					next if $j == 0 && $k == 0;
                    print "Base is $base\n";
                    print "j is $j\n";
                    print "k is $k\n";
                    print "x[k+j] is $x[$k + $j]\n";
                    print "x[k+base] is $x[$k + $base]\n";
                    print "y[k+j] is $y[$k + $j]\n";
                    print "y[k+base] is $y[$k + $base]\n";
					if ($x[$k + $j] != $x[$k + $base] || $y[$k + $j] != $y[$k + $base]) {
						++$numDiscrepancies;
						$discPos = $j + $k;
                        print "Number of discrepencies $numDiscrepancies\n";
                        print "New discrepency position $discPos\n";
					}
				}
			}
			if ($numDiscrepancies < 2) {
				$offset = $base/$n;
				$power = $numG / $n;
				$sub = '';
				for ($k = 0; $k < $n; ++$k) {
					if ($x[$base+$k] > 0) {
						$sub .= 'm' x $x[$base+$k];
					} elsif ($x[$base+$k] < 0) {
						$sub .= 'M' x -$x[$base+$k];
					}
					if ($y[$base+$k] > 0) {
						$sub .= 'n' x $y[$base+$k];
					} elsif ($y[$base+$k] < 0) {
						$sub .= 'N' x -$y[$base+$k];
					}
					$sub .= substr($gs, $base+$k, 1);
				}
				print "HERE word=$word n=$n base=$base sub=$sub\n";
				local $subIDAllowed = ($n > 1) ? 1 : 0;
				local $subLine = '';
				if ($subIDAllowed) {
					print "sub=$sub\n";
					local @subLines = &getPossiblePowers($sub);
					print "SUB($sub)=", $#subLines, "\n";
					foreach (@subLines)  {
						my @f = split(/ /);
						if (!$f[3] || ($f[4] == 0 && $f[5] == 0)) {
							$subLine = "{$_}";
							$subIDAllowed = 0;
						}
					}
				}
				local $matchRequired = ($discPos == 0) ? 0 : 1;
				if ($subIDAllowed && $matchRequired) {
					local $discBasePos = $base + ($discPos % $n);
					local $diffX = $x[$discBasePos] - $x[$discPos];
					local $diffY = $y[$discBasePos] - $y[$discPox];
					if ($diffX % $power != 0 || $diffy % $power != 0) {
#						$matchRequired = 0;
					}
				}
				local $matchX = $x[$base]-$x[0];
				local $matchY = $y[$base]-$y[0];
				print "THERE word=$word n=$n base=$base sub=$sub\n";
					
				$line = join(" ", 'PossiblePower', $word, $subIDAllowed, $matchRequired, $matchX, $matchY, "$sub^$power ~ $discPos n=$n base=$base", $subLine);
				if ($subIDAllowed == 0 && $matchRequired == 0) {
					return ($line);
				} else {
					push(@result, $line);
				}
			}
		}
	}
	print "returning($word)=", $#result, "\n";
	@result;
}
