#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    getchar();
    char llamada[50];

    snprintf(llamada, 50, "cat /proc/%d%s", getpid(), "/maps");

    printf("\n\n");
    printf("Soy el programa hijo. He cambiado de imagen\n\n");
    printf("Mi mapa de memoria es:\n");
    system(llamada);

    printf("\n\n");
    printf("Introducir entrada para terminar la ejecución\n");
    getchar();

    // Terminamos el proceso padre, que quedó en un bucle infinito
    if (kill(atoi(argv[1]), SIGKILL) < 0){
        perror("Error al terminar el proceso padre");
        exit(EXIT_FAILURE);
    }

    printf("Cerrando...\n");
    exit(EXIT_SUCCESS);
}
