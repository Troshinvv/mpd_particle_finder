#!/bin/bash

#SBATCH -D /scratch2/troshin/log
#SBATCH -t 24:00:00
#SBATCH -p nica
#SBATCH -J finder
#SBATCH -a 1-836
#SBATCH --mem-per-cpu=4G
#SBATCH -o /scratch2/troshin/log/%A_%a.log
#SBATCH --exclude=ncx111,ncx112,ncx113,ncx115,ncx117,ncx121,ncx123,ncx127,ncx153,ncx158,ncx159,ncx171,ncx181,ncx207,ncx214,ncx216,ncx222,ncx223,ncx224,ncx225,ncx227,ncx170,ncx177,ncx172
##1246/17
list_dir=/scratch2/troshin/mpd_hyperons/req40_qa/lambda/list_dir/
output_dir=/scratch2/troshin/mpd_hyperons/req40_qa/kfp_cent
#output_dir=/lustre/stor2/mephi/vtroshin/mpd_xexe_2.87_enh_lamb_20/kfp_qa
date
hostname
id=$SLURM_ARRAY_TASK_ID
input_list=/scratch2/troshin/req40_converted_84M.txt
#99976
split -l 1 -d -a 4 --additional-suffix=.txt $input_list $list_dir
file_list=$( ls $list_dir | head -n $id | tail -n 1 )
mkdir -p $output_dir
cd $output_dir
mkdir -p $id
cd $id
#source /cvmfs/nica.jinr.ru/sw/os/login.sh
#source /cvmfs/nica.jinr.ru/centos7/bmn/env.sh
#source /cvmfs/nica.jinr.ru/centos7/bmnroot/23.08.0/bmnroot_config.sh

source /cvmfs/nica.jinr.ru/sw/os/login.sh
module add mpddev
export MPDROOT=/scratch2/troshin/mpd_hyperons/ownmpd
source $MPDROOT/config/env.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/scratch2/troshin/mpd_hyperons/PFSimple/install/lib/:/scratch2/troshin/mpd_hyperons/PFSimple/install/external/lib

#time /scratch2/troshin/mpd_hyperons/mpd_particle_finder/build/find_candidates /scratch2/troshin/mpd_hyperons/mpd_particle_finder/macro/uni/lambda_kshort_rec40.cc  $list_dir$file_list
#time /scratch2/troshin/mpd_hyperons/mpd_particle_finder/build/find_candidates /scratch2/troshin/mpd_hyperons/mpd_particle_finder/macro/uni/tru_lambda_qa.cc  $list_dir$file_list
time root -l -b -q /scratch2/troshin/mpd_hyperons/mpd_particle_finder/macro/uni/rec_lambda_sim_qa_rec40.cc
#time root -l -b -q /lustre/home/user/v/vtroshin/bmn_hyperons/bmn_particle_finder/macro/rec_k_short_qa.cc
time mv $output_dir/$id/rec_lambda_sim_qa.root $output_dir/rec_lambda_sim_qa_$id.root
#time mv $output_dir/$id/tru_lambda_qa.root $output_dir/tru_lambda_qa_$id.root
