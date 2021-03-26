#!/bin/bash

runtime=12:00:00
nodes=101
project=PHASTA_aesp

qsub -t $runtime -n $nodes -q default --mode script -A $project manager.sh
