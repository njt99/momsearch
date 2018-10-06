#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=32
#SBATCH -t 90:00:00
#SBATCH --mem=64GB
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=yarmola@princeton.edu

base_dir="/home/ayarmola/momsearch"
bin_dir="$base_dir/bin"
words_dir=$base_dir

search="$base_dir/scripts/dosearch.py"
words="$words_dir/words"
powers="$words_dir/powers_combined"

data_dir="/scratch/network/ayarmola"

cd $bin_dir

python "$search" -w "$words" -p "$powers" -c 32 "$data_dir/source" "$data_dir/output" > "$data_dir/refine.log" 2>&1
