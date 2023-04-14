# An implementation of the local binary pattern (LBP) operator in a parallel environment

## 1. List of files
- `lbp.c`\
Serial version of the LBP operator feature extraction
- `openmp_lbp.c`\
OpenMP version of the LBP operator feature extraction
- `mpi_lbp.c`\
MPI and OpenMP version of the LBP operator feature extraction

- `stb_image.h`\
Single file to loading/decoding images in C/C++ https://github.com/nothings/stb

- `stopif.h`\
Macro to evaluate assertions

- `run.py`\
File to perform multiple executions of the programs lbp, openmp_lbp and mpi_lbp

## 2. How to compile

Compile in Linux with `gcc`, need to be linked to the math library `-lm`\
`$ gcc -std=c99 -O3 -Dtest_lbp lbp.c -lm -o -lbp`

Compile with OpenMP support\
`$ gcc -std=c99 -O3 openmp_lbp.c -lm -fopenmp -o -openmp_lbp`

Compile MPI + OpenMP support in Linux\
`$ mpicc -std=c99 -O3 mpi_lbp.c -fopenmp -o mpi_lbp`

## 3. Running the programs

Run the serial version:\
`$ program <name_image>`\
`$ ./lbp midrib40.png`

Run the OpenMP version:\
`$ program <name_image> <nthreads>`\
`$ ./openmp_lbp midrib40.png 8`

Run the MPI and OpenMP version:\
`$ mpiexec -n <nprocs> -H <list_hosts> program <name_image>`\
`$ mpiexec -n 8 -H compute-0-0 mpi_lbp midrib40.png`\
`$ mpiexec -n 16 -H compute-0-0,compute-0-1 mpi_lbp midrib40.png`

## 4. Images
Images taken from **Midrib** dataset http://scg.ifsc.usp.br/dataset/Midrib.php:

`midrib40`:	Solanum_lycocarpum/Solanum_Mx10_C1_NP.png, size: 5066 x 5179, 40.8 MB\
`midrib80`: 	Tocoyena_formosa/Tocoyena_Mx10_M3_C1_NP.png, size: 7328 x 9771, 84 MB\
`midrib120`: 	Byrsonima_subterranea/C_Proc_Byrsonima_Mx10_P2_C02_NP.png, size: 9551 x 9935, 120 MB

## 5. Profiling
To profile the serial version without compiler optimization, add `-pg` flag when compiling\
`$ gcc -std=c99 -Dtest_lbp -pg lbp.c -lm -o lbp_profile`

Run the program normally\
`$ ./lbp_profile midrib80.png`\
It produces a file `gmon.out`. 

Then run `gprof` to produce a readable file\
`$ gprof lbp_profile > profile.txt`\
The file profile.txt contains information about the running time of the program.
