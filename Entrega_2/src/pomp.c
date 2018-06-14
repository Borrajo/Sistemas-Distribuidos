#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

void omp_mult(double *A, double *B, double *L, double *C, double *D, double *U, double *M, int total, int N, double ul){
 int i,j,k;
 #pragma omp parallel for collapse(3) private(k)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     for(k=0;k<N;k++){
         M[i*N+j] =  M[i*N+j] + (A[i*N+k] * B[k+j*N]);
     }
   }
 }

 #pragma omp parallel for collapse(3) private(k)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     for(k=0;k<=i;k++){
       M[i*N+j] =  M[i*N+j] + (L[k+(i*(i+1))/2] * C[k+j*N]);
     }
   }
 }

 #pragma omp parallel for collapse(3) private(k)
 for(i=0;i<total;i++){
   for(j=0;j<N;j++){
     for(k=0;k<=j;k++){
       M[i*N+j] =  M[i*N+j] + (D[i*N+k])*(U[k+(j*(j+1))/2]);
     }
     M[i*N+j] =   M[i*N+j] * ul;
   }
 }
}

/*void omp_prom(){
 int i,j;
 
 #pragma omp parallel for collapse(2) private(k) reduction(+:sumU)
  double total_u = 0 , total_l = 0;
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      if(i<=j)
      {
        total_u = total_u + U[i+j*(j+1)/2];
      }
      if(i>=j)
      {
        total_l = total_l + L[j+(i*(i+1))/2];
      }
    }
  }
  *ul = total_u/N * total_l/N;
}*/
