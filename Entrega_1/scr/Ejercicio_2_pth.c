/*
2. Realizar un algoritmo Pthreads y otro OpenMP que resuelva la expresión:
                  M = ¬(ul).A.A.C + ¬b.L.B.E + ¬b.D.U.F
Donde A, B, C, D, E y F son matrices de NxN. L y U son matrices triangulares de
NxN inferior y superior, respectivamente.¬b es el promedio de los valores de
los elementos de la matriz B y ¬(ul) es el producto de los promedios de los
valores de los elementos de las matrices U y L, respectivamente.
Evaluar N=512, 1024 y 2048.
*/

/*
    Revisando un poco la expresion del segundo y tercer termino puede extrarse como factor comun
    ¬b (L.B.E + D.U.F) de esta forma pueden calcularse LBE por un lado, DUF por otro, luego sumarla
    y al final multiplicar por el promedio
 */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"tiempo.h"
#include"cpu.h"

void print_m(double *,int);
void *promedio(void *arg);
void *multiplicacion(void *arg);

unsigned int N;
double sum = 0;
int total = 1;
int num[]= {0,1,2,3,4,5,6,7};
pthread_mutex_t mutexsum;
double *A,*B,*C,*D,*E,*F;
unsigned int b,u,l;

void *promedio(void *arg)
{
  int id = *(int*)arg;
  int i,j;
  double parcial = 0;
  for(i=id*total;i<(id+1)*total;i++)
  {
    for (j = 0; j < N; j++)
    {
      parcial = parcial + B[i*N+j];
    }
  }
  //acceso al mutex para actualizar el total
  pthread_mutex_lock (&mutexsum);
  sum = sum + parcial;
  pthread_mutex_unlock (&mutexsum);
  pthread_exit(NULL);
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

int main(int argc, char *argv[])
{
  double timetick;
  double time_secuencial;
  double time_parallel;
  double speedup;
  unsigned int cantidad;
  unsigned int i,j,k,t;
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
  total = N/numThreads;
  cantidad = N*N;
  sprintf(filename, "../docs/metricas/Ej2_Pth_%d_%d.txt", N,numThreads);
  fp = fopen( filename, "a+");
  // si el archivo está vacio imprimimos la cabecera
  if(fgetc(fp) == EOF)
  {
    fprintf(fp, "|N\t|pth\t|t_sec\t|t_par\t|sp_Up\t|efic\t|CPU\t|\n");
  }
  fprintf(fp, "|%d\t|%d\t",N,numThreads);
  //se crean los hilos
  pthread_t Hilos[numThreads];
  //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  E=(double*)malloc(sizeof(double)*N*N);
  F=(double*)malloc(sizeof(double)*N*N);
  //Inicializa las matrices A
 for(i=0;i<N;i++)
 {
  for(j=0;j<N;j++)
  {
    B[i*N+j]=rand()%100+1;
  }
 }
//Promedio de matriz de forma paralela
pthread_mutex_init(&mutexsum, NULL);
timetick = dwalltime();
//Se crean los hilos para calcular el promedio de B
for ( t = 0; t < numThreads; t++)
{
  pthread_create(&Hilos[t], NULL, promedio, (void*)&num[t]);
}
for ( t = 0; t < numThreads; t++)
{
  pthread_join(Hilos[t],NULL);
}
pthread_mutex_destroy(&mutexsum);
b = sum/cantidad;
/* -- Fin de calculo del promedio de B --*/
time_secuencial = dwalltime() - timetick;
printf("Tiempo en segundos secuencial %f \nPromedio : %.2f\n", time_secuencial,sum/cantidad);
pthread_exit(NULL);
fclose(fp);
free(A);
}
