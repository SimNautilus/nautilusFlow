# Sensitivity Analysis

Demonstrate use of ensemble output for sensitivity analysis via chaospy. 

"Chaospy is a numerical tool for performing uncertainty quantification using polynomial chaos expansions, advanced Monte Carlo methods implemented in Python. It also include a full suite of tools for doing low-discrepancy sampling, quadrature creation, polynomial manipulations, and a lot more."

https://chaospy.readthedocs.io/en/master/#installation

Installation: 
* See chaospy documentation

Applicable to any simulation output. In this application we work with two sets of data: 
* Input stocastic samples 
* Output QoI 

Our objective with this data is to: 
* Construct a polynomial chaos (PC) expansion with minimal validation error
* Generate PC based sobal indices to assess the relative influence of different design parameters

See Jupyter notebook for chaospy demonstration
