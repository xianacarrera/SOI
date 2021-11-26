#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define N 3         // Número de hilos a crear

// ps -T para ver los hilos de un proceso
// ps -T -p <PID>

int num_hilo = 1;       // Número de hilo actual

void * ejecutar_hilo(){
    if (num_hilo++ != N){       // No es el último hilo
        getchar();              // Se queda esperando
        pthread_exit(NULL);     // Finaliza sin enviar ningún mensaje
    }
    printf("Fork: %d\n", fork());       // Crea un proceso hijo
    getchar();     // También queda esperando para poder hacer comprobaciones
                   // por la terminal
    pthread_exit(NULL);
}

int main(){
    pthread_t hilos[N];         // Identificadores de los hilos
    int error;                  // Control de errores
    char * finalizacion;        // Mensaje de finalización de los hilos
    int i;                      // Contador

    // Imprimimos el PID para poder buscar más fácilmente en la consola
    printf("Mi PID es %d\n", getpid());

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
