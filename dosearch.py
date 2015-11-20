#!/usr/bin/python

import os, subprocess, sys, getopt, glob, time
from time import sleep
from multiprocessing import Process

# A few useful functions
def command_output(command):
    try:
        # Note that subprocess.check_output retuns a byte string
        pipe = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        out, err = pipe.communicate()
        return out
    except:
        print('Error in {0}\n'.format(command)) 
        sys.exit(1)

def add_holes(holes, treeholes, directory, boxfile):
    command = '{0} -r {1} \'{2}\''.format(treeholes, directory, boxfile)
    byte_string = command_output(command)
    if not byte_string: return
    byte_string = byte_string.replace('root','')
    if len(byte_string) == 0 : byte_string = 'root'
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

def run_refine(command, destDir) :
    pid = os.getpid()
    pid_file = destDir + '/' + str(pid) + '.pid'
    with open(pid_file,'a') as fp : 
        fp.write(command + '\n')
        fp.close()
    returnCode = subprocess.call(command, shell=True)
    if returnCode == 0:
        with open(pid_file,'a') as fp :
            fp.write('\ncompleted')
        sys.exit(0)
    else:
        with open(pid_file,'a') as fp :
            fp.write('\nfailed')
        sys.exit(1)

if __name__ == '__main__' :

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
    childLimit = 4

    maxSize = '3000000'
    maxDepth = '42'
    truncateDepth = '6'
    inventDepth = '42'
    ballSearchDepth = '9'
    maxArea = '5.2'
    fillHoles = ' --fillHoles'
    mom = '/home/ayarmola/momsearch/momWords'
    parameterized = '/home/ayarmola/momsearch/parameterizedWords'
    powers = '/home/ayarmola/momsearch/powers_combined'

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
        done = set([os.path.basename(boxfile).replace('.out','') for boxfile in glob.glob(destDir + '/*.out')])
    except:
        print('Error reading {0}\n'.format(destDir)) 
        sys.exit(1)

    print "Launching Refine"

    # Launch the refine runs
    activePidToHole = {};
    refineRunCount = 0
    childCount = 0
    waitForHoles = False
    while True:
        sleep(2) # We don't need to to run the main loop to death since we aren't using os.wait
        openHoles = holes - done
        if len(openHoles) == 0 and refineRunCount == 0:
            bestHole = 'root'
        else : 
            bestHole = '1'*200
        for hole in openHoles:
            if len(hole) < len(bestHole):
                bestHole = hole    

        if len(bestHole) > 95:
            if childCount > 0 :
                waitForHoles = True
            else :
                # We only break if we don't have any more refine processes running
                break
        else :
            waitForHoles = False

        # We now check for completed refine processes.
        if childCount >= childLimit or (childCount > 0 and len(openHoles) == 0) or waitForHoles:
            iterDict = dict(activePidToHole)
            for donePid, doneHole in iterDict.iteritems() :
                pid_file = destDir + '/' + str(donePid) + '.pid'
                status = command_output('tail -1 {0}'.format(pid_file))

                if 'completed' in status :
                    # We should check the output either way to make sure it is clean 
                    subprocess.call('{0} {1} \'{2}\''.format(treecheck, destDir, doneHole), shell=True)

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
                    del activePidToHole[donePid]
                    continue

                elif 'failed' in status :
                    # We should check the output either way to make sure it is clean 
                    subprocess.call('{0} {1} \'{2}\''.format(treecheck, destDir, doneHole), shell=True)
                    # If there was an error refining
                    print 'Error refining hole {}\n'.format(done)
                    done.remove(doneHole)
                    childCount -= 1
                    del activePidToHole[donePid]
                    continue
                else :
                    continue
            sleep(60) # We don't need to to run the main loop to death since we aren't using os.wait
            continue        

        # If we make it here. We are running refine
        print 'Best hole: {0}\n'.format(bestHole)

        out = destDir + '/' + bestHole + '.out'
        err = destDir + '/' + bestHole + '.err'

        if bestHole == 'root':
            pidBallSearchDepth = '-1'
        else: 
            pidBallSearchDepth = ballSearchDepth

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
                    ' --powers ' + powers + \
                    ' --mom ' + mom + \
                    fillHoles + \
                    ' --parameterized ' + parameterized + \
                    ' > ' + out  + ' 2> ' + err

        first_command = '{0} {1} {2} | head -1'.format(treecat, srcDir, bestHole)
        first = command_output(first_command).rstrip()

        if first[:1] == 'H': # HOLE
            command = command.replace(srcDir, backingDir)

        print 'Running with run count {1}: {0}\n'.format(command, refineRunCount)
        refine_run = Process(target=run_refine, args=(command, destDir,))
        refine_run.start()
        pid = refine_run.pid

        childCount += 1   
        refineRunCount += 1
        done.add(bestHole)
        activePidToHole[pid] = bestHole
