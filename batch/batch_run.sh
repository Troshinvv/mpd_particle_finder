#!/bin/bash
#$ -l h=!(ncx111.jinr.ru|ncx113.jinr.ru|ncx116.jinr.ru|ncx123.jinr.ru|ncx143.jinr.ru|ncx144.jinr.ru|ncx149.jinr.ru|ncx152.jinr.ru|ncx172.jinr.ru|ncx205.jinr.ru|ncx223.jinr.ru|ncx231.jinr.ru)

format='+%Y/%m/%d-%H:%M:%S'

date $format
echo "JOB IS RUNNING on $HOSTNAME"

job_num=$(($SGE_TASK_ID))
filelist=$lists_dir/$(ls $lists_dir | sed "${job_num}q;d")

cd $output_dir
mkdir -p $job_num
cd $job_num

source /cvmfs/nica.jinr.ru/sw/os/login.sh
module add GCC-Toolchain/

export SIMPATH="/scratch1/mmamaev/fairsoft/install"
export FAIRROOTPATH="/scratch1/mmamaev/fairroot/install"

source /scratch1/mmamaev/bmnroot-gitlab/build/config.sh
echo
date
echo "Running particle finder ..."
echo "/scratch1/mmamaev/bmn_particle_finder/build/find_candidates /scratch1/mmamaev/bmn_particle_finder/macro/test.cc $filelist"

/scratch1/mmamaev/bmn_particle_finder/build/find_candidates /scratch1/mmamaev/bmn_particle_finder/macro/k_short.cc $filelist

echo PROCESS FINISHED