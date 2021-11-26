#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define N 3                    // Número de hilos
#define LIM_SUP ULONG_MAX     // Límite superior para Fibonnaci

// Colores para distinguir los hilos por consola
#define ROJO "\e[1;31m"
#define VERDE "\e[1;32m"
#define CYAN "\e[1;36m"
#define RESET "\e[0m"          // Color de errores (predeterminado)

// Podemos hacer pthread_yield en los hilos computacionalmente costosos, que
// agotarían el quantum.

// Cálculo de límite -> Cuando n tiende a infinito, Fibonnaci se aproxima a
// 1/sqrt(5)*phi^n, donde phi =continue;       // No se hace nada (1+sqrt(5)/2)

int num_hilo = 1;       // Número de hilo actual

unsigned long long fibonacci_aprox(long n){
    // Aproximación de la n-ésima iteración de Fibonnaci basada en la fórmula
    // de Binet
    return pow(1+sqrt(5)/2, n)/sqrt(5);
}

// Función que calcula la sucesión de Fibonnaci cediendo la CPU habitualmente
void * fibonacci_ligero(void * arg){
    unsigned long long f_anterior = 0;
    unsigned long long f_actual = 1;
    unsigned long long temp;
    unsigned long long limite = fibonacci_aprox(1000);  // 1000 iteraciones
    long iteracion = 0;
    int error;

    // Calculamos hasta que nuestros valores dejen de caber en un
    // unsigned long long
    while (f_actual < LIM_SUP){
        // Sustituimos los valores de Fibonnaci al cambiar de iteración
        temp = f_anterior;
        f_anterior = f_actual;
        f_actual += temp;

        if (f_actual > limite){
            sleep(1);
            // Cada 1000 iteraciones, imprimimos el valor y cedemos la CPU
            printf("%sValor actual de fibonacci_ligero: %llu%s\n",
                    ROJO, f_actual, RESET);
            printf("%sSe cede la gestión de la CPU%s\n", ROJO, RESET);
            if ((error = pthread_yield()) != 0){
                fprintf(stderr, "Error %d en pthread_yield: %s\n",
                        error, strerror(error));
                exit(EXIT_FAILURE);
            }

            limite = fibonacci_aprox(iteracion + 1000);
        }
        iteracion++;
    }

    pthread_exit(NULL);    // No pasamos valor de vuelta
}

void * fibonacci_costoso(void * arg){
    unsigned long long f_anterior = 0;
    unsigned long long f_actual = 1;
    unsigned long long temp;
    unsigned long long limite = fibonacci_aprox(1000);
    long iteracion = 0;

    // Calculamos hasta que nuestros valores dejen de caber en un
    // unsigned long long
    while (f_actual < LIM_SUP){
        sleep(1);
        // Sustituimos los valores de Fibonnaci al cambiar de iteración
        temp = f_anterior;
        f_anterior = f_actual;
        f_actual += temp;

        // Cada 1000 iteraciones, imprimimos el valor actual de la función
        if (f_actual > limite){
            printf("%sValor actual de fibonacci_costoso: %llu\n%s",
                    VERDE, f_actual, RESET);
            limite = fibonacci_aprox(iteracion + 1000);
        }
        iteracion++;
    }

    pthread_exit(NULL);
}



void * preguntar(void * arg){
    int i;
    char c;

    // Si se usa, es recomendable poner a dormir al resto de hilos de vez en
    // cuando para poder ver entrada

    // Dado que esta función es fundamentalmente de E/S, pero
    // computacionalmente poco costosa, lo más eficiente será que la
    // CPU le llegue frecuentemente, pues la dejará rápidamente.
    for (i = 0; i < 10; i++){
        printf("%sSOLICITO ENTRADA%s\n", CYAN, RESET);
        scanf(" %c", &c);
    }

    pthread_exit(NULL);
}


int main(){
    pthread_t hilos[N];         // Identificadores de los hilos
    char * finalizacion;        // Mensaje de salida de los hilos
    int error;                  // Gestión de errores
    int i;                      // Contador

    for (i = 0; i < N; i++){
        switch(i){      // Mandamos cada hilo a una función distinta
            case 0:
                error = pthread_create(&hilos[i], NULL, fibonacci_ligero,
                        NULL);
                break;
            case 1:
                error = pthread_create(&hilos[i], NULL, fibonacci_costoso,
                        NULL);
                break;
            case 2:
                error = pthread_create(&hilos[i], NULL, preguntar,
                    NULL);
                break;
            default:
                continue;       // No se hace nada
        }
        if (error){
            fprintf(stderr, "Error %d en pthread_create: %s\n", error,
                    strerror(error));
            exit(EXIT_FAILURE);
        }

    }

    // Realizamos los pthread_join fuera del primer bucle para comprobar
    // que todos los hilos pueden coexistir
    for (i = 0; i < N; i++){
        if ((error = pthread_join(hilos[i], (void *) &finalizacion)) != 0){
            fprintf(stderr, "Error %d en pthread_join: %s\n", error,
                    strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
