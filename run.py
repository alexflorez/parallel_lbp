import sys
import os
from subprocess import Popen, PIPE, STDOUT
import numpy as np

programs = ['lbp', 'openmp_lbp_static', 'openmp_lbp_dynamic', 'openmp_lbp_guided', 
            'openmp_lbp_static_chunk', 'openmp_lbp_dynamic_chunk', 'openmp_lbp_guided_chunk', 'mpi_lbp']

processes = {4: "compute-0-0", 
             8: "compute-0-0", 
             12: "compute-0-0,compute-0-6", 
             16: "compute-0-0,compute-0-6",
             20: "compute-0-0,compute-0-6,compute-0-5",
             24: "compute-0-0,compute-0-6,compute-0-5",
             28: "compute-0-0,compute-0-6,compute-0-5,compute-0-8",
             32: "compute-0-0,compute-0-6,compute-0-5,compute-0-8",
             36: "compute-0-0,compute-0-6,compute-0-5,compute-0-8",
             40: "compute-0-0,compute-0-6,compute-0-5,compute-0-8",
             44: "compute-0-0,compute-0-6,compute-0-5,compute-0-8"}

program = sys.argv[1] if len(sys.argv) > 1 else 'lbp'
image = sys.argv[2] if len(sys.argv) > 2 else 'midrib40.png'
trials = int(sys.argv[3]) if len(sys.argv) > 3 else 50
nprocs = int(sys.argv[4]) if len(sys.argv) > 4 else 4

if program in programs:
    cmd = './{} {}'.format(program, image)
    
    mpirun = os.environ.get('MPIRUN', 'mpiexec')
    if 'mpi' in program:
        cmd = 'mpiexec -n {} -H {} {} {}'.format(nprocs, processes[nprocs], program, image)

    if 'openmp' in program:
        cmd = './{} {} {}'.format(program, image, nprocs)
        
    print(cmd)
    times = np.zeros(trials)
    for i in range(trials):
        p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT)
        # subprocess.call(cmd, shell=True)
        output = p.stdout.read()
        try:
            times[i] = float(output)
        except ValueError as ve:
            pass
    print(times)
    print("mean: {}".format(np.mean(times)))
    print("std: {}".format(np.std(times)))
