# nautiliusFlow
Tools to mitigate traditional partial differential equation (PDE) solver workflow inefficiences 

This repository serves as a demonstration of an immersive (in situ) simulation software framework on a 2D agressive subsonic diffuser in the fluid solver PHASTA. 

Our objective is to provide a template so that immersive simulation tools may be applied in other PDE solvers, and to other problems. 

Immersive simulation tools
1. SENSEI in situ interface
2. Catalyst in situ infrastructure
3. libEnsemble efficient ensemble generation
4. Chaospy python library for UQ techniques

# Software Tools 

## Immersive Simulation

Scripts necessary for the SENSEI in situ interface and Catalyst in situ infrastructure to operate with PHASTA. Together these tools enable live, reconfigurable visualization and live, reconfigurable problem definition that promote rapid design space exploration. 

Computational steering allows practitioners to define design space limits and select quantities of interest that inform ensemble simulation. 

The in situ SENSEI and Catalyst links to PHASTA solver can be found [here](senseiCatalystInSitu).

## Ensemble Generation

Implement [libEnsemble with PHASTA](ensembleGeneration)!
  
"libEnsemble is a library to coordinate the concurrent evaluation of dyanmic ensembles of calculations" https://libensemble.readthedocs.io/en/master/
 
In essence: if you seek to evaluate numerous PHASTA simulations on a HPC then this tool is for you.

## Sensitivity Analysis

We show use of chaospy, an open source uncertainty quantification Python library to conduct a global sensitivity analysis. 

[Chaospy Senstivity Analysis](sensitivityAnalysis)
