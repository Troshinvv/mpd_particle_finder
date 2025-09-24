#!/bin/bash

#SBATCH -D /lustre/home/user/v/vtroshin/mpd_hyperons/mpd_particle_finder/log
#SBATCH -p mpd-ice --qos=dirac
##SBATCH -t 24:00:00
#SBATCH -J finder
#SBATCH -a 1-763
#SBATCH --mem-per-cpu=4G
#SBATCH -o /lustre/home/user/v/vtroshin/mpd_hyperons/mpd_particle_finder/log/%A_%a.log
#SBATCH --exclude=n02p002,n02p069,n02p064,n05p010
##640
##465_JAM
list_dir=/lustre/stor2/mephi/vtroshin/mpd_hyperons/lambda/list_dir/
output_dir=/lustre/stor2/mephi/vtroshin/mpd_hyperons/lambda/out_phsd_nhits10

date
hostname
id=$SLURM_ARRAY_TASK_ID
input_list=/lustre/home/user/v/vtroshin/mpd_hyperons/rec30_lamb.list
split -l 1 -d -a 4 --additional-suffix=.txt $input_list $list_dir
file_list=$( ls $list_dir | head -n $id | tail -n 1 )
mkdir -p $output_dir
cd $output_dir
mkdir -p $id
cd $id
source /cvmfs/nica.jinr.ru/sw/os/login.sh
source /cvmfs/nica.jinr.ru/centos7/bmn/env.sh
source /cvmfs/nica.jinr.ru/centos7/bmnroot/23.08.0/bmnroot_config.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lustre/home/user/v/vtroshin/bmn_hyperons/PFSimple/install/lib/:/lustre/home/user/v/vtroshin/bmn_hyperons/PFSimple/install/external/lib

echo "/lustre/home/user/v/vtroshin/bmn_particle_finder/build/find_candidates /lustre/home/user/v/vtroshin/bmn_hyperons/bmn_particle_finder/macro/lambda.cc $list_dir$file_list"
time /lustre/home/user/v/vtroshin/mpd_hyperons/mpd_particle_finder/build/find_candidates /lustre/home/user/v/vtroshin/mpd_hyperons/mpd_particle_finder/macro/rec30_lambda_kshort.cc  $list_dir$file_list
time root -l -b -q /lustre/home/user/v/vtroshin/mpd_hyperons/mpd_particle_finder/macro/rec_lambda_sim_qa.cc
#time root -l -b -q /lustre/home/user/v/vtroshin/bmn_hyperons/bmn_particle_finder/macro/rec_k_short_qa.cc
time mv $output_dir/$id/rec_lambda_sim_qa.root $output_dir/rec_lambda_sim_qa_$id.root
