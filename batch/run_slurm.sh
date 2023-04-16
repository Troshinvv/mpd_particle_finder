#!/bin/bash

#SBATCH -p fast
#SBATCH -t 01:00:00
#SBATCH -J finder
#SBATCH -o /mnt/pool/nica/7/mam2mih/soft/basov/bmn_particle_finder/log/%A_%a.log

list_dir=${1}
output_dir=${2}

id=$SLURM_ARRAY_TASK_ID

file_list=$( ls $list_dir | head -n $id | tail -n 1 )

mkdir -p $output_dir
cd $output_dir
mkdir $id
cd $id

source /mnt/pool/nica/7/mam2mih/soft/basov/fairsoft/install/bin/thisroot.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/mnt/pool/nica/7/mam2mih/soft/basov/PFSimple/install/lib/:/mnt/pool/nica/7/mam2mih/soft/basov/PFSimple/install/external/lib

echo "/mnt/pool/nica/7/mam2mih/soft/basov/bmn_particle_finder/build/find_candidates /mnt/pool/nica/7/mam2mih/soft/basov/bmn_particle_finder/macro/lambda.cc $list_dir/$file_list"

time /mnt/pool/nica/7/mam2mih/soft/basov/bmn_particle_finder/build/find_candidates /mnt/pool/nica/7/mam2mih/soft/basov/bmn_particle_finder/macro/lambda.cc $list_dir/$file_list
time root -l -b -q /mnt/pool/nica/7/mam2mih/soft/basov/bmn_particle_finder/macro/rec_lambda_qa.cc