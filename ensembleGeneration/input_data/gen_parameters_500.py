#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import scipy.io

# parameteric samples for senstivity analysis 
# First run limit cases
# columns include 

headers = ['LBvelMax','UBvelMax','LBvelMin', 'UBvelMin','BlowerPosition'] 

n_vars = len(headers)
n_samps = 500

param_mat = np.zeros((n_samps, n_vars))

# Generate uniform bounds - scale from -1 to 1 

uniform_mat = np.random.uniform(-1,1,(n_samps,n_vars))
#print(uniform_mat.shape)

# Transform to parameters. 
# UB, LB mean and amplitude + position
UB_v_lim = [2.0, 4.0]
UB_amp_lim = [0.0, 3.0]

UB_v = 0.5*(uniform_mat[:,0]+1)*(UB_v_lim[1]-UB_v_lim[0])+UB_v_lim[0]
UB_amp = 0.5*(uniform_mat[:,1]+1)*(UB_amp_lim[1]-UB_amp_lim[0])+UB_amp_lim[0]

UBvelMax = UB_v + UB_amp
UBvelMin = UB_v - UB_amp


LB_v_lim = [0.0, 0.2]
LB_amp_lim = [0.0, 0.1]

LB_v = 0.5*(uniform_mat[:,2]+1)*(LB_v_lim[1]-LB_v_lim[0])+LB_v_lim[0]
LB_amp = 0.5*(uniform_mat[:,3]+1)*(LB_amp_lim[1]-LB_amp_lim[0])+LB_amp_lim[0]

LBvelMax = LB_v + LB_amp
LBvelMin = LB_v - LB_amp

Pos_lim = [0.0, 1.0]
Pos = 0.5*(uniform_mat[:,4]+1)*(Pos_lim[1]-Pos_lim[0])+Pos_lim[0]

param_mat = np.stack((LBvelMax, UBvelMax, LBvelMin, UBvelMin, Pos),axis=1) 

#print(param_mat)

#count, bins, ignored = plt.hist(param_mat[:,0], 15, density=True)
#plt.plot(bins, np.ones_like(bins), linewidth=2, color='r')
#plt.show()

# Specify specific samples
#for i_var in range(n_vars):
#    param_mat[(2*(i_var)+1,i_var)] = 0.05
#    param_mat[(2*(i_var+1),i_var)] = -0.05

#print(param_mat)

#file = open('design_params.dat', "w")
#file.write(str(param_mat))
#file.close()

# Save input parameters
#np.savetxt('parameters_500.dat', param_mat, delimiter=",", header=str(','.join(headers)))
#scipy.io.savemat('parameters_500.mat', {'param_mat':param_mat})
# Save seed random variables
#np.savetxt('RV_500.dat', uniform_mat, delimiter=",", header=str(','.join(headers)))
#scipy.io.savemat("RV_500.mat", {"uniform_mat":uniform_mat})

