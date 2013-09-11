import time
import subprocess
import re

def check_output(*popenargs, **kwargs):
	process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
	output, unused_err = process.communicate()
	retcode = process.poll()
	if retcode:
		cmd = kwargs.get("args")
		if cmd is None:
			cmd = popenargs[0]
		error = subprocess.CalledProcessError(retcode, cmd)
		error.output = output
		raise error
	return output
	
def RunClients():
	while (1):
		jobs = check_output(['qstat']).split('\n')
		job_count = 0
		for job in jobs:
			if re.search('momclient', job):
				job_count += 1
		if job_count < 36:
			subprocess.check_call(['qsub', 'momclient.pbs'])
			
		else:
			time.sleep(10)

if  __name__ == '__main__':
	RunClients()

		
