#!/usr/bin/python

import os subprocess sys getopt glob

try:
    opts, args = getopt.getopt(sys.argv[1:],'w:',['words='], )
except getopt.GetoptError as err:
    print str(err)
    print('Usage: dosearch [-w,--words <wordsfile>] srcDir destDir backingDir')
    sys.exit(2)

if len(args) != 3:
    print('Usage: dosearch [-w,--words <wordsfile>] srcDir destDir backingDir')
    sys.exit(2)

# Executables
treecat = './treecat'
refine = './refine'

seenWords = {}

for opt, val in opts:
    if opt in ('-w', '--words'):
        try:
            seenWords = {word.rstrip() : True for word in open(val)}
        except:
            print('Error loading words file %s'.format(val))
            sys.exit(1)

srcDir = args[0]
destDir = args[1]
backingDir = args[2]
childLimit = 2
childCount = 0
ballSearchDepth = '9'

holes = [];
try:
    # Get the holes in the destination directory
    holes = subprocess.check_output([treecat, '-r', '--holes', destDir, '']).rstrip().split('\n')
except:
    print('Error in treecat -r %s \'\''.format(destDir)) 
    sys.exit(1)

done = {}
try:
    done = {os.path.basename(boxfile) : True for boxfile in glob.glob(destDir + '/*.out')}
except:
    print('Error reading %s'.format(destDir)) 
    sys.exit(1)

pidHole = {};
while True:
	if ($childCount >= $childLimit) {
		$donePid = wait;
		$doneHole = $pidHole{$donePid};
		print "$donePid $doneHole done\n";
		@newHoles = `perl mkHoles.pl $destDir/$doneHole.out`;
		chomp @newHoles;
		push(@holes, @newHoles);

		$numPatched = `grep -c Patched $destDir/$doneHole.err`;
		$numUnpatched = `grep -c Unpatched $destDir/$doneHole.err`;
		$numHoles = `grep -c HOLE $destDir/$doneHole.err`;
		chomp $numPatched;
		chomp $numUnpatched;
		chomp $numHoles;
		print "Holes: $numPatched patched, $numUnpatched unpatched, $numHoles holes\n";
		@newWords = ();
		@seenWords = `sortuniq $destDir/$doneHole.out`;
		chomp @seenWords;
		foreach (@seenWords) {
			($null, $count, $word) = split(/\s+/);
			next if $word =~ /\d/ || $word eq 'HOLE' || $word eq 'X';
			next if defined $seenWord{$word};
			$seenWord{$word} = 1;
			print "adding word $word\n";
			push(@newWords, $word);
		}
		if ($#newWords > -1) {
			open(WORDS, ">>allWords_s6");
			foreach (@newWords) {
				print WORDS "$_\n";
			}
			close WORDS;
		}
		--$childCount;
	}

	bestHole = '1'*200
	for hole in holes:
	    if done[hole]: continue
        if len(hole) < len(bestHole):
            bestHole = hole	

    if len(bestHole) > 95: break

    done[bestHole] = True
    childCount += 1   
 
	pid = os.fork()
	if pid == 0:
        bsdOpt = '--ballSeachDepth ' + ballSearchDepth
		if not bestHole:
			bestHole = 'root'
		    bsdOpt = ''

        maxSize = '3000000'
        maxDepth = '42'
        truncateDepth = '6'
        inventDepth = '42'

		command = treecat + srcDir + bestHole + ' | ' + refine + \
                    ' --box ' + bestHole\
                    ' --maxDepth ' + maxDepth\
                    ' --truncateDepth ' + truncateDepth\
                    ' --inventDepth ' + inventDepth\
                    ' --maxSize ' + maxSize\
                    -w allWords_s6 $searchWords -a 5 -M /dev/null -P /dev/null > $destDir/$bestHole.out 2> $destDir/$bestHole.err";
		$first = `./treecat $srcDir $bestHole | head -1`;
		chomp $first;
		if ($first eq 'HOLE') {
			$command =~ s/$srcDir/$backingDir/;
		}
		print "doing $command\n";
	    system $command;
	    exit;
	else:
	    pidHole[pid] = bestHole
	}
}
