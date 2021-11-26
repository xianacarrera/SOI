#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Probar a ejecutar liberando p y sin liberar q para ver dónde va q

int main(){
    int *p;
    int *q;

    if ((p = (int *) malloc(5*sizeof(int))) == NULL){
        fprintf(stderr, "Error reservando memoria. Abortando...\n");
        exit(EXIT_FAILURE);
    }

    printf("Mi PID es %d\n", getpid());

    printf("Dirección de p: %p\n", &p);
    printf("Dirección a la que apunta p: %p\n", p);

    printf("Introducir carácter para proceder a liberar memoria\n");
    getchar();

    free(p);
    printf("Memoria de p liberada\n");
    printf("\n");

    printf("Introducir carácter para proceder a reservar memoria\n");
    getchar();

    if ((q = (int *) malloc(4*sizeof(int))) == NULL){
        fprintf(stderr, "Error reservando memoria. Abortando...\n");
        exit(EXIT_FAILURE);
    }

    printf("Dirección de q: %p\n", &q);
    printf("Dirección a la que apunta q: %p\n", q);

    printf("Esperando entrada para cerrar el programa\n");
    getchar();

    free(q);

    printf("Cerrando programa...\n");
    exit(EXIT_SUCCESS);
}
