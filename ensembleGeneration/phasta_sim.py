import subprocess   # not certain if I need this
import os
import numpy as np
import time         # maybe need this
from datetime import datetime

import re
import shutil
from tempfile import mkstemp
from pathlib import Path

from libensemble.executors.executor import Executor
from libensemble.message_numbers import WORKER_DONE, WORKER_KILL, TASK_FAILED

def read_last_line(filepath):
    """Read list line of statfile"""
    try: 
        with open(filepath, 'rb') as fh:
            line = fh.readlines()[-1].decode().rstrip()
    except Exception:
        line = "" # In case file is empty or not yet created
    return line

def create_run_directory(run_id, nprocs):
    """
    Create PHASTA run directory
    Input: run_id

    link restarts, geometry, mesh, copy solver.inp
    """
    
    orig = "base_0"
    nprocs = "24"
    orig_t_step = "6000"
    dir_name = 'ensemble_500_1/Run-' + str(run_id)

    # Delete the run directory if it exists
    dir_path = Path(dir_name)
    if dir_path.exists() and dir_path.is_dir():
        shutil.rmtree(dir_path)

    # Create directory
    cmd_1 = ['mkdir', dir_name]
    cmd_2 = ['cp', orig + '/solver.inp', dir_name]
    cmd_4p5 = ['ln', '-s', '../../' + orig + '/input.config', dir_name + '/input.config']
    dir_procs = dir_name + '/' + nprocs + '-procs_case'
    cmd_5 = ['mkdir', dir_procs]
    cmd_6 = ['mkdir', dir_name + '/'+'catalyst_data']
    cmd_7 = ['cp', orig + '/' + nprocs + '-procs_case/numstart.dat', dir_procs + '/.']
    cmd_8 = ['cp', orig + '/' + nprocs + '-procs_case/numpe.in', dir_procs + '/.']
    
    #cmd_9 = ['ln', '-fs', '../../../' + orig + '/' + nprocs + '-procs_case/geombc.dat.*', dir_procs + '/.']
    #cmd_10 = ['ln', '-fs', '../../../' + orig + '/' + nprocs + '-procs_case/restart.*', dir_procs + '/.']
    cmd_11 = ['ln', '-s', '../../../' + orig + '/' + nprocs + '-procs_case/senseiPhasta.xml', dir_procs + '/.']

    subprocess.run(cmd_1, shell = False)
    subprocess.run(cmd_2, shell = False)
    subprocess.run(cmd_4p5, shell = False)
    subprocess.run(cmd_5, shell = False)
    subprocess.run(cmd_6, shell = False)
    subprocess.run(cmd_7, shell = False)
    subprocess.run(cmd_8, shell = False)
    
    for i_file in range(int(nprocs)):    
        cmd_9 = ['ln', '-fs', '../../../' + orig + '/' + nprocs + '-procs_case/geombc.dat.'+str(i_file+1), dir_procs + '/.']
        cmd_10 = ['ln', '-fs', '../../../' + orig + '/' + nprocs + '-procs_case/restart.'+orig_t_step+'.'+str(i_file+1), dir_procs + '/.']
        subprocess.run(cmd_9, shell = False)
        subprocess.run(cmd_10, shell = False)
    
    ## When using sync io the above loop is not necessary    
    #subprocess.run(cmd_9, shell = False)
    #subprocess.run(cmd_10, shell = False)
    subprocess.run(cmd_11, shell = False)

    print("Created directory for run: ", run_id)

def sed_line(pattern, replace, source, dest=None): 
    
    """ Reads a source file and writes the destination file. 
    
    Checks all lines, replaces txt with pattern with replace. 
    
    Args: 
        pattern (str): patter to match (can be re.pattern)
        replace (str): replacement str
        source  (str): input filename
        dest    (str): destination filename, if not given, source will be overwritten
        """
    fin = open(source, 'r')
        
    if dest: 
        fout = open(dest, 'w')
    else: 
        fd, name = mkstemp()
        fout = open(name, 'w')
            
    for line in fin:
        fout.write(replace+'\n' if pattern in line else line)        
        
    try:
        fout.writelines(fin.readlines())
    except Exception as E:
        raise E

    fin.close()
    fout.close()

    if not dest: 
        shutil.move(name, source)

def read_inputs_from_file(run_id):
    """
    read phasta sim specific inputs from file
    """
    file_name = 'input_data/parameters_500.dat'

    ## Read inputs from file
    with open(file_name) as fn:
        x1 = fn.readlines()[run_id]
        x2 = np.array(x1.rstrip('\n').split(','))

    return x2

def replace_solver_strings(find_strings, field_variables, source):
    for i_replace in range(len(find_strings)):
         sed_line('     '+find_strings[i_replace]+': ','     '+find_strings[i_replace]+':  '+field_variables[i_replace], source)
        
def edit_solver_inp(run_id, x): 
    """
    Edit solver.inp with run specific inputs
    """
    
    dir_name = 'ensemble_500_1/Run-' + str(run_id)
    
    source = dir_name + '/solver.inp'
   
    P1 = str(x[0])+'  '+str(x[1])
    P2 = str(x[2])+'  '+str(x[3])
    P3 = str(x[4])

    field_variables = [P1, P2, P3]
    field_strings = ['Blower Maximum u_normal', 'Blower Minimum u_normal', 'Upper Blower Position']
    
    replace_solver_strings(field_strings, field_variables, source)

    print("Edited solver.inp for run: " + str(run_id))

def phasta_call(run_id, NPROCS):
    """
    Call phasta 

    """
    dir_name = 'ensemble_500_1/Run-' + str(run_id)

    LOGDIR = "/logs"
    filename = dir_name + '/' + str(NPROCS)+'-procs_case/numstart.dat'
        
    with open(filename, 'r') as filehandle:
        TS = str.strip(str(filehandle.read()))
    
    # Check if log_directory is there
    if os.path.exists(dir_name+LOGDIR) == False:
        print("Creating new directory for PHASTA logs in {}".format(dir_name+LOGDIR))
        subprocess.run(['mkdir', dir_name+LOGDIR], shell=False)

    subprocess.run(['cp', dir_name + '/solver.inp', dir_name+LOGDIR+'/solver.inp.'+TS],shell=False)

    ## Executor
    exctr = Executor.executor

    # Submit our simulation for execution. 
    P_out = './'+LOGDIR+'/phasta_out.'+TS
    P_time = './'+LOGDIR+'/phasta_time.'+TS
    
    cmd_2 = ['date']

    os.chdir(dir_name)

    f = open(P_time,'a')
    subprocess.run(cmd_2, shell=False, stdout=f, stderr=f)
    f.close()
    
    task = exctr.submit(calc_type='sim', num_procs=NPROCS, stdout=P_out, stderr=P_out, wait_on_run=True)

    # Adjust polling to function for PHASTA
    timeout_sec = 600
    poll_interval = 2
    while not task.finished:
        line = read_last_line(P_out)
        if line == "phasta.cc - last call before finalize!":
            task.kill(wait_time=2)
        elif task.stdout_exists():
            if 'Error' in task.read_stdout():
                task.kill()
        elif task.runtime > timeout_sec:
            task.kill()
        else: 
            time.sleep(poll_interval)
            task.poll()

    if task.finished:
        if task.state == 'FINISHED':
            print("Task {} completed".format(task.name))
            calc_status = WORKER_DONE
        elif task.state == 'FAILED':
            print("Warning: Task {} failed: Error code {}". format(task.name, task.errcode))
            calc_status = TASK_FAILED
        elif task.state == 'USER_KILLED':
            print(datetime.now().hour,':',datetime.now().minute,':',datetime.now().second,'.', datetime.now().microsecond, " Warning: Task {} has been intentionally killed".format(task.name))
            calc_status = WORKER_KILL
        else:
            print("Warning: Task {} in unknown state {}. Error code {}".format(task.name, task.state, task.errcode))

    f = open(P_time,'a')
    subprocess.run(cmd_2, shell=False, stdout=f, stderr=f)
    f.close()

    print(task.state)
    os.chdir('../../')

    return calc_status

def phasta_evaluate(H, persis_info, sim_specs, _):
    
    """
    Evalues PHASTA test case
    H           - History array. Updated by workers with gen_f and sim_f inputs and outputs. H is passed to generator in case user wants to generate new samples based on previous data
    persis_info - Dictionary with worker-specific information. 
    gen_specs   - Dictionary with user-defined and operational parameters for gen_f. 
    LibE_info   - might not need this
    """
    
    out = np.zeros(1, dtype=sim_specs['out'])
    NPROCS = 24
    
    print("Simulation number: "+ str(int(H['x'][0])))
    
    # Identify simulation number
    run_id = int(H['x'][0])

    create_run_directory(run_id, NPROCS)
    
    phasta_inputs = read_inputs_from_file(run_id)
    
    edit_solver_inp(run_id, phasta_inputs)

    dir_name = 'Run-' + str(run_id)
    print(datetime.now().hour,':',datetime.now().minute,':',datetime.now().second,'.', datetime.now().microsecond, " Running simulation for {}".format(dir_name))
    
    phasta_call(run_id, NPROCS)
    
    print(datetime.now().hour,':',datetime.now().minute,':',datetime.now().second,'.', datetime.now().microsecond, " Completed simulation for {}".format(dir_name))
     
    out['y'] = 0.8

    return out, persis_info
