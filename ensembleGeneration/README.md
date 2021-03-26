# Ensemble Generation

Implement libEnsemble with PHASTA!

"libEnsemble is a library to coordinate the concurrent evaluation of dynamic ensembles of calculations" 
https://libensemble.readthedocs.io/en/master/

In essence: If you seek to evaluate numerous PHASTA simulations on a HPC then this tool is for you. 

Installation: 
* See libEnsemble Docs

##Phasta Implementation (Cooley):

Setup custom scripts for your simulations \
The following scripts were designed for Phasta ensemble simulation on Argonne National Computing Facility's Cooley resource. 

### submit_ensemble_cooley.sh - 
Submission script - tailor to specific job and resource. On some HPC systems the job submission requires a bash script.
All this script does is run manager.sh

### manager_sh.sh
Setup the appropriate environment, ie with python3.6 and libEnsemble. In this case we use a custom conda environment.
Record worker_list if on Cooley. \
call calling_func.py 

### Calling_func.py
launch libEnsemble. Specify number of workers, number of simulations and simulations executable. 

### allocation_func.py 
Responsible for determining next simulation to run - in our case we just increment by 1. 

### phasta_sim
Performs a Phasta simulation. A fairly involved process that: 
- Creates a run (simulation) directory
- Reads inputs from file and edits solver.inp
- Calls Phasta

Adjust according to your phasta simulation setup. 

### gather_ensemble
Optional script to collect run data - for instance if an airfoil coeficient of pressure vector is saved, store these and save to .txt .mat or desired file format. 


