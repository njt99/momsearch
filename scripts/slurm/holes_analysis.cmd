#!/bin/bash
#SBATCH -N 1 # node count
#SBATCH --ntasks-per-node=10
#SBATCH -t 4:59:00
# sends mail when process begins, and 
# when it ends. Make sure you define your email 
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-user=andrew.yarmola@uni.lu

cd /home/users/ayarmola/mom_search/momsearch/hole_analysis

perl ../scripts/get_sorted_holes.pl
