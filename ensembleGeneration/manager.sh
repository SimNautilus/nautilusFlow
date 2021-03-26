#!/bin/bash

source ~/.bash_profile_conda
# conda environment with python 3.6 and libEnsemble
conda activate myenv

cat $COBALT_NODEFILE > worker_list
# remove first node from worker_list file - this is the head node for libEnsemble
sed -i 1d worker_list

python calling_func.py
