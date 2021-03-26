#!/usr/bin/python

import numpy as np
# parameteric samples for senstivity analysis 
# First run limit cases
# columns include 

headers = ['LBvelMax','UBvelMax','LBvelMin', 'UBvelMin','Viscosity','BlowerPosition'] 

n_vars = len(headers)
n_samps = 6

param_mat = np.zeros((n_samps, n_vars))

# Specify specific samples
#for i_var in range(n_vars):
#    param_mat[(2*(i_var)+1,i_var)] = 0.05
#    param_mat[(2*(i_var+1),i_var)] = -0.05

#print(param_mat)

#file = open('design_params.dat', "w")
#file.write(str(param_mat))
#file.close()

np.savetxt('parameters_limits.dat', param_mat, delimiter=",", header=str(','.join(headers)))
