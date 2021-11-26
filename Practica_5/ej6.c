#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Misma variable local para los dos hilos?
    // En el enunciado habla de variableS localES en el hilo principal???

int global;

void * ejecutar_h1(void * arg){
    int local_hilo;

    local_hilo = *((int *) arg) * global;
    printf("H1 - Dirección de la variable global: %p\n", &global);
    printf("H1 - Dirección del parámetro: %p\n", &arg);
    printf("H1 - Dirección de la variable local de HP: %p\n", arg);  //???????????????
    printf("H1 - Dirección de la variable local de H1: %p\n", &local_hilo);

    pthread_exit(NULL);
}

int main(){
    int local;
    pthread_t hilo1, hilo2;

    printf("PID del hilo principal: %d\n", getpid());

    printf("Dirección de la variable global: %p\n", &global);
    printf("Dirección de la variable local: %p\n", &local);

    if ((pthread_create(&hilo1, NULL, ejecutar_h1, (void *) &local)) != 0){
        fprintf(stderr, "Error en la creación del primer hilo\n");
        exit(EXIT_FAILURE);
    }

    while(1);
/*
    if ((pthread_create(&hilo2, NULL, ejecutar_hilo, (void *) &local)) != 0){
        fprintf(stderr, "Error en la creación del segundo hilo\n");
        exit(EXIT_FAILURE);
    }*/
}
