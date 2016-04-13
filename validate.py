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

def run_refine(command, dest_dir) :
    pid = os.getpid()
    pid_file = dest_dir + '/' + str(pid) + '.pid'
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
        opts, args = getopt.getopt(sys.argv[1:],'w:p:c:O',['words=','powers=','child_limit=','overwrite'])
    except getopt.GetoptError as err:
        print str(err)
        print('Usage: validate [-w,--words <wordsfile>] [-p,--powers <powersFile>] [-c,--child_limit <limit>] [-O, --overwrite] src_dir dest_dir')
        sys.exit(2)

    if len(args) != 2:
        print('Usage: validate [-w,--words <wordsfile>] [-p,--powers <powersFile>] [-c,--child_limit <limit>] [-O, --overwrite] src_dir dest_dir')
        sys.exit(2)

    # Executables
    treecat = './treecat'
    treeholes = './treecat --open_holes'
    treecheck = './treecat --mark -s'
    refine = './refine'

    # Set up the rest of the arguments
    src_dir = args[0]
    dest_dir = args[1]
    child_limit = 8

    maxSize = '300000000'
    maxDepth = '96'
    truncateDepth = '96'
    inventDepth = '0'
    ballSearchDepth = '-1'
    maxArea = '5.24'
    fillHoles = ' --fillHoles'
    improveTree = ' --improveTree'
    mom = '/dev/null' #/home/ayarmola/momsearch/momWords'
    parameterized = '/dev/null' #/home/ayarmola/momsearch/parameterizedWords'
    powers = '/home/ayarmola/momsearch/powers_combined'
    wordsFile = '/home/ayarmola/momsearch/words'.format(time.time())

    # Get config
    overwrite = False
    for opt, val in opts:
        if opt in ('-w', '--words'):
            wordsFile = val
        if opt in ('-p', '--powers'):
            powers = val
        if opt in ('-c', '--child_limit'):
            child_limit = int(val)
        if opt in ('-O', '--overwrite'):
            overwrite = True
    # Check for and mark incomplete or foreign trees
    subprocess.call('{0} -r {1} \'{2}\''.format(treecheck, dest_dir, ''), shell=True)

    if len(os.listdir(dest_dir)) > 0 :
        print('WARNING: Destination is not empty!') 

    # Get all out file boxes
    try:
        src_boxes = set([os.path.basename(boxfile).replace('.out','') for boxfile in glob.glob(src_dir + '/*.out')])
    except:
        print('Error reading {0}\n'.format(src_dir))
        sys.exit(1)

    if not overwrite :
        try:
            dest_boxes = set([os.path.basename(boxfile).replace('.out','') for boxfile in glob.glob(dest_dir + '/*.out')])
            src_boxes -= dest_boxes
        except:
            print('Error reading {0}\n'.format(dest_dir))
            sys.exit(1)

    print "Launching Refine"

    # Launch the refine runs
    active_pid_to_file = {};
    refine_run_count = 0
    child_count = 0
    done_boxes = set()
    while True:
        sleep(1) # We don't need to to run the main loop to death since we aren't using os.wait
        open_boxes = src_boxes - done_boxes
        best_box = '1'*200
        for box in open_boxes:
            if len(box) < len(best_box):
                best_box = box    

        if len(best_box) > 65:
            if child_count > 0 :
                wait_for_refine = True
            else :
                # We only break if we don't have any more refine processes running
                break
        else :
            wait_for_refine = False

        # We now check for completed refine processes.
        if child_count >= child_limit or (child_count > 0 and len(open_boxes) == 0) or wait_for_refine:
            iter_dict = dict(active_pid_to_file)
            for done_pid, done_box in iter_dict.iteritems() :
                pid_file = dest_dir + '/' + str(done_pid) + '.pid'
                status = command_output('tail -1 {0}'.format(pid_file))

                if 'completed' in status :
                    # We should check the output either way to make sure it is clean 
                    # Refine only ouputs on file! Don't need recursive
                    result = subprocess.call('{0} {1} \'{2}\''.format(treecheck, dest_dir, done_box), shell=True)
                    num_failed = command_output('grep -c FAILED {0}/{1}.err; exit 0'.format(dest_dir, done_box)).rstrip()

                    if result == 0 :
                        if  int(num_failed) == 0 :
                            print 'Completed {0} {1}\n'.format(done_box,done_pid)
                        else :
                            print 'Check FAILED {0} times for box {1}\n'.format(num_failed, done_box)
                    else : # something broke refine but it thinks it has finished
                        print 'Tree corrupt for box {}\n'.format(done_box)
                        done_boxes.remove(done_box) 

                    child_count -= 1
                    del active_pid_to_file[done_pid]
                    os.remove(pid_file)
                    continue

                elif 'failed' in status :
                    # We should check the output either way to make sure clean it up
                    subprocess.call('{0} {1} \'{2}\''.format(treecheck, dest_dir, done_box), shell=True)
                    # If there was an error refining
                    print 'Error with pid {0}\n'.format(done_pid)
                    print 'Error refining box {0}\n'.format(done_box)
                    done.remove(done_box)
                    child_count -= 1
                    del active_pid_to_file[done_pid]
                    os.remove(pid_file)
                    continue
                else :
                    continue
            # sleep(1) # We don't need to to run the main loop to death since we aren't using os.wait
            continue        

        # If we make it here. We are running refine
        print 'Best box: {0}\n'.format(best_box)

        out = dest_dir + '/' + best_box + '.out'
        err = dest_dir + '/' + best_box + '.err'

        treecat_command = '{0} {1} {2}'.format(treecat, src_dir, best_box)
        refine_command = refine + \
                    fillHoles + \
                    improveTree + \
                    ' --box ' + best_box + \
                    ' --maxDepth ' + maxDepth + \
                    ' --truncateDepth ' + truncateDepth + \
                    ' --inventDepth ' + inventDepth + \
                    ' --maxSize ' + maxSize + \
                    ' --words ' + wordsFile + \
                    ' --ballSearchDepth ' + ballSearchDepth + \
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
        print 'Running with run count {1}: {0}\n'.format(command, refine_run_count)
        refine_run = Process(target=run_refine, args=(command, dest_dir,))
        refine_run.start()
        pid = refine_run.pid

        child_count += 1   
        refine_run_count += 1
        done_boxes.add(best_box)
        active_pid_to_file[pid] = best_box
