#!/bin/bash
#SBATCH -N 5 # node count
#SBATCH --ntasks-per-node=28
#SBATCH -t 120:00:00
#SBATCH --mem=10GB
#SBATCH --qos=qos-batch
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

data_dir="$base_dir"

cd $bin_dir

python "$search" -w "$words" -p "$powers" -c 140 "$data_dir/source" "$data_dir/output" > "$data_dir/refine.log" 2>&1
