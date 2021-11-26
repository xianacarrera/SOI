#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Se puede usar signal???

// Se puede imprimir el mapa del proceso sin ir a consola
// system("cat/proc/%d/maps", getpid());

// USAR PMAP -A para COMPROBAR DÓNDE ESTÁN LAS VARIABLES

void run_hijo();

int main(){
    pid_t div;
    char llamada[50];

    printf("Soy el padre. Mi PID es %d\n\n", getpid());

    snprintf(llamada, 50, "cat /proc/%d%s", getpid(), "/maps");
    printf("Mi mapa de memoria es:\n");
    system(llamada);

    if ((div = fork()) < 0){
        fprintf(stderr, "Error en la creación de un proceso hijo\n");
        exit(EXIT_FAILURE);
    } else if (!div)
        run_hijo();

    //printf("Soy el padre. Entrando en bucle infinito\n");
    while(1);
}

void run_hijo(){
    //int *p = (int *) malloc(sizeof(int));
    char pid_p[20];
    char *args_exec[3] = {"./hijo4"};
    char llamada[50];
    double *p;

    if ((p = (double *) malloc(1000*sizeof(double))) == NULL){
        printf("Error reservando memoria. Cerrando hijo...\n");
        exit(EXIT_FAILURE);
    }

    snprintf(pid_p, 20, "%d", getppid());
    args_exec[1] = pid_p;
    args_exec[2] = NULL;

    snprintf(llamada, 50, "cat /proc/%d%s", getpid(), "/maps");

    printf("\n\n");
    printf("Soy el hijo. Mi PID es %d\n", getpid());
    printf("He reservado memoria. Dirección apuntada: %p\n\n", p);
    printf("Mi mapa de memoria es:\n");
    system(llamada);

    printf("\n\n");
    printf("Introducir carácter para cambiar de imagen\n");
    getchar();

    execv(args_exec[0], args_exec);
}
