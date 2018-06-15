/*
Calcular y analizar, en caso de existir, el desbalance de carga.
El informe debe incluir las tablas con los tiempos de ejecución, el speedup y la
eficiencia.
+ 1 Realizar un algoritmo MPI que resuelva la expresión:
            M = ¬(u.l) * (AB + LC + DU)
    Donde A, B, C y D son matrices de NxN. L y U son matrices triangulares de NxN
inferior y superior, respectivamente.¬u y ¬l son los promedios de los valores de
los elementos de la matrices U y L, respectivamente.
Evaluar N=512, 1024 y 2048.

  fila        columna
  M,A,L,D     B,C,U

  U por columna = U[i + j*(j+1)/2]
 */
#include<stdio.h>
#include<stdlib.h>
#include"tiempo.h"
//#include"cpu.h"
void print_m(double*,int,int);
void calcular_avg(double*,double*,double*,int);
int main(int argc, char **argv)
{
  int i;
  int j;
  int N;
  int k;
  double *M,*A,*B,*L,*C,*U,*D;
  double ul;
  double timetick;
  double time_total;
  N=atoi(argv[1]);
  //Aloca memoria para las matrices
  M=(double*)malloc(sizeof(double)*N*N);
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  L=(double*)malloc(sizeof(double)*N*N);
  //inicializacion de matriz resultante.
  for(i=0;i<N;i++)
  {
    for(j=0;j<N;j++)
    {
      M[i*N+j]=0;
      L[i*N+j]=0;
    }
  }

    printf("|proc N\t| t_comunic\t| t_computo|\n");
    //creacion de datos para las matrices.
    //Inicializa las matrices
    for(i=0;i<N;i++)
    {
      for(j=0;j<N;j++)
      {
        A[i*N+j]=rand()%10+1;
        B[i+j*N]=rand()%10+1;
        C[i+j*N]=rand()%10+1;
        D[i*N+j]=rand()%10+1;
      }
      for(j=i;j<N;j++){
        U[i+(j*(j+1))/2] = rand()%10+1;
      }
      for(j=0;j<=i;j++){
        L[i*N+j] = rand()%10+1;
      }
    }

    //Inicia el tiempo total
    timetick = dwalltime();
    calcular_avg(&ul, U, L, N);

  //AB
   for(i = 0;i<N;i++)
     {
      for(j=0;j<N;j++)
      {
       for(k=0;k<N;k++)
       {
         M[i*N+j] =  M[i*N+j] + (A[i*N+k] * B[k+j*N]);
       }
      }
     }
     //LC
      for(i=0;i<N;i++)
      {
       for(j=0;j<N;j++)
       {
         for(k=0;k<=i;k++)
         {
             M[i*N+j] =  M[i*N+j] + (L[i*N+k] * C[k+j*N]);
         }
       }
      }
      //DU
       for(i=0;i<N;i++)
       {
        for(j=0;j<N;j++)
        {
          for(k=0;k<=j;k++)
          {
              M[i*N+j] =  M[i*N+j] + (D[i*N+k])*(U[k+(j*(j+1))/2]);
          }
	       M[i*N+j] =   M[i*N+j] * ul;
        }
       }
     time_total = dwalltime() - timetick;
     printf("| 0\t| %f\t|\n",time_total);

     printf("tiempo total %f seg\n",time_total);

   free(A);
   free(B);
   free(L);
   free(C);
   free(D);
   free(U);
   free(M);
   return 0;
}

void calcular_avg(double *ul, double *U, double *L, int N)
{
  int i,j;
  double total_u = 0 , total_l = 0;
  for (i = 0; i < N; i++)
  {
    for(j=0;j<N;j++)
    {
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
  *ul = (total_u/(N*N)) * (total_l/(N*N));
}

void print_m(double *M, int dim,int id)
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
