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
void print_v(int *,int);
unsigned int N;
int total = 1;
int *V;

int main(int argc, char *argv[]){
  double timetick;
  double time_secuencial;
  double time_parallel;
  double speedup;
  unsigned int i,j,k;
  unsigned int pares = 0;
  char filename[10];
  FILE *fp;

  //Controla los argumentos al programa
  if (argc < 3)  {
    printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
    return 0;
  }

  N=atoi(argv[1]);
  int numThreads = atoi(argv[2]);
  omp_set_num_threads(numThreads);
  total = N/numThreads;

  sprintf(filename, "../docs/metricas/Ej3_Omp_%d_%d.txt", N,numThreads);
  fp = fopen( filename, "a+");
  // si el archivo está vacio imprimimos la cabecera
  if(fgetc(fp) == EOF)
  {
    fprintf(fp, "|N\t|pth\t|t_sec\t|t_par\t|sp_Up\t|efic\t|CPU\t|\n");
  }
  fprintf(fp, "|%d\t|%d\t",N,numThreads);

  //Aloca memoria para el vector
  V=(int*)malloc(sizeof(int)*N);

  for(i=0;i<N;i++)
  {
     V[i]=rand()%10000+1;
  }

  timetick = dwalltime();

  for(i=0; i<N; i++){
   if(V[i] % 2 == 0){
    pares++;
   }
  }
  time_secuencial = dwalltime() - timetick;

  printf("Tiempo en segundos secuencial %f \n", time_secuencial);
  fprintf(fp, "|%.3f\t",time_secuencial);

  timetick = dwalltime();
  pares = 0;
  #pragma omp parallel for reduction(+:pares)
    for(i=0; i<N; i++){
     if(V[i]%2==0){
      pares++;
     }
    }

  time_parallel = dwalltime() - timetick;

  printf("Tiempo en segundos paralelo %f \n", time_parallel);
  speedup = time_secuencial/time_parallel;
  printf("SpeedUp con %d hilos: %f \n",numThreads,speedup);
  fprintf(fp, "|%.3f\t|%.3f\t|%.3f\t",time_parallel,speedup,speedup/numThreads);
  fprintf(fp,"|%s\t|\n",cpu_id());
  fclose(fp);
  free(V);
}
