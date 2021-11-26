#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// https://stackoverflow.com/questions/24117002/same-address-for-different-variables-of-different-functions-in-c/24117065

void f1(int n){
    int localf1;

    printf("Soy f1\n");
    printf("\tDirección de mi parámetro: %p\n", &n);
    printf("\tDirección de mi variable local: %p\n", &localf1);
}

void f2(int m){
    int localf2;

    printf("Soy f2\n");
    printf("\tDirección de mi parámetro: %p\n", &m);
    printf("\tDirección de mi variable local: %p\n", &localf2);
}

int main(){
    printf("Mi PID es %d\n", getpid());

    printf("Dirección de f1: %p\n", f1);
    printf("Dirección de f2: %p\n", f2);

    f1(3);
    f2(5);


    printf("Esperando entrada para cerrar el programa\n");
    getchar();

    printf("Cerrando programa...\n");
    exit(EXIT_SUCCESS);
}
