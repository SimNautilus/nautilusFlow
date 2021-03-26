#!/usr/bin/python

import sys
import os

import pandas as pd
import numpy as np

import matplotlib.pyplot as plt
import scipy.io as sc


#from paraview.simple import *

# Step through all run directories, load recircData, extract numbers and save to file. 
###############################################################################

def gather_run_data(dir_name,tstep):
    """
    Output matrix of collected data. 
    """
    # Initialise array to save results to
    # want to save upper and lower, for different runs, for different time steps. 
    RunScalar = np.zeros((2,1))
    # one column for upper, one for lower, one for probe pressure
    
    # Save u, v and p at AIP
    #RunAIP = np.zeros((3,153))
    
    #print 'Extracting results from run:' ,runNum
    directory = dir_name +'/catalyst_data'
    #directory = 'base_case/16-procs_case/run_data'
    os.chdir(directory)
    
    # Upper recirculation area
    fname_dwall= 'upper_'+str(tstep)+'.csv'
    if os.path.isfile(fname_dwall):
        df_dwall=pd.read_csv(fname_dwall, sep=',') 
        data_dwall= df_dwall['dwall']
        RunScalar[0,0]=np.sqrt(data_dwall[0])
    else:
        print(dir_name + ' incomplete')
        RunScalar[0,0]=0
    # Lower recirculation area
    fname_dwall= 'lower_'+str(tstep)+'.csv'
    if os.path.isfile(fname_dwall):
        df_dwall = pd.read_csv(fname_dwall, sep=',') 
        data_dwall= df_dwall['dwall']
        RunScalar[1,0]=np.sqrt(data_dwall[0])
    else: 
        print(dir_name + ' incomplete')
        RunScalar[1,0]=0

    os.chdir('../../../')
    
    return RunScalar

def save_ensemble(tstep, n_runs, i_ens):
    #tstep = 320
    #n_runs = 2

    dir_base = 'ensemble_500_1/Run-'

    scalar_data = np.zeros((n_runs,2))

    for i_run in range(n_runs):
        print(i_run)
        dir_name = dir_base + str(i_run+1)
        scalar_out = gather_run_data(dir_name,tstep)
    
        scalar_data[i_run,0] = scalar_out[0]
        scalar_data[i_run,1] = scalar_out[1]

    np.savetxt('output_data/scalar_data_'+str(i_ens+1)+'.txt', scalar_data)
    sc.savemat('output_data/scalar_data_'+str(i_ens+1)+'.mat', mdict={'scalar_data':scalar_data})

tstep = 10000
n_runs = 500
i_ens = 1

save_ensemble(tstep, n_runs, i_ens)

