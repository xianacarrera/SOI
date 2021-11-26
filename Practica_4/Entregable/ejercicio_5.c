#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>


/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 4 - Ejercicio 5
 *
 * Este programa calcula aproxima pi tanto de forma paralela, a través
 * del trabajo combinado de varios hilos, como de forma secuencial, en un único
 * hilo, el principal. Se imprimen y comparan ambos resultados, así como
 * el tiempo requerido para cada uno de ellos.
 *
 * Los hilos se turnan las iteraciones. Cada uno de ellos trabaja en bloques
 * de 100, desde 0 hasta 1 000 000, y las iteraciones se reparten en módulo
 * el número de hilos * 100: un hilo k se encarga del rango
 * [k*100, k*100+99], [(N_HILOS + k) * 100, (N_HILOS + k) * 100 + 99],
 * [(2*N_HILOS + k) * 100, (2*N_HILOS + k) * 100 + 99], etc., parando si
 * llega a un número igual o superior a 1 000 000.
 *
 * El número de hilos debe ser igual o superior a 1 e igual o inferior al
 * número de cores del equipo.
 *
 * Las comprobaciones correspondientes al número de iteraciones se han tapado.
 * Las líneas afectadas empiezan por ---
 */


// Mi procesador es un Intel Core i7-7500U, de 2 cores (4 hilos)
#define N_HILOS 2           // Número de hilos paralelos a crear
#define N_CORES 2           // Máximo número de hilos
#define MAX_K 1000000       // Número de iteraciones total
#define ITER_POR_HILO 100   // Longitud de los bloques de iteraciones por hilo

// Función en la que trabajan los hilos creados
void * ejecutar_hilo(void * p_num_hilo);
// Función que calcula una iteración de la fórmula de Bailey-Borwein-Plouffe
long double calcular_iteracion_pi(int k);

long double pi_parcial[N_HILOS];     // Resultados obtenidos por cada hilo
//--- int n_iters[N_HILOS];     // Número de iteraciones realizadas por hilo

/*
 * Para medir el tiempo, utilizaremos gettimeofday() en lugar de clock() porque
 * este último cuenta el tiempo utilizado de CPU, de forma que se sumarían las
 * contribuciones de los distintos hilos.
 */

int main(){
    pthread_t hilos[N_HILOS];       // Identificadores de los hilos creados
    int claves[N_HILOS];       // Clave de acceso de cada hilo a sus variables
    long double pi_paralelo = 0;      // Aproximación de pi en paralelo
    long double pi_secuencial = 0;    // Aproximación de pi secuencial
    struct timeval t0_par, tf_par, t_par;  // Medición del tiempo en paralelo
    struct timeval t0_sec, tf_sec, t_sec;  // Medición del tiempo secuencial
    struct timeval t_dif;                  // Diferencia entre los tiempos
    //--- int tot_iter = 0;      // Número de iteraciones total (comprobación)
    char * exit_hilo;            // Valor de retorno de los hilos
    int error;      // Gestión de errores de pthread_create() y pthread_join()
    int i;                              // Variable de iteración

    // Comprobamos que el número de hilos fijado sea correcto en relación
    // al número de cores
    if (N_HILOS > N_CORES || N_HILOS == 0){
        printf("El número de hilos elegido (%d) debe estar en el rango "
                "[1, %d]. Abortando...\n", N_HILOS, N_CORES);
        exit(EXIT_FAILURE);
    }
    /*
     * No es necesario comprobar el caso N_HILOS < 0, ya que tendría
     * lugar un error de compilación al usar N_HILOS como tamaño de arrays.
     * Esto también obliga a que N_HILOS sea un entero.
     */

    // Capturamos el instante de inicio del cálculo paralelo
    // El segundo argumento de gettimeofday() está obsoleto. Lo dejamos a NULL
    gettimeofday(&t0_par, NULL);

    // Creamos los N_HILOS hilos que trabajarán en paralelo
    for (i = 0; i < N_HILOS; i++){
        /*
         * A cada hilo le pasaremos una clave de acceso a sus variables, que
         * será el valor de i con el que se haya creado. Tenemos que almacenar
         * ese valor en un array para que no se modifique entre iteraciones.
         */
        claves[i] = i;

        /*
         * Guardamos el identificador de los hilos en hilos[] para poder
         * llamar a pthread_join con ellos después. Nos interesa posponer
         * la llamada en lugar de ejecutarla dentro de este bucle para
         * asegurar que los hilos trabajarán a la vez.
         *
         * El segundo argumento es NULL, ya que no queremos alterar la
         * configuración por defecto de los hilos (capacidad para poder ser
         * aguardados, alcance, planificador, tamaño del stack, etc.).
         *
         * Como tercer argumento pasamos la dirección de la instrucción desde
         * la que empieza a ejecutarse el hilo creado, esto es, su función.
         *
         * El cuarto elemento es el (único) argumento que recibe la función
         * del hilo: la dirección donde está su clave.
         */
        if ((error = pthread_create(&hilos[i], NULL, ejecutar_hilo,
                    (void *) &claves[i])) != 0){
            fprintf(stderr, "Error %d en pthread_create() para el hilo %d: "
                    "%s\n", error, i, strerror(error));
            // Obtenemos la representación textual del error con strerror()
            exit(EXIT_FAILURE);
        }
    }

    // Con pthread_join() dejamos bloqueado el hilo principal para que no
    // finalice antes de que terminen todos los hilos creados
    for (i = 0; i < N_HILOS; i++){
        // Para comprobar que la finalización de los hilos es correcta,
        // guardamos el valor de su pthread_exit() en exit_hilo
        if ((error = pthread_join(hilos[i], (void **) &exit_hilo)) != 0){
            fprintf(stderr, "Error %d en pthread_join() para el hilo %d: "
                    "%s\n", error, i, strerror(error));
            exit(EXIT_FAILURE);
        }
        // La ejecución del hilo principal no sigue hasta que termina hilos[i]
    }
    // Capturamos el tiempo en el que ha terminado el cálculo en paralelo
    gettimeofday(&tf_par, NULL);


    // Para que la medición del tiempo sea lo más exacta posible, comprobamos,
    // imprimimos y actualizamos los resultados en un bucle separado
    for (i = 0; i < N_HILOS; i++){
        // Ahora que acabaron todos los hilos, sumamos sus resultados  
        pi_paralelo += pi_parcial[i];  
    
        // Comprobamos que el valor de retorno sea el mensaje indicado
        if (strcmp(exit_hilo, "finalizado correctamente")){
            fprintf(stderr, "El hilo %d finalizó de forma incorrecta.\n", i);
            exit(EXIT_FAILURE);
        }
        // strcmp() devuelve 0 si las cadenas son iguales

        printf("Hilo %d %s\n", i, exit_hilo);
        //--- printf("\tIteraciones realizadas: %d\n", n_iters[i]);
        //--- tot_iter += n_iters[i];     // Sumamos al total de iteraciones
        printf("\tCálculo parcial de pi: %.60Lf\n\n", pi_parcial[i]);
    }


    printf("****** Resumen de resultados:\n");
    //--- printf("\tNúmero de iteraciones totales: %d\n", tot_iter);
    printf("\tPi calculado en paralelo: %.60Lf\n", pi_paralelo);
    // Calculamos la diferencia entre los tiempos (tf - t0) con timersub, que
    // opera sobre timevals
    timersub(&tf_par, &t0_par, &t_par);
    printf("\tTiempo usado en paralelo: %ld seg, %6ld microseg\n\n",
            t_par.tv_sec, t_par.tv_usec);
    // timeval tiene dos campos: uno de segundos y otro de microsegundos
    // Ambos equivalen a un long int


    // Guardamos el inicio del cálculo secuencial
    gettimeofday(&t0_sec, NULL);
    // Calculamos pi a través de un solo hilo
    for (i = 0; i < MAX_K; i++)
        pi_secuencial += calcular_iteracion_pi(i);
    gettimeofday(&tf_sec, NULL);    // Fin del cálculo secuencial

    printf("****** Resultados globales:\n");
    printf("\tPi calculado secuencialmente por el hilo principal: %.60Lf\n",
            pi_secuencial);
    timersub(&tf_sec, &t0_sec, &t_sec);    // Diferencia entre fin e inicio
    printf("\tTiempo usado secuencialmente: %ld seg, %6ld microseg\n\n",
            t_sec.tv_sec, t_sec.tv_usec);

    printf("****** Comparación\n");
    printf("\tPi paralelo - pi secuencial = %.60Lf\n",
            pi_paralelo - pi_secuencial);
    // Calculamos la diferencia entre los tiempos paralelos y secuenciales
    timersub(&t_sec, &t_par, &t_dif);
    printf("\tTiempo paralelo - tiempo secuencial = %ld seg, %6ld microseg"
            "\n\n", t_dif.tv_sec, t_dif.tv_usec);
    // Si el tiempo paralelo fuese mayor que el secuencial (muy improbable),
    // la diferencia de tiempos sería negativa

    printf("Cerrando programa...\n\n");
    exit(EXIT_SUCCESS);
}


/*
 * Función que ejecuta cada uno de los hilos creados por el hilo principal.
 * Calcula la contribución de pi correspondiente en función del número
 * total de hilos, y guarda el resultado en pi_parcial.
 * También cuenta el número de iteraciones realizadas por cada hilo, que
 * debería ser MAX_K / N_HILOS, y almacena el valor obtenido en n_iters.
 *
 * Solamente puede tener un argumento, un puntero a void. Se ha optado por
 * pasar desde el main() la dirección de una clave para cada hilo. Otra
 * opción sería pasar directamente el valor como si fuese una dirección:
 * (void *) i en el main, y usarlo directamente como (int) p_num_hilo en
 * ejecutar_hilo(), pero este cast no es seguro y saltarían warnings.
 */
void * ejecutar_hilo(void * p_num_hilo){
    int k[N_HILOS];      // Contadores de la iteración actual de cada hilo
    int temp[N_HILOS];   // Variables temporales para contar 100 iteraciones

    /*
     * Los hilos comparten variables locales, pero el argumento que recibe
     * ejecutar_hilo para cada uno será diferente. Utilizaremos este valor
     * como una clave que les permite acceder a sus variables propias.
     *
     * Así, trabajaremos con arrays donde el elemento de cada índice
     * es "propiedad" del hilo que tiene por clave ese índice.
     *
     * No podemos guardar el valor como una variable local, porque sería
     * sobreescrita por cada hilo. Tenemos que repetir *((int *) p_num_hilo)
     * cuando nos hace falta recurrir a la clave.
     */

    /*
     * Inicializamos el array k con la primera iteración que hará cada hilo,
     * i.e., su desplazamiento.
     * El hilo 0 empezará en 0; el 1, en 100; el 2, en 200; el 3, en 300, etc.
     * k aumentará de 1 en 1, saltando los bloques que cada hilo no tenga que
     * calcular.
     */
    k[*((int *) p_num_hilo)] = *((int *) p_num_hilo) * ITER_POR_HILO;
    // Inicializamos el sumatorio parcial de pi del hilo
    pi_parcial[*((int *) p_num_hilo)] = 0;
    // Inicializamos el contador de iteraciones del hilo
    //--- n_iters[*((int *) p_num_hilo)] = 0;

    // Las iteraciones ocurren en el rango [0, MAX_K = 1000000)
    while (k[*((int *) p_num_hilo)] < MAX_K){
        // Guardamos la cota superior de este bloque de 100 iteraciones
        temp[*((int *) p_num_hilo)] = k[*((int *) p_num_hilo)] + 100;

        // Realizamos bloques de ITER_POR_HILO = 100 iteraciones
        // Se mantiene un contador por hilo como un elemento del array i
        for (; k[*((int *) p_num_hilo)] < temp[*((int *) p_num_hilo)];
                k[*((int *) p_num_hilo)]++){
            // Calculamos 100 iteraciones con la fórmula de pi
            pi_parcial[*((int *) p_num_hilo)] +=
                    calcular_iteracion_pi(k[*((int *) p_num_hilo)]);
            // Actualizamos el contador de iteraciones realizadas
            //--- n_iters[*((int *) p_num_hilo)]++;
        }

        // Saltamos todos los k's correspondientes al resto de hilos
        // Si, por ejemplo, hay 5 hilos, el hilo 2 pasará de k = 300
        // a k = 700; de k = 800 a k = 1200, etc.
        k[*((int *) p_num_hilo)] += (N_HILOS - 1) * ITER_POR_HILO;
    }

    // Señalizamos que el hilo termina correctamente
    // Para no tener que reservar memoria directamente, pasamos una cadena
    pthread_exit((void *) "finalizado correctamente");
}


/*
 * Función que calcula una iteración de la fórmula de Bailey-Borwein-Plouffe.
 * Recibe como argumento el número de la iteración, k.
 * Devuelve un long double con el resultado de la iteración.
 */
long double calcular_iteracion_pi(int k){
    // Dado que el denominador 8k + 1 se repite, lo almacenamos como variable
    long double den1;

    den1 = 8 * k + 1;
    // Usamos que 16^k = (2^4)^k = 2^(4*k)
    // Como trabajamos con long double, usamos powl en lugar de pow
    return (4/den1 - 2/(den1 + 3) - 1/(den1 + 4) - 1/(den1 + 5))/
            powl((long double) 2, (long double) 4*k);
}
