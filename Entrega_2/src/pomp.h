#ifndef __OMP_FUNCTIONS__
#define __OMP_FUNCTIONS__

extern void omp_mult(double *A, double *B, double *L, double *C, double *D, double *U, double *M, int total, int N, double ul, int miID);

extern double omp_prom(double *U, double *L, int N);

#endif
