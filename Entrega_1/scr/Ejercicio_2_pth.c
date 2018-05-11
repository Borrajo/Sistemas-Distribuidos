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
/* Almacenamiento de las matrices en memoria */
/*  A:fila  E:colm
    B:colm  F:colm
    C:colm  L:fila
    D:fila  U:colm
*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"tiempo.h"
#include"cpu.h"

void print_m(double *,int);
void *promedio(void *arg);
void *multiplicacion(void *arg);
void *multp_triangular_U(void *arg);
void *multp_triangular_L(void *arg);
void *multp_DU_F(void *arg);

unsigned int N;
double sumU = 0;
double sumL = 0;
double sumB = 0;
int total = 1;
int num[]= {0,1,2,3,4,5,6,7};
pthread_mutex_t mutexsum;
double *A,*B,*C,*D,*E,*F,*DU,*DUF,*LB,*U,*L;
double b,u,l;

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
  sumU = sumU + parcial;
  pthread_mutex_unlock (&mutexsum);
  pthread_exit(NULL);
}

void *multp_triangular_L(void *arg)
{
  /* para acceder a la posicion U[i,j] = U[i*N+j-(i*(i+1))/2]*/
  int id = *(int*)arg;
  int i,j,k;
  double parcialL = 0;
  double parcialB = 0;
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
    if(i>=j){
      parcialL = parcialL + L[j+(i*(i+1))/2];
    }
    parcialB = parcialB + B[i+j*N];
    for(k=0;k<=i;k++)
    {
        LB[i*N+j] =  LB[i*N+j] + (L[k+(i*(i+1))/2] * B[k+j*N]);
    }
   }
  }
  //acceso al mutex para actualizar el total
  pthread_mutex_lock (&mutexsum);
  sumL = sumL + parcialL;
  sumB = sumB + parcialB;
  pthread_mutex_unlock (&mutexsum);
  pthread_exit(NULL); 
}

void *multp_triangular_U(void *arg)
{
  /* para acceder a la posicion L[i,j] = U[i*N+j-(i*(i+1))/2]*/
  int id = *(int*)arg;
  int i,j,k;
  double parcial = 0;
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
     if(i<=j)
     {
       parcial = parcial + U[i+j*(j+1)/2];
       //printf("U(%d)[%d,%d]=%.0f\n",id,i,j,U[i+j*(j+1)/2]);
     }
     for(k=0;k<=j;k++)
     {
         DU[i*N+j] =  DU[i*N+j] + (D[i*N+k])*(U[k+(j*(j+1))/2]);
     }
   }
  }
  //acceso al mutex para actualizar el total
  pthread_mutex_lock (&mutexsum);
  sumU = sumU + parcial;
  pthread_mutex_unlock (&mutexsum);
  pthread_exit(NULL);
}

void *multp_DU_F(void *arg)
{
  int id = *(int*)arg;
  int i,j,k ;
  //printf("\nid: %d , desde: %d, hasta: %d\n ",id,id*total,(id+1)*total);
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      DUF[i*N+j] =  DUF[i*N+j] + (DU[i*N+k] * F[k+j*N]);
    }
   }
  }
  pthread_exit(NULL);
}

void *multp_LB_E(void *arg)
{
  int id = *(int*)arg;
  int i,j,k ;
  //printf("\nid: %d , desde: %d, hasta: %d\n ",id,id*total,(id+1)*total);
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      DUF[i*N+j] =  DUF[i*N+j] + (LB[i*N+k] * E[k+j*N]);
    }
    DUF[i*N+j] =  DUF[i*N+j]*b;
   }
  }
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

void print_mT(double *M, int dim)
{
  int i,j;
  for(i=0;i<dim;i++)
  {
   for(j=0;j<dim;j++)
   {
     printf("|%.0f\t",M[i+j*dim]);
   }
   printf("|\n");
  }
}

void print_mU(double *M, int dim)
{
  int i,j;
  for(i=0;i<dim;i++)
  {
   for(j=0;j<dim;j++)
   {
     if(i<=j){
      printf("|%.0f\t",M[i+(j*(j+1))/2]);
     }else{
      printf("|0\t");
     }
   }
   printf("|\n");
  }
}

void print_mL(double *M, int dim)
{
  int i,j;
  for(i=0;i<dim;i++)
  {
   for(j=0;j<dim;j++)
   {
     if(i>=j){
      printf("|%.0f\t",M[j+(i*(i+1))/2]);
     }else{
      printf("|0\t");
     }
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
  DU=(double*)malloc(sizeof(double)*N*N);
  DUF=(double*)malloc(sizeof(double)*N*N);
  LB=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  L=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  //Inicializa las matrices A
 for(i=0;i<N;i++)
 {
  for(j=0;j<N;j++)
  {
    D[i*N+j]=rand()%10+1;
    F[i+j*N]=rand()%10+1;
    B[i+j*N]=rand()%10+1;
    E[i+j*N]=rand()%10+1;
    DU[i*N+j]=0;
    DUF[i*N+j]=0;
    LB[i*N+j]=0;
  }
  for(j=i;j<N;j++){
    U[i+(j*(j+1))/2] = rand()%5+1;    
  }
  for(j=0;j<=i;j++){
    L[j+(i*(i+1))/2] = rand()%5+1; 
  }
 }
 printf("Matriz D\n" );
 print_m(D,N);
 printf("Matriz U\n" );
 print_mU(U,N);
 printf("Matriz F\n" );
 print_mT(F,N);
 printf("Matriz L\n" );
 print_mL(L,N);
 printf("Matriz B\n" );
 print_mT(B,N);
 printf("Matriz E\n" );
 print_mT(E,N);
//Inicia el tiempo
timetick = dwalltime();
//Se crean los hilos para calcular el DU
pthread_mutex_init(&mutexsum, NULL);
for ( t = 0; t < numThreads; t++)
{
  pthread_create(&Hilos[t], NULL, multp_triangular_U, (void*)&num[t]);
}
for ( t = 0; t < numThreads; t++)
{
  pthread_join(Hilos[t],NULL);
}
u = sumU/(N*(N+1)/2);

//MULTIPLICACION DE DU*F
for ( t = 0; t < numThreads; t++)
{
  pthread_create(&Hilos[t], NULL, multp_DU_F, (void*)&num[t]);
}
for ( t = 0; t < numThreads; t++)
{
  pthread_join(Hilos[t],NULL);
}
printf("promedio de U %.2f / %d = %.2f\n",sumU,(N*(N+1)/2), u);
printf("Matriz DUF\n" );
print_m(DUF,N);

//MULTIPLICACION DE L*B
for ( t = 0; t < numThreads; t++)
{
  pthread_create(&Hilos[t], NULL, multp_triangular_L, (void*)&num[t]);
}
for ( t = 0; t < numThreads; t++)
{
  pthread_join(Hilos[t],NULL);
}
l = sumL/(N*(N+1)/2);
b = sumB/(N*N);

//MULTIPLICACION DE LB*E
for ( t = 0; t < numThreads; t++)
{
  pthread_create(&Hilos[t], NULL, multp_LB_E, (void*)&num[t]);
}
for ( t = 0; t < numThreads; t++)
{
  pthread_join(Hilos[t],NULL);
}

printf("promedio de L %.2f / %d = %.2f\n",sumL,(N*(N+1)/2), l);
printf("promedio de B %.2f / %d = %.2f\n",sumB,N*N, b);
/* -- Fin de calculo del promedio de B --*/
time_secuencial = dwalltime() - timetick;
//printf("Tiempo en segundos secuencial %f \nPromedio : %.2f\n", time_secuencial,sum/cantidad);
//printf("Tiempo en segundos secuencial %f \n", time_secuencial);
/*printf("Matriz DU\n" );
print_m(DU,N);
printf("Matriz F\n" );
print_mT(F,N);
printf("Matriz DUF\n" );
print_m(DUF,N);*/
printf("Matriz LB\n" );
print_m(LB,N);
printf("Matriz DUF\n" );
print_m(DUF,N);
pthread_exit(NULL);
fclose(fp);
free(D);
free(DU);
free(U);
free(DUF);
}
