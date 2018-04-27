
# Programación con memoria compartida
La entrega es en grupos de a los sumo dos personas.
Fecha límite 12 de mayo.
Se deben entregar los archivos .c con el código fuente de cada ejercicio y un
informe en pdf que describa brevemente las soluciones planteadas y conclusiones.
En todos los ejercicios se debe implementar un algoritmo secuencial optimizado.
Paralelizar y calcular **Speedup** y **Eficiencia** con respecto a 2 y 4 hilos.
El informe debe incluir las tablas con los tiempos de ejecución, el speedup y la
eficiencia para cada caso de prueba.

1. Resolver con **Pthreads** y **OpenMP** la siguiente expresión:
                                R = AA
  Donde A es una matriz de NxN. Analizar el producto AA y utilizar la estrategia
  que proporcione el mejor tiempo de ejecución.
  **Evaluar N=512, 1024 y 2048.**

2. Realizar un algoritmo **Pthreadsy** otro **OpenMP** que resuelva la expresión:̅̅̅
        M = ¬(u.l).AAC + ¬b.LBE + ¬b.DUF
  Donde A, B, C, D, E y F son matrices de NxN. L y U son matrices triangulares de
  NxN inferior y superior, respectivamente. ̅¬b es el promedio de los valores de
  los elementos de la matriz B y ̅¬̅(u.l)̅ es el producto de los promedios de los
  valores de los elementos de las matrices U y L, respectivamente.
  **Evaluar N=512, 1024 y 2048.**
3. Paralelizar con **OpenMP** un algoritmo que cuente la cantidad de número pares
  en un vector de N elementos. Al finalizar, el total debe quedar en una
  variable llamada pares.
  Evaluar con valores de N donde el algoritmo paralelo represente una mejora
  respecto al algoritmo secuencial.
