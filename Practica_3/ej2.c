#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021-2022
 * Práctica 3 - Ejercicio 2
 *
 * Este programa solicita un dato al usuario con el objetivo de quedarse
 * esperando indefinidamente a que la introducción del comando kill por la
 * consola.
 *
 * Sintaxis de kill: kill [options] <pid>
 * Ejemplo: kill -s SIGKILL <pid>, kill -SIGKILL <pid>
 */

int main(){
    char c;

    // Imprimimos el PID del proceso para facilitar la ejecución de kill
    printf("Mi PID es: %d\n", getpid());
    printf("Introduce el comando kill desde consola.\n");
    scanf("%c", &c);
    printf("No me ejecuto porque mi proceso ha muerto\n");

    exit(EXIT_SUCCESS);
}
