#!/bin/bash
# serial job using 1 node and 8 processor,
# and runs for 24 hours (max).
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=4
#SBATCH -t 23:59:00
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=yarmola@princeton.edu


bin_dir="/home/ayarmola/momsearch"
words_dir=$bin_dir

search="$bin_dir/dosearch.py"
words="$words_dir/words"
powers="$words_dir/powers_combined"
holes="$bin_dir/open_holes"

data_dir="/scratch/network/ayarmola/run2015"

cd $bin_dir

python "$search" -w "$words" -p "$powers" -c 12 -d 65 -h "$holes" "$data_dir/source" "$data_dir/output" > "$data_dir/holes.log" 2>&1
