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
#include<omp.h>
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
double *A,*B,*C,*D,*E,*F,*DU,*DUF,*LB,*AA,*AAC,*AT,*U,*L;
double b,u,l,ul;

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
  sprintf(filename, "../docs/metricas/Ej2_Omp_%d_%d.txt", N,numThreads);
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
/*printf("Matriz TOTAL\n" );
print_m(DUF,N);*/

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
 /*printf("Matriz D\n" );
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
 printf("Matriz A\n" );
 print_m(A,N);
 printf("Matriz C\n" );
 print_mT(C,N);*/
//Inicia el tiempo
timetick = dwalltime();
//Se crean los hilos para calcular el DU

//DU
 #pragma omp parallel for collapse(2) private(k) reduction(+:sumU)
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
    free(D);
    free(U);
 //DUF
 #pragma omp parallel for collapse(2) private(k)
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
    free(DU);
    free(F);
 //LB
 
 #pragma omp parallel for collapse(2) private(k) reduction(+:sumL,sumB)
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
free(B);
free(L);
 //b(LBE+DUF)
 #pragma omp parallel for collapse(2) private(k)
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
free(LB);
free(E);

 //Transpuesta de A
 #pragma omp parallel for collapse(2)
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
    AT[i+N*j] = A[i*N+j];
   }
  }

 //AA
 #pragma omp parallel for collapse(2) private(k)
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
free(A);
free(AT);
 //ul(AAC)
 #pragma omp parallel for collapse(2) private(k)
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
 free(AA);
free(C);

 //SUMA FINAL
 #pragma omp parallel for collapse(2)
 for(i=0;i<N;i++)
  {
   for(j=0;j<N;j++)
   {
     DUF[i*N+j] =  DUF[i*N+j] + AAC[i*N+j];
   }
  }

//printf("promedio de L %.2f / %d = %.2f\n",sumL,(N*(N+1)/2), l);
/*printf("promedio de L %.2f / %d = %.2f\n",sumL,N*N, l);
printf("promedio de B %.2f / %d = %.2f\n",sumB,N*N, b);
printf("ul = %.2f\n",ul);*/
/* -- Fin de calculo del promedio de B --*/
time_parallel = dwalltime() - timetick;
printf("Tiempo en segundos paralelo %f \n", time_parallel);
speedup = time_secuencial/time_parallel;
printf("SpeedUp con %d hilos: %f \n",numThreads,speedup);
fprintf(fp, "|%.3f\t|%.3f\t|%.3f\t",time_parallel,speedup,speedup/numThreads);
fprintf(fp,"|%s\t|\n",cpu_id());
/*printf("Matriz DU\n" );
print_m(DU,N);
printf("Matriz F\n" );
print_mT(F,N);
printf("Matriz LB\n" );
print_m(LB,N);*/
/*printf("Matriz AAC\n" );
print_m(AAC,N);*/
/*printf("Matriz TOTAL\n" );
print_m(DUF,N);*/
fclose(fp);
free(DUF);
free(AAC);
}
