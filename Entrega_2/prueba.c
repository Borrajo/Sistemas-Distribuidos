#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "mpi.h"

void print_m(double *M, int dim)
{
  int i,j;
  for(i=0;i<dim;i++)
  {
   for(j=0;j<dim;j++)
   {
     printf("|%.0f\t",M[i*dim+j]);
   }
   printf("|\n");
  }
}

int main( int argc, char *argv[]){
  double *A,*B,*R;
  int i, j;
  int N = 4;
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  R=(double*)malloc(sizeof(double)*N*N);
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      A[i*N+j]=0;
      B[i*N+j]=0;
      R[i*N+j]=0;
    }
  }
  int myrank;
  int cantProcesos;
  int total = 2	;
  MPI_Status status;
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

  if (myrank == 0){
    int i,j,k;
    for(i=0;i<N;i++){
      for(j=0;j<N;j++){
        A[i*N+j]=rand()%100+1;
        B[i+j*N]=rand()%100+1;
      }
    }
    MPI_Send(B, N*N, MPI_DOUBLE, 1, 99, MPI_COMM_WORLD);
    MPI_Send(A+((N*N)/2), (N*N)/2, MPI_DOUBLE, 1, 99, MPI_COMM_WORLD);
    printf("%d my rank\n", myrank);
    print_m(A,N);
    printf("%d my rank\n", myrank);
    print_m(B,N);

    for(i=myrank*total;i<(myrank+1)*total;i++){
      for(j=0;j<N;j++){
        for(k=0;k<N;k++){
      	  R[i*N+j] =  R[i*N+j] + (A[i*N+k])*(B[k+j*N]);
        }
      }
    }

    MPI_Recv(R+((N*N)/2), (N*N)/2, MPI_DOUBLE, 1, 99, MPI_COMM_WORLD, &status); 
    printf("%d my rank\n", myrank);
    print_m(R,N);
    
  }else if (myrank == 1){
    
    int i,j,k;
    MPI_Recv(B, N*N, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, &status); 
    MPI_Recv(A+((N*N)/2), (N*N)/2, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, &status);
    

    for(i=myrank*total;i<(myrank+1)*total;i++){
      for(j=0;j<N;j++){
        for(k=0;k<N;k++){
      	  R[i*N+j] =  R[i*N+j] + (A[i*N+k])*(B[k+j*N]);
        }
      }
    }
    MPI_Send(R+((N*N)/2), (N*N)/2, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
  }
  free(A);
  free(B);
  free(R);
  MPI_Finalize();
  return 0;
}	
