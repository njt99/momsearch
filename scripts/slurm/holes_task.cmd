#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=10
#SBATCH -t 23:59:00
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
holes="$words_dir/open_holes"

data_dir=$base_dir

cd $bin_dir

# python "$search" -w "$words" -p "$powers" -c 5 -d 80 -h "$holes" "$data_dir/source" "$data_dir/output" > "$data_dir/holes.log" 2>&1
python "$search" -w "$words" -p "$powers" -c 10 -d 80 "$data_dir/source" "$data_dir/output" > "$data_dir/holes.log" 2>&1
