#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>  // Para la función sleep

int main(){
    int i, j, N = 1000000000;
    volatile int *p = NULL;

    for (i = 0; i < N; i++){
        sleep(1);    // Espera durante 1 segundo
        p = (int *) malloc(100*N*sizeof(int));

        // Para que realmente los datos pasen a la RAM, debemos usarlos.
        // Si no lo hacemos, malloc no se realiza directamente sobre el
        // heap y no vemos gasto de la RAM.
        for (j = 0; j < N; j++){
            p[j] = 0;
            printf("%d\n", p[j]);
        }
    }

    return(EXIT_SUCCESS);
}
