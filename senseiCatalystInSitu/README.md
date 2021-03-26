# SENSEI in situ interface and Catalyst in situ infrastructure

SENSEI: "Write once, use everywhere" 

Catalyst: live visualzation and co-processing

We present the immersive simulation software infrastructure, demonstrating the linkages between solver and co-processing tools to enable live, reconfigerable visualization and live, reconfigurable problem definition. 

We use the fluid solver PHASTA. The purpose of this repo is to demonstrate relationship between solver and immersive simulation tools.  

The interaction between PDE solver and immersive simulation tools is divided into the following three directories: 
1. IS_scripts_pde_solver - scripts in solver source (data adaptor)
2. IS_scripts_run_directory  - scripts in run directory (analysis adapter) 
3. IS_sensei_miniadaptor_phasta - adjustments to miniapp in SENSEI source (in situ Bridge)

