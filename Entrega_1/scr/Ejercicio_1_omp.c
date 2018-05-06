/*
1. Resolver con Pthreads y OpenMP la siguiente expresión:
                    R = AA
Donde A es una matriz de NxN. Analizar el producto AA y utilizar la estrategia
que proporcione el mejor tiempo de ejecución.
Evaluar N=512, 1024 y 2048.
*/
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include"tiempo.h"
#include"cpu.h"
/* Time in seconds from some point in the past */
double dwalltime();
void print_m(double *,int);
unsigned int N;
int total = 1;
double *A,*R;

int main(int argc, char *argv[])
{
  double timetick;
  double time_secuencial;
  double time_parallel;
  double speedup;
  unsigned int i,j,k;
  char filename[10];
  FILE *fp;
  //Controla los argumentos al programa
  if (argc < 3)
  {
    printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
    return 0;
  }
  N=atoi(argv[1]);
  int numThreads = atoi(argv[2]);
  omp_set_num_threads(numThreads);

  total = N/numThreads;

  sprintf(filename, "../docs/metricas/Ej1_Omp_%d_%d.txt", N,numThreads);
  fp = fopen( filename, "a+");
  // si el archivo está vacio imprimimos la cabecera
  if(fgetc(fp) == EOF)
  {
    fprintf(fp, "|N\t|pth\t|t_sec\t|t_par\t|sp_Up\t|efic\t|CPU\t|\n");
  }
  fprintf(fp, "|%d\t|%d\t",N,numThreads);

  //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  R=(double*)malloc(sizeof(double)*N*N);

  //Inicializa las matrices A, B,C en 1
 for(i=0;i<N;i++)
 {
  for(j=0;j<N;j++)
  {
    A[i*N+j]=rand()%100+1;
    R[i*N+j]=0;
  }
 }

 timetick = dwalltime();

//printf("Matriz original\n");
//print_m(A,N);

/*
 for(i=0;i<N;i++)
 {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
         printf("|(%d,%d,%d) = %.0f + %0.f * %.0f|", i,j,k , R[i*N+j] , (A[i*N+k]), (A[j+k*N]));
	       R[i*N+j] =  R[i*N+j] + (A[i*N+k])*(A[j+k*N]);
         printf(" = |%.0f\n",R[i*N+j]);
    }
    printf("(%d,%d) = %.0f\n", i,j, R[i*N+j]);
   }
   printf("\n" );
  }
*/
/* MULTIPLICACION */
for(i=0;i<N;i++)
{
  for(j=0;j<N;j++)
  {
   for(k=0;k<N;k++)
   {
        R[i*N+j] =  R[i*N+j] + (A[i*N+k])*(A[j+k*N]);
   }
  }
}
time_secuencial = dwalltime() - timetick;
//printf("Matriz resultante\n");
//print_m(R,N);
printf("Tiempo en segundos secuencial %f \n", time_secuencial);
fprintf(fp, "|%.3f\t",time_secuencial);
//Proceso en paralelo
//Borrado de Matriz resultante
for(i=0;i<N;i++)
{
 for(j=0;j<N;j++)
 {
   R[i*N+j]=0;
 }
}
timetick = dwalltime();
#pragma omp parallel for collapse(3)
for(i=0;i<N;i++)
{
  for(j=0;j<N;j++)
  {
   for(k=0;k<N;k++)
   {
        R[i*N+j] =  R[i*N+j] + (A[i*N+k])*(A[j+k*N]);
   }
  }
}
time_parallel = dwalltime() - timetick;
//printf("Matriz resultante\n");
//print_m(R,N);
printf("Tiempo en segundos paralelo %f \n", time_parallel);
speedup = time_secuencial/time_parallel;
printf("SpeedUp con %d hilos: %f \n",numThreads,speedup);
fprintf(fp, "|%.3f\t|%.3f\t|%.3f\t",time_parallel,speedup,speedup/numThreads);
fprintf(fp,"|%s\t|\n",cpu_id());
fclose(fp);
free(A);
free(R);
}

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
