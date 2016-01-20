#!/bin/bash
# serial job using 1 node and 4 processor,
# and runs for 24 hours (max).
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=4
#SBATCH -t 120:00:00
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=yarmola@princeton.edu

cd /home/ayarmola/momsearch
python dosearch.py -w /home/ayarmola/momsearch/words -c 16 -d 67 /scratch/network/ayarmola/run2015/source /scratch/network/ayarmola/run2015/output > /home/ayarmola/run2015.log 2>&1
