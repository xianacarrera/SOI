#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 3 - Ejercicio 3
 *
 * Este programa permite probar el envío de señales entre procesos,
 * utilizando en concreto las señales SIGUSR1 y SIGUSR2, pensadas para que su
 * comportamiento sea definido por el programador, y SIGTERM, de finalización.
 * El proceso padre crea un hijo, que le envía una señal SIGUSR1 al primero.
 * Este le envía una señal SIGUSR2 de vuelta y, después, lo hace finalizar
 * mediante SIGTERM.
 */

/* Declaracion de la funcion de gestion de señales recibidas */
static void gestion(int);

int main(){
    int padre;       // PID del proceso padre
    int hijo;        // Resultado del fork (padre -> PID, hijo -> 0)
    int estado;      // Llamada a wait()

    // Derivamos el tratamiento de las señales SIGUSR1 y SIGUSR2 a la
    // función gestion()
    if (signal(SIGUSR1, gestion) == SIG_ERR)
        printf("Error al crear el gestor 1\n");
    if (signal(SIGUSR2, gestion) == SIG_ERR)
        printf("Error al crear el gestor 2\n");

    padre = getpid();           // Guardamos el PID del padre

    if ((hijo = fork()) == 0){        /* Trabajo del hijo */
        kill(padre, SIGUSR1);         /* Envía señal al padre */
        for (;;);        /* Espera señales del padre indefinidamente */
    }
    else {                   /* Trabajo del padre */
        // Imprimimos los PIDs para corroborar que el resultado sea correcto
        printf("PID del padre: %d\n", padre);
        printf("PID del hijo: %d\n", hijo);

        kill(hijo, SIGUSR2);        // El hijo imprime un mensaje

        // Esperamos un tiempo para que la señal de terminación no se envíe
        // antes de que el hijo reciba SIGUSR2 e imprima su mensaje
        sleep(2);
        kill(hijo, SIGTERM);        // El hijo 2 termina

        if (wait(&estado) != hijo){
            // wait devuelve el ID del hijo terminado en caso de éxito
            printf("Error en la finalización del hijo\n");
            exit(EXIT_FAILURE);
        }

        printf("El hijo ha terminado su ejecución. Padre finalizando...\n");
    }


    exit(EXIT_SUCCESS);
}

/* Función de gestión de señales */
static void gestion(int numero_de_senhal) {
    switch (numero_de_senhal) {
        case SIGUSR1:                   /*Entra señal SIGUSR1*/
            // Este mensaje será imprimido por el padre
            printf("Señal tipo 1 recibida. Soy %d\n", getpid()); break;
        case SIGUSR2:
            // Este mensaje será imprimido por el hijo
            printf("Señal tipo 2 recibida. Soy %d\n", getpid());
    }
    return;
}
