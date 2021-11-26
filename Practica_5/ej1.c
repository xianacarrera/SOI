#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int global;
int global_inic = 1;
int array_global[10];
int array_global_inic[] = {2, 3};

int main(){
    int local;
    int local_inic = 5;
    int array_local[10];
    int array_local_inic[] = {6, 7, 8};

    printf("Mi PID es %d\n", getpid());

    printf("Dirección de global: %p\n", &global);
    printf("Dirección de global_inic: %p\n", &global_inic);
    printf("Dirección de local: %p\n", &local);
    printf("Dirección de local_inic: %p\n", &local_inic);
    printf("\n");

    printf("Dirección de array_global[]: %p\n", array_global);
    printf("Direcciń de array_global_inic[]: %p\n", array_global_inic);
    printf("Dirección de array_local[]: %p\n", array_local);
    printf("Dirección de array_local_inic[]: %p\n", array_local_inic);
    printf("\n");

    printf("Dirección de main: %p\n", main);
    printf("\n");

    printf("Esperando entrada para cerrar el programa\n");
    getchar();

    printf("Cerrando programa...\n");
    exit(EXIT_SUCCESS);
}
