  Programacion distribuida
 La entrega es en grupos de a los sumo dos personas. Fecha límite viernes 15 de
junio.
Se deben entregar los archivos .c con el código fuente de cada ejercicio y un
informe en pdf que describa brevemente las soluciones planteadas y conclusiones.
En todos los ejercicios se debe implementar un algoritmo secuencial optimizado.
Paralelizar y calcular *Speedup* y *Eficiencia* con respecto a 4 y 8 procesos. Realizar el
análisis de escalabilidad correspondiente y evaluar el impacto de las
comunicaciones.
Para la solución de MPI con 4 procesos utilizar sólo una máquina y dos máquinas
(2 procesos por máquina).
Calcular y analizar, en caso de existir, el desbalance de carga.
El informe debe incluir las tablas con los tiempos de ejecución, el speedup y la
eficiencia.
+ 1. Realizar un algoritmo MPI que resuelva la expresión:
	  M =¬(u.l) * (AB + LC + DU)̅̅̅̅	
Donde A, B, C y D son matrices de NxN. L y U son matrices triangulares de NxN
inferior y superior, respectivamente. ̅ y ̅ son los promedios de los valores de
los elementos de la matrices U y L, respectivamente.

Evaluar N=512, 1024 y 2048.

+ 2. Para el ejercicio anterior realizar un algoritmo híbrido MPI-OpenMP y
comparar con los resultados obtenidos anteriormente.

