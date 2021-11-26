#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


#define N 4


char letra = 'a';
int * secuencia;        // Puntero a impar

void * ejecutar_hilo(void * numero){
    printf("Letra (global): %c\n", letra++);

    // Puede que el valor de impar imprimido no siga la secuencia 1->2->3->...
    // Dado que los hilos están trabajando a la vez, es probable que todos
    // cambien el valor al mismo tiempo, y se encuentren que, al leerlo de
    // nuevo, todos lo han modificado.

    // Primero se lee, justo después se cambia para el siguiente
    printf("Valor de impar(local): %d\n", (*secuencia)++);

    // Número no cambia entre hilos, ya que es el argumento de cada uno
    printf("Número (local): %d\n", ++numero);

    pthread_exit((void *) "Hilo finalizado");
}

int main(){
    pthread_t hilos[N];         // Identificadores de los hilos
    int numero = 0;             // Argumento de cada hilo
    int impar = 0;              // Variable local compartida por los hilos
    int i;                      // Contador
    int error;                  // Código de error de cada hilo
    char * finalizacion;        // Mensaje de salida de cada hilo

    secuencia = &impar;

    for (i = 0; i < N; i++){
        if (error = pthread_create(
                    &hilos[i], NULL, ejecutar_hilo, (void *) numero)){
            fprintf(stderr, "Error %d en pthread_create: %s\n", error,
                    strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    // Realizamos los pthread_join fuera del primer bucle para comprobar
    // que todos los hilos pueden coexistir
    for (i = 0; i < N; i++){
        if (error = pthread_join(hilos[i], (void *) &finalizacion)){
            fprintf(stderr, "Error %d en pthread_join: %s\n", error,
                    strerror(error));
            exit(EXIT_FAILURE);
        } else
            // Imprimimos el mensaje de salida de cada hilo
            printf("%s\n", finalizacion);

    }

    exit(EXIT_SUCCESS);
}
