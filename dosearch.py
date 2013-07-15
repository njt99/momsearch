#!/usr/bin/python

import os, subprocess, sys, getopt, glob, time

try:
    opts, args = getopt.getopt(sys.argv[1:],'w:',['words='])
except getopt.GetoptError as err:
    print str(err)
    print('Usage: dosearch [-w,--words <wordsfile>] srcDir destDir backingDir')
    sys.exit(2)

if len(args) != 3:
    print('Usage: dosearch [-w,--words <wordsfile>] srcDir destDir backingDir')
    sys.exit(2)

# Executables
treecat = './treecat'
treeholes = './treecat --holes'
treecheck = './treecat --mark -s'
refine = './refine'

# Set up the rest of the arguments
srcDir = args[0]
destDir = args[1]
backingDir = args[2]
childLimit = 2
childCount = 0

maxSize = '3000000'
maxDepth = '42'
truncateDepth = '6'
inventDepth = '42'
ballSearchDepth = '9'
maxArea = '5'
fillHoles = ' --fillHoles'
mom = '/dev/null'
parameterized = '/dev/null'

# A few useful functions
def command_output(command):
    try:
        # Note that subprocess.check_output retuns a byte string
        return subprocess.check_output(command, shell=True)
    except:
        print('Error in {0}\n'.format(command)) 
        sys.exit(1)

def add_holes(holes, treeholes, directory, boxfile):
    command = '{0} -r {1} \'{2}\''.format(treeholes, directory, boxfile)
    byte_string = command_output(command)
    if not byte_string: return
    byte_string.replace('root','')
    newHoles = set(byte_string.rstrip().split('\n'))
    holes |= newHoles

def add_words(words, fp):
    try:
        for line in open(fp):
            word = line.rstrip()
            if word[0].isdigit() or\
               word[0] == 'X'    or\
               word[0] == 'H': continue
            else:
                words.add(word)
    except:
        print('Error loading words file {0}\n'.format(fp))
        sys.exit(1)

# Get the seen words
wordsFile = 'allWords_{0:.0f}'.format(time.time())
seenWords = set()
for opt, val in opts:
    if opt in ('-w', '--words'):
        wordsFile = val
        add_words(seenWords, wordsFile)

# Check for incomplete trees
subprocess.call('{0} -r {1} \'{2}\''.format(treecheck, destDir, ''), shell=True)

# Get holes. Note, treecat will check that all files are complete trees
holes = set();
add_holes(holes, treeholes, destDir, '')

# Get done words
done = set()
try:
    done = {os.path.basename(boxfile).replace('.out','') for boxfile in glob.glob(destDir + '/*.out')}
except:
    print('Error reading {0}\n'.format(destDir)) 
    sys.exit(1)

# Launch the refine runs
pidToHole = {};
while True:
    if childCount >= childLimit:
        donePid, status = os.wait()
        exitStatus = os.WEXITSTATUS(status)
        doneHole = pidToHole[donePid]

        # We should check the output either way to make sure it is clean 
        subprocess.call('{0} {1} \'{2}\''.format(treecheck, destDir, doneHole), shell=True)

        # If there was an error refining
        if exitStatus != 0:
            done.remove(doneHole)
            continue 

        print 'Completed {0} {1}\n'.format(doneHole,donePid)
        add_holes(holes, treeholes, destDir, doneHole)

        numPatched = command_output('grep -c Patched {0}/{1}.err; exit 0'.format(destDir, doneHole)).rstrip()
        numUnpatched = command_output('grep -c Unpatched {0}/{1}.err; exit 0'.format(destDir, doneHole)).rstrip()
        numHoles = command_output('grep -c HOLE {0}/{1}.err; exit 0'.format(destDir, doneHole)).rstrip()
        
        print 'Holes: {0} patched, {1} unpatched, {2} holes\n'.format(numPatched, numUnpatched, numHoles)

        boxWords = set()
        add_words(boxWords, '{0}/{1}.out'.format(destDir, doneHole))        
        newWords = boxWords - seenWords
        seenWords |= newWords

        if len(newWords) > 0: 
            f = open(wordsFile, 'a')
            for word in newWords:
                print 'Adding word {0}'.format(word)
                f.write(word + '\n')
            f.close()

        childCount -= 1

    bestHole = '1'*200
    for hole in holes:
        if hole not in done and len(hole) < len(bestHole):
            bestHole = hole    

    if len(bestHole) > 95: break

    if not bestHole:
        bestHole = 'root'

    done.add(bestHole)
    childCount += 1   
 
    pid = os.fork()
    if pid == 0:
        if bestHole == 'root':
            pidBallSearchDepth = '-1'
        else: 
            pidBallSearchDepth = ballSearchDepth

        out = destDir + '/' + bestHole + '.out'
        err = destDir + '/' + bestHole + '.err'

        command = treecat + ' ' +  srcDir + ' ' + bestHole + \
                    ' | ' + refine + \
                    ' --box ' + bestHole + \
                    ' --maxDepth ' + maxDepth + \
                    ' --truncateDepth ' + truncateDepth + \
                    ' --inventDepth ' + inventDepth + \
                    ' --maxSize ' + maxSize + \
                    ' --words ' + wordsFile + \
                    ' --ballSearchDepth ' + pidBallSearchDepth + \
                    ' --maxArea ' + maxArea + \
                    ' --mom ' + mom + \
                    fillHoles + \
                    ' --parameterized ' + parameterized + \
                    ' > ' + out  + ' 2> ' + err



        first_command = '{0} {1} {2} | head -1'.format(treecat, srcDir, bestHole)
        first = command_output(first_command).rstrip()

        if first[:1] == 'H': # HOLE
            command = command.replace(srcDir, backingDir)

        print 'Running {0}\n'.format(command)
        returnCode = subprocess.call(command, shell=True)
        if returnCode == 0: sys.exit(0) 
        else: sys.exit(1)
    else:
        pidToHole[pid] = bestHole
