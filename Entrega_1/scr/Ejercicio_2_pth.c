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
	At:colm
*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"tiempo.h"
#include"cpu.h"

void multiplicacion(int);
void multp_triangular_U(int);
void multp_triangular_L(int);
void multp_DU_F(int);
void multp_LB_E(int);
void trasponer(int);
void multp_A_A(int);
void multp_AA_C(int);
void sum_todo(int);
void *calculo(void *arg);

unsigned int N;
double sumU = 0;
double sumL = 0;
double sumB = 0;
int total = 1;
int num[]= {0,1,2,3,4,5,6,7};
pthread_mutex_t mutexsum;
double *A,*B,*C,*D,*E,*F,*DU,*DUF,*LB,*AA,*AAC,*AT,*U,*L;
double b,u,l,ul;
pthread_barrier_t barrera;
pthread_barrier_t barrera2;
pthread_barrier_t barrera3;
pthread_barrier_t barrera4;
pthread_barrier_t barrera5;

void multp_triangular_L(int id)
{
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
}

void multp_triangular_U(int id)
{
  int i,j,k;
  double parcial = 0;

  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
     if(i<=j)
     {
       parcial = parcial + U[i+j*(j+1)/2];
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
}

void *calculo(void *arg)
{
	int id = *(int*)arg;
	//Multiplicacion de D*U
	multp_triangular_U(id);
	//MULTIPLICACION DE L*B
	multp_triangular_L(id);
  //TRANSPUESTA DE A
  trasponer(id);

  //Estas 3 funciones las ejecuta el programa principal
  if(id == 0)
  {
    sumU/(N*N);
    l = sumL/(N*N);
    ul = u*l;
    b = sumB/(N*N);
  }
	pthread_barrier_wait(&barrera);
  //para cualquier otro calculo son necesarias alguna de estas matrices.

  //MULTIPLICACION DE DU*F
  multp_DU_F(id);
  pthread_barrier_wait(&barrera2);

	//MULTIPLICACION DE LB*E, Suma a DUF y Multiplicacion por el promedio de B
	multp_LB_E(id);
	pthread_barrier_wait(&barrera3);

	//MULTIPLICACION DE A*A
	multp_A_A(id);
	pthread_barrier_wait(&barrera4);

	//MULTIPLICACION DE AA*C
	multp_AA_C(id);
	pthread_barrier_wait(&barrera5);

	//SUMA DE AAC + RESTO
	sum_todo(id);

	pthread_exit(NULL);
}

void multp_DU_F(int id)
{
  int i,j,k ;
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
}

void multp_LB_E(int id)
{
  int i,j,k ;
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
}

void trasponer(int id)
{
  int i,j,k ;

  /* TRANSPUESTA */
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
    AT[i+N*j] = A[i*N+j];
   }
  }
}

void multp_A_A(int id)
{
  int i,j,k ;
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      AA[i*N+j] = AA[i*N+j] + (A[i*N+k])*(AT[k+j*N]);
    }
   }
  }
}

void multp_AA_C(int id)
{
  int i,j,k ;
  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      AAC[i*N+j] =  AAC[i*N+j] + (AA[i*N+k] * C[k+j*N]);
    }
    AAC[i*N+j] =  AAC[i*N+j]*ul;
   }
  }
}

void sum_todo(int id)
{
  int i,j,k ;

  for(i=id*total;i<(id+1)*total;i++)
  {
   for(j=0;j<N;j++)
   {
     DUF[i*N+j] =  DUF[i*N+j] + AAC[i*N+j];
   }
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
  AA=(double*)malloc(sizeof(double)*N*N);
  AT=(double*)malloc(sizeof(double)*N*N);
  AAC=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  L=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  //Inicializa las matrices A
 for(i=0;i<N;i++)
 {
  for(j=0;j<N;j++)
  { D[i*N+j]=rand()%10+1;
    F[i+j*N]=rand()%10+1;
    B[i+j*N]=rand()%10+1;
    E[i+j*N]=rand()%10+1;
    A[i*N+j]=rand()%10+1;
    C[i+j*N]=rand()%10+1;
    DU[i*N+j]=0;
    DUF[i*N+j]=0;
    LB[i*N+j]=0;
    AA[i*N+j]=0;
    AT[i+j*N]=0;
    AAC[i*N+j]=0;
  }
  for(j=i;j<N;j++){
    U[i+(j*(j+1))/2] = rand()%5+1;
  }
  for(j=0;j<=i;j++){
    L[j+(i*(i+1))/2] = rand()%5+1;
  }
 }

 /* CODIGO SECUENCIAL */

 //Inicia el tiempo
 timetick = dwalltime();

 //DU
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
     if(i<=j)
     {
       sumU = sumU + U[i+j*(j+1)/2];
     }
     for(k=0;k<=j;k++)
     {
         DU[i*N+j] =  DU[i*N+j] + (D[i*N+k])*(U[k+(j*(j+1))/2]);
     }
   }
  }

 u = sumU/(N*N);

 //DUF
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      DUF[i*N+j] =  DUF[i*N+j] + (DU[i*N+k] * F[k+j*N]);
    }
   }
  }

 //LB
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
    if(i>=j){
      sumL = sumL + L[j+(i*(i+1))/2];
    }
    sumB = sumB + B[i+j*N];
    for(k=0;k<=i;k++)
    {
        LB[i*N+j] =  LB[i*N+j] + (L[k+(i*(i+1))/2] * B[k+j*N]);
    }
   }
  }

 l = sumL/(N*N);
 b = sumB/(N*N);
 ul = u*l;

 //b(LBE+DUF)
 for(i=0;i<N;i++)
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

 //Transpuesta de A
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
    AT[i+N*j] = A[i*N+j];
   }
  }

 //AA
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      AA[i*N+j] = AA[i*N+j] + (A[i*N+k])*(AT[k+j*N]);
    }
   }
  }

 //ul(AAC)
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
    for(k=0;k<N;k++)
    {
      AAC[i*N+j] =  AAC[i*N+j] + (AA[i*N+k] * C[k+j*N]);
    }
    AAC[i*N+j] =  AAC[i*N+j]*ul;
   }
  }


 //SUMA FINAL
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
     DUF[i*N+j] =  DUF[i*N+j] + AAC[i*N+j];
   }
  }

 time_secuencial = dwalltime() - timetick;
 printf("Tiempo en segundos secuencial %f \n", time_secuencial);
 fprintf(fp, "|%.3f\t",time_secuencial);

 //Resetea las matrices
 for(i=0;i<N;i++)
 {
  for(j=0;j<N;j++)
  {
    DU[i*N+j]=0;
    DUF[i*N+j]=0;
    LB[i*N+j]=0;
    AA[i*N+j]=0;
    AT[i+j*N]=0;
    AAC[i*N+j]=0;
  }
 }
 sumB = 0;
 sumU = 0;
 sumL = 0;
 l = 0;
 u = 0;
 ul = 0;

//Inicia el tiempo
timetick = dwalltime();
//Se crean los hilos para calcular el DU
pthread_mutex_init(&mutexsum, NULL);
pthread_barrier_init(&barrera, NULL, numThreads); //cantidad de procesos en espera
pthread_barrier_init(&barrera2, NULL, numThreads); //cantidad de procesos en espera
pthread_barrier_init(&barrera3, NULL, numThreads); //cantidad de procesos en espera
pthread_barrier_init(&barrera4, NULL, numThreads); //cantidad de procesos en espera
pthread_barrier_init(&barrera5, NULL, numThreads); //cantidad de procesos en espera
for ( t = 0; t < numThreads; t++)
{
  pthread_create(&Hilos[t], NULL, calculo, (void*)&num[t]);
}
for ( t = 0; t < numThreads; t++)
{
  pthread_join(Hilos[t],NULL);
}

time_parallel = dwalltime() - timetick;
printf("Tiempo en segundos paralelo %f \n", time_parallel);
speedup = time_secuencial/time_parallel;
printf("SpeedUp con %d hilos: %f \n",numThreads,speedup);
fprintf(fp, "|%.3f\t|%.3f\t|%.3f\t",time_parallel,speedup,speedup/numThreads);
fprintf(fp,"|%s\t|\n",cpu_id());

pthread_exit(NULL);
fclose(fp);
free(B);
free(L);
free(A);
free(LB);
free(E);
free(AT);
free(AA);
free(C);
free(D);
free(U);
free(DU);
free(F);
free(DUF);
free(AAC);
}
