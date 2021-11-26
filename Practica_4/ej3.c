#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define N 3             // Número de hilos

int num_hilo = 0;       // Número de hilo actual

void * ejecutar_hilo(){
    // El primer hilo no entra en el if, pero el resto quedan bloqueados
    if (num_hilo++) sleep(5);
    printf("Hola\n");      // Solo se va a imprimir un hola
    exit(EXIT_SUCCESS);    // El primer hilo finaliza todo el programa
}

int main(){
    pthread_t hilos[N];         // Identificadores de los hilos
    int error;                  // Control de errores
    char * finalizacion;        // Mensaje de finalización de los hilos
    int i;                      // Contador

    for (i = 0; i < N; i++){
        if ((error = pthread_create(&hilos[i], NULL, ejecutar_hilo, NULL))
                != 0){
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
