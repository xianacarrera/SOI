Xiana Carrera Alonso
Sistemas Operativos I - Curso 2021/2022
Práctica 4 - Ejercicio 5
                                       
                          Número de iteraciones
                                        
Para comprobar los resultados mostrando el número de iteraciones 
de cada hilo es necesario destapar las líneas 46, 62, 147, 148,
154, 235 y 250.

                                 Makefile
                                 
El makefile incluido permite compilar el entregable de forma 
automática con la regla por defecto ("make"). Se utilizan las
opciones -lpthread y -lm. 
Asimismo, se puede limpiar el archivo .o mediante
"make clean" y limpiar tanto el archivo .o como el ejecutable 
con "make cleanall".         


                               Apartado f)
                               
El cálculo en paralelo supone una mejora en cuanto a eficiencia,
ya que se reparte el trabajo entre los hilos creados, reduciendo
el tiempo total. 

El ahorro depende del número de hilos utilizado.

Fijar N_HILOS = 1 no tiene sentido, pues implicaría que ambas
fórmulas de realizar la aproximación se realizarían secuencialmente.
De hecho, el hilo principal es algo más rápido, pues para el
creado habría que llamar a una función más y su código requiere 
más comprobaciones.            

Con N_HILOS = 2, vemos que el cálculo en paralelo es unos 
0.22 segundos más rápidos (los resultados varían entre 
0.2 y 0.25 segundos).

Dado que mi ordenador tiene 2 CPUs, este sería el máximo número
de hilos que podría fijar según las condiciones del enunciado.
No obstante, dado que soporta 4 hilos con hyperthreading, 
también hice algunas ejecuciones con este valor, cambiando el
código momentáneamente para que lo permitiera. Obtuve una
diferencia aún mayor, de unos 0.35 segundos. Este resultado 
tiene sentido, pues la arquitectura está optimizada para 4 hilos.

En cambio, si aumentamos exageradamente N_HILOS, vemos pequeñas 
caídas en la eficiencia, dado que cada CPU debe gestionar
la ejecución cambiando entre ellos. Por ejemplo, con N_HILOS = 50,
la diferencia disminuye hasta los 0.34 segundos, aproximadamente.

Se comprobó también la carga que supone la ejecución del programa 
para cada core utilizando el monitor del sistema. Nótese que 
aparecen representadas 4 CPUs por los 4 hilos soportados, pero en
la realidad, únicamente hay 2. Se incluye una captura de los 
resultados observados en 4 ejecuciones (véanse los picos).

Se puede ver que hay siempre 2 cores (2 hilos) trabajando.
El pico más agudo corresponde al hilo principal, que debe realizar 
el cálculo secuencialmente. Se puede notar otro pico, que estará
trabajando en el cálculo en paralelo y, en ocasiones, un tercero,
en caso de que la carga se distribuya entre varios hilos. Aun así,
no tiene por qué involucrarse un tercer hilo, pues habrá puntos
donde el hilo principal quede bloqueado, al esperar por el resto,
de forma que esa CPU puede ocuparse también de las operaciones en
paralelo.




                                
                                 
                                       
