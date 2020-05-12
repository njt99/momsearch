#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=39
#SBATCH -t 239:59:00
#SBATCH --mem=60GB
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=yarmola@princeton.edu

base_dir="/u/yarmola/momsearch"
bin_dir="$base_dir/bin"
words_dir=$base_dir

search="$base_dir/scripts/dosearch_slopes.py"
words="$words_dir/words"
powers="$words_dir/powers_combined"

data_dir="/scratch/network/yarmola/run_slopes"

cd $bin_dir

python "$search" -i 120 -t 6 -r "$bin_dir/refine_slopes" -w "$words" -p "$powers" -c 39 "$data_dir/source" "$data_dir/output_live" > "$data_dir/refine.log" 2>&1
