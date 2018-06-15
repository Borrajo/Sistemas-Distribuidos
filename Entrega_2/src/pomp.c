#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

void omp_mult(double *A, double *B, double *L, double *C, double *D, double *U, double *M, int total, int N, double ul, int miID){
 int i,j,k;
 #pragma omp parallel for collapse(2) private(k)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     for(k=0;k<N;k++){
         M[i*N+j] =  M[i*N+j] + (A[i*N+k] * B[k+j*N]);
     }
   }
 }

 #pragma omp parallel for collapse(2) private(k)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     for(k=0;k<=(miID*total)+i;k++){
       M[i*N+j] =  M[i*N+j] + (L[i*N+k] * C[k+j*N]);
     }
   }
 }

 #pragma omp parallel for collapse(2) private(k)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     for(k=0;k<=j;k++){
       M[i*N+j] =  M[i*N+j] + (D[i*N+k])*(U[k+(j*(j+1))/2]);
     }
   }
 }

 #pragma omp parallel for collapse(2)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     M[i*N+j] =   M[i*N+j] * ul;
   }
 }
}

double omp_prom(double *U, double *L, int N){
 int i,j;
  double total_u = 0 , total_l = 0;
 
 #pragma omp parallel for collapse(2) reduction(+:total_u),reduction(+:total_l)

  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      if(i<=j)
      {
        total_u = total_u + U[i+j*(j+1)/2];
      }
      if(i>=j)
      {
        total_l = total_l + L[i*N+j];
      }
    }
  }
  return (total_u/(N*N)) * (total_l/(N*N));
}
