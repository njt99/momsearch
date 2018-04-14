#!/bin/bash
# serial job using 1 node and 8 processor,
# and runs for 24 hours (max).
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=16
#SBATCH -t 23:59:00
#SBATCH --mem=64GB
#SBATCH --qos=qos-besteffort
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=andrew.yarmola@uni.lu

base_dir="/home/users/ayarmola/mom_search/momsearch"
bin_dir="$base_dir/bin"
words_dir=$base_dir

search="$base_dir/scripts/dosearch.py"
words="$words_dir/words"
powers="$words_dir/powers_combined"

data_dir=$base_dir

cd $bin_dir

python "$search" -w "$words" -p "$powers" -c 16 "$data_dir/source" "$data_dir/output" > "$data_dir/refine.log" 2>&1
