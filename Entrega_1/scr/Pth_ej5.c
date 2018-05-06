#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//Dimension por defecto de las matrices
int N=100;
int hasta = 1;
unsigned long total_paralelo = 0;
void* contar(void *arg);
int* A;
int num[] = {0,1,2,3,4,5,6,7,8};
pthread_mutex_t mutexsum;
//Para calcular tiempo
double dwalltime()
{
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void *contar(void *arg)
{
  int id = *(int*)arg;
  int i;
  unsigned long parcial = 0;
  //printf("\nid: %d , desde: %d, hasta: %d\n ",id,id*hasta,(id+1)*hasta);
  for(i=id*hasta;i<(id+1)*hasta;i++)
  {
      parcial += A[i];
  }
  //printf("\nid: %d, encontro %d veces el numero %d",id,parcial,X);
  //acceso al mutex para actualizar el total
  pthread_mutex_lock (&mutexsum);
  total_paralelo += parcial;
  pthread_mutex_unlock (&mutexsum);
  pthread_exit(NULL);
}

int main(int argc,char*argv[])
{
 int i,t;
 unsigned long total_secuencial = 0;
 double timetick;
 //Controla los argumentos al programa
 //programa | tamaño del vector | num a buscar | cantidad de hilos
 int PtCount = atoi(argv[2]);
 N = atoi(argv[1]);
 hasta = N/PtCount;
 //se crean los hilos
 pthread_t Hilos[PtCount];
 //Aloca memoria para el vector
 A=(int*)malloc(sizeof(int)*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++)
  {
      A[i] = rand()%100;
  }
  pthread_mutex_init(&mutexsum, NULL);
  //Algoritmo en SECUENCIAL
  timetick = dwalltime();
   //Verifica el resultado
    for(i=0;i<N;i++)
    {
      //printf("%d ,", A[i]);
         total_secuencial += A[i];
    }
    printf("\n(secuencial) Promedio: %d\n",total_secuencial/N);
    printf("(secuencial) Tiempo en segundos %f\n", dwalltime() - timetick);
    // FIN ALGORITMO SECUENCIAL
//Realiza la multiplicacion
  timetick = dwalltime();
  for ( t = 0; t < PtCount; t++)
  {
    pthread_create(&Hilos[t], NULL, contar, (void*)&num[t]);
  }

  for ( t = 0; t < PtCount; t++)
  {
    pthread_join(Hilos[t],NULL);
  }

printf("\n(paralelo) Promedio: %d\n",total_paralelo/N);
printf("(paralelo) Tiempo en segundos %f\n", dwalltime() - timetick);


free(A);
pthread_mutex_destroy(&mutexsum);
pthread_exit(NULL);

 return(0);
}
