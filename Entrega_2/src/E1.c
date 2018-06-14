//caracteristicas:
//Raspberry 2b
//  Procesador: ARM Cortex A7 900Mhz
//  RAM: 1GB
//Raspberry 3
//  Procesador: Broadcom BCM2837 1.2Ghz
//  RAM: 1GB
/*
Calcular y analizar, en caso de existir, el desbalance de carga.
El informe debe incluir las tablas con los tiempos de ejecución, el speedup y la
eficiencia.
+ 1 Realizar un algoritmo MPI que resuelva la expresión:
            M = ¬(u.l) * (AB + LC + UD)
    Donde A, B, C y D son matrices de NxN. L y U son matrices triangulares de NxN
inferior y superior, respectivamente.¬u y ¬l son los promedios de los valores de
los elementos de la matrices U y L, respectivamente.
Evaluar N=512, 1024 y 2048.

  fila        columna
  M,A,L,U     B,C,D

  L por fila = L[j+i*(i+1)/2]
  U por fila = U[i*N+j - i*(i+1)/2]
 */
#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include"tiempo.h"
//#include"cpu.h"
void print_m(double*,int,int);

int main(int argc, char **argv)
{
  int miID;
  int NProcs;
  int i;
  int j;
  int N;
  int k;
  int total;
  double *M,*A,*B,*L,*C,*U,*D;
  double u,l;
  double timetick;
  double time_gatter;
  double time_total;
  double time_computo;
  double time_comunicacion;
  MPI_Status status;
  MPI_Request request;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&miID);
  MPI_Comm_size(MPI_COMM_WORLD,&NProcs);
  N=atoi(argv[1]);
  if(NProcs > N)
  {
    NProcs = N;
  }
  total = N/NProcs;
  //Aloca memoria para las matrices
  M=(double*)malloc(sizeof(double)*N*N);
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  L=(double*)malloc(sizeof(double)*(N*(N+1)/2));
  //Aloca memoria para las matrices temporales
  AB=(double*)malloc(sizeof(double)*N*total);
  LC=(double*)malloc(sizeof(double)*N*total);
  DU=(double*)malloc(sizeof(double)*N*total);
  //inicializacion de matriz resultante.
  for(i=0;i<N;i++)
  {
    for(j=0;j<N;j++)
    {
      M[i*N+j]=0;
    }
  }
  //Inicializacion de las matrices temporales
  for(i=0;i<N;i++)
  {
    for(j=0;j<total;j++)
    {
      AB[i*N+j]=0;
      LC[i*N+j]=0;
      DU[i*N+j]=0;
    }
  }
  //el proceso 0 crea y carga los valores de las matrices A,B,C,D,U,L
  if(miID == 0)
  {
    printf("\ntotal: %d; NProcs: %d\n",total,NProcs );
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
        D[i+j*N]=rand()%10+1;
      }
      for(j=0;j<=i;j++){
        L[j+(i*(i+1))/2] = rand()%10+1;
      }
      for(j=0;j>=i;j++){
        U[i*N+j - i*(i+1)/2] = rand()%10+1;
      }
    }
    /*printf("matriz A\n" );
    print_m(A, N, miID);
    printf("matriz B\n" );
    print_m(B, N, miID);*/
    //Inicia el tiempo total
    time_total = dwalltime();
  }
  //TODOS los procesos ejecutan estas sentencias
    time_comunicacion = dwalltime();
    //se envia/recibe B
    MPI_Bcast(B, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //se envia/recibe C
    MPI_Bcast(C, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //se envia/recibe D
    MPI_Bcast(D, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //se envia/recibe las partes de A
    MPI_Scatter(A, N*total, MPI_DOUBLE,A, N*total, MPI_DOUBLE,0, MPI_COMM_WORLD);
    //se envia/recibe las partes de L
    MPI_Scatter(L, N*total, MPI_DOUBLE,L, N*total, MPI_DOUBLE,0, MPI_COMM_WORLD);
    //se envia/recibe las partes de U
    MPI_Scatter(U, N*total, MPI_DOUBLE,U, N*total, MPI_DOUBLE,0, MPI_COMM_WORLD);

    time_comunicacion = dwalltime() - time_comunicacion;
    time_computo = dwalltime();
   //una vez enviados todos los trozos de matrices, comienza a trabajar con su porcion
    if(miID == 0)
    {
      calcular_avg(&u,&l);
    }
    /***----***/
     for(i=0;i<total;i++)
     {
      for(j=0;j<N;j++)
      {
       for(k=0;k<N;k++)
       {
         C[i*N+j] =  C[i*N+j] + (A[i*N+k] * B[k+j*N]);
       }
      }
     }
     time_computo = dwalltime() - time_computo; //tiempo de computo
     time_gatter = dwalltime();
     MPI_Gather(C, N*total, MPI_DOUBLE ,C, N*total, MPI_DOUBLE,0, MPI_COMM_WORLD);
     time_gatter = dwalltime() - time_gatter;
     time_comunicacion = time_comunicacion + time_gatter;
     if(miID == 0)
     {
       time_total = dwalltime() - time_total; //tiempo total
     }
     printf("| %d\t| %f\t| %f|\n",miID,time_comunicacion,time_computo);
     //el proceso 0 muestra el tiempo total y la matriz resultante
     if(miID == 0)
     {
       printf("tiempo total con %d procesos: %f seg\n",NProcs,time_total);
       printf("matriz C de (%d:)\n",miID );
       print_m(C,N,miID);
     }
    /*printf("----procesos---\n" );
    printf("matriz A\n" );
    print_m(A,N,miID);
    printf("matriz B\n" );
    print_m(B,N,miID);
    */
        //printf("C[%d,%d] = %0.f + (%0.f * %0.f)  \n", i,j,C[i*N+j], A[i*N+k] , B[k+j*N]);
      //printf("C[%d,%d]: %0.f\n",i,j,C[i*N+j]);
   MPI_Finalize();
   return 0;
}

void calcular_avg(double *u, double *l, int N)
{
  int i,j;
  double total_u, total_l;
  for (i = 0; i < N; i++)
  {
    for(j=0;j<N;j++)
    {

    }
  }
}

void print_m(double *M, int dim,int id)
{
  int i,j;
  for(i=0;i<dim;i++)
  {
   for(j=0;j<dim;j++)
   {
     printf("(%d:)|%.0f\t",id,M[i*dim+j]);
   }
   printf("|\n");
  }
}
