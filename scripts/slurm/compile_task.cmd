#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=1
#SBATCH -t 02:59:00
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=andrew.yarmola@uni.lu

bin_dir="/home/users/ayarmola/mom_search/momsearch/src"

cd $bin_dir
make refine
make treecat
