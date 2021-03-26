import numpy as np

# Import libEnsemble items
from libensemble.libE import libE
from libensemble.executors.mpi_executor import MPIExecutor
from libensemble.tools import parse_args

from libensemble import libE_logger
libE_logger.set_level('DEBUG')

# Import custom scripts
from phasta_sim import phasta_evaluate as sim_f
from allocation_func import test_alloc as alloc_f

from datetime import datetime

def call_ensemble(nworkers,n_sim):

    exctr = MPIExecutor(central_mode=True) 
	
    # Register simulation executable with executor
    pconf = "/projects/VisComm/fnewberr/build/phasta"
    VER = "IC" 

    sim_app = pconf+'/bin/phasta'+VER+'.exe'
    
    exctr.register_calc(full_path=sim_app, calc_type = 'sim')

    libE_specs = {'nworkers': nworkers, 'comms': 'local'}
    sim_specs = {'sim_f': sim_f, 'in': ['x'], 'out': [('y', float),('calc_status',str)]}
    gen_specs = {}

    # Set up H0 - used as run_ID
    H0 = np.zeros(n_sim, dtype=[('x', float), ('sim_id', int)])
    H0['sim_id'] = range(n_sim)
    
    alloc_specs = {'alloc_f': alloc_f, 'out': [('x', float)]}

    exit_criteria = {'sim_max':len(H0)}

    print(datetime.now().hour,':',datetime.now().minute,':',datetime.now().second,'.', datetime.now().microsecond," Starting libE")

    # Perform the run
    H, persis_info, flag = libE(sim_specs, gen_specs, exit_criteria, alloc_specs=alloc_specs, libE_specs=libE_specs, H0=H0)

    print(datetime.now().hour,':',datetime.now().minute,':',datetime.now().second,'.', datetime.now().microsecond," Ending libE")
    print([i for i in H.dtype.fields])
    print(H)

### Ensemble settings
nworkers = 50 # excludes one master node with python running. In this example there are 50 workes, 2 nodes each. Each worker runs a 24 processor phasta simulation
n_sim = 50

call_ensemble(nworkers, n_sim)
