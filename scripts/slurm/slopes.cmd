#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=38
#SBATCH -t 238:59:00
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
words="$words_dir/words_final"
powers="$words_dir/powers_final"

data_dir="/scratch/network/yarmola/slopes"

cd $bin_dir

cat "$data_dir/refine.log" >> "$data_dir/refine.log.all"

python "$search" -i 42 -t 12 -r "$bin_dir/refine_slopes" -w "$words" -p "$powers" -c 38 "$data_dir/source" "$data_dir/output_live" > "$data_dir/refine.log" 2>&1
