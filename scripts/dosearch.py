#!/usr/bin/python

import os, subprocess, sys, getopt, glob, time, re
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

def add_holes_from_file(holes,fp) :
    try:
        for line in open(fp):
            hole = line.rstrip()
            if hole[0] == '1' or\
               hole[0] == '0' :
                holes.add(hole)
    except:
        print('Error loading holes file {0}\n'.format(fp))
        sys.exit(1)


def add_words(words, fp):
    try:
        for line in open(fp):
            word = line.rstrip()
            if word[0].isdigit() or\
               word[0] == 'X'    or\
               word[0] == 'H' : continue
            else:
                if ',' in word :
                    word = re.findall('\((.*?),.*\)', word)[-1]
                elif '(' in word :
                    word = re.findall('\((.*?)\)', word)[-1]
                if word[0] == 'G' or word[0] == 'g' :
                    words.add(word)
    except:
        print('Error loading words file {0}\n'.format(fp))
        # sys.exit(1)

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
        opts, args = getopt.getopt(sys.argv[1:],'w:p:c:d:h:r:i:t:s:',['words=','powers=','child_limit=','depth_limit=','holes=','refine=','invent_depth=','truncate_depth','search_depth='])
    except getopt.GetoptError as err:
        print str(err)
        print('Usage: dosearch [-w,--words <wordsfile>] [-p,--powers <powersFile>] [-c,--child_limit <limit>] [-d,--depth_limit <limit>] [-h,-holes <holesfile>] srcDir destDir')
        sys.exit(2)

    if len(args) != 2:
        print('Usage: dosearch [-w,--words <wordsfile>] [-p,--powers <powersFile>] [-c,--child_limit <limit>] [-d,--depth_limit <limit>] [-h,-holes <holesfile>] srcDir destDir')
        sys.exit(2)

    # Executables
    treecat = './treecat'
    treeholes = './treecat --open_holes'
    treecheck = './treecat --mark -s'
    refine = './refine'

    # Set up the rest of the arguments
    srcDir = args[0]
    destDir = args[1]
    childLimit = 8
    depth_limit = 67

    maxSize = '5000000'
    maxDepth = '181'
    truncateDepth = '139'
    inventDepth = '145'
    ballSearchDepth = '6'
    maxArea = '5.24'
    fillHoles = ' --fillHoles'
    mom = '/dev/null' #/home/ayarmola/momsearch/momWords'
    parameterized = '/dev/null' #/home/ayarmola/momsearch/parameterizedWords'
    powers = '/home/ayarmola/momsearch/powers_combined'
    wordsFile = '/home/ayarmola/momsearch/words'.format(time.time())

    # Get config
    holes_file = None
    seenWords = set()
    for opt, val in opts:
        if opt in ('-w', '--words'):
            wordsFile = val
        if opt in ('-p', '--powers'):
            powers = val
        if opt in ('-c', '--child_limit'):
            childLimit = int(val)
        if opt in ('-d', '--depth_limit'):
            depth_limit = int(val)
        if opt in ('-h', '--holes'):
            holes_file = val
        if opt in ('-r', '--refine'):
	    refine = val
        if opt in ('-i', '--invent_depth'):
	    inventDepth = str(int(val))
        if opt in ('-t', '--truncate_depth'):
	    truncateDepth = str(int(val))
        if opt in ('-s', '--search_depth'):
	    ballSearchDepth = str(int(val))
            
    add_words(seenWords, wordsFile)

    # Check for incomplete trees
    subprocess.call('{0} -r {1} \'{2}\''.format(treecheck, destDir, ''), shell=True)

    # Get holes. Note, treecat will check that all files are complete trees
    holes = set();
    if holes_file :
        add_holes_from_file(holes, holes_file)
    else :
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
        sleep(0.1) # We don't need to to run the main loop to death since we aren't using os.wait
        openHoles = holes - done
        if len(openHoles) == 0 and refineRunCount == 0:
            bestHole = 'root'
        else : 
            bestHole = '1'*200
        for hole in openHoles:
            if len(hole) < len(bestHole) :
                bestHole = hole    

        if len(bestHole) > depth_limit:
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
                    
                    print 'Holes: {0} patched, {1} unpatched, {2} open holes\n'.format(numPatched, numUnpatched, int(numHoles))

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
                    os.remove(pid_file)
                    continue

                elif 'failed' in status :
                    # We should check the output either way to make sure it is clean 
                    subprocess.call('{0} {1} \'{2}\''.format(treecheck, destDir, doneHole), shell=True)
                    # If there was an error refining
                    print 'Error with pid {0}\n'.format(donePid)
                    print 'Error refining hole {0}\n'.format(doneHole)
                    done.remove(doneHole)
                    childCount -= 1
                    del activePidToHole[donePid]
                    os.remove(pid_file)
                    continue
                else :
                    continue
            sleep(0.1) # We don't need to to run the main loop to death since we aren't using os.wait
            continue        

        # If we make it here. We are running refine
        print 'Open hole count: {0}\n'.format(len(openHoles))
        print 'Best hole: {0}\n'.format(bestHole)

        out = destDir + '/' + bestHole + '.out'
        err = destDir + '/' + bestHole + '.err'

        if bestHole == 'root':
            pidBallSearchDepth = '-1'
        else: 
            pidBallSearchDepth = ballSearchDepth

        treecat_command = '{0} {1} {2}'.format(treecat, srcDir, bestHole)
        refine_command = refine + \
                    fillHoles + \
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
                    ' --parameterized ' + parameterized + \
                    ' > ' + out  + ' 2> ' + err

        first_command = treecat_command + ' | head -1'
        first = command_output(first_command).rstrip()

        if first[:1] == 'H': # HOLE
            treecat_command = 'echo 1'

        command = treecat_command + ' | ' + refine_command
        print 'Running with run count {1}: {0}\n'.format(command, refineRunCount)
        refine_run = Process(target=run_refine, args=(command, destDir,))
        refine_run.start()
        pid = refine_run.pid

        childCount += 1   
        refineRunCount += 1
        done.add(bestHole)
        activePidToHole[pid] = bestHole
