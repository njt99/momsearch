#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=1
#SBATCH -t 23:59:00
#SBATCH --qos=qos-besteffort
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=andrew.yarmola@uni.lu

base_dir="/home/users/ayarmola/mom_search/momsearch"
bin_dir="$base_dir/src"
data_dir=$base_dir

cd $bin_dir

./treecat --mark -s -r "$base_dir/output" ''  
