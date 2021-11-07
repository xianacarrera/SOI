#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 3 - Ejercicio 6
 *
 * En este programa, el proceso padre crea un hijo, que inmediatamente realiza
 * un pause(). 5 segundos después, el padre le envía la señal SIGUSR1, que
 * desbloquea al hijo.
 */

// Función de gestión para la señal SIGUSR1
void gestion(int numero_de_senhal);

int main(){
    int res;

    if ((res = fork()) == -1){
        perror("Error en la creación del proceso hijo");
        exit(EXIT_FAILURE);
    } else if (res == 0){
        /*
         * El hijo personaliza su gestión de SIGUSR1
         * Dado que el padre va a dormir 5 segundos, el hijo tiene tiempo
         * de sobra para ejecutar signal(). Otra opción sería que el padre
         * realizara la llamada a signal() antes del fork() y que el hijo
         * heredara la configuración.
         */
        if (signal(SIGUSR1, gestion) == SIG_ERR){
            perror("Error al crear el gestor de SIGUSR1\n");
            exit(EXIT_FAILURE);
        }

        // El hijo queda bloqueado hasta recibir una señal que lo haga
        // terminar o que se derive a una función gestora.
        pause();

        printf("Hijo desbloqueado. Hijo terminando...\n");
        exit(EXIT_SUCCESS);
    }

    // Las siguientes líneas solo son ejecutadas por el padre

    sleep(5);       // Duerme 5 segundos
    // Desbloquea al hijo al despertar
    if (kill(res, SIGUSR1) == -1){
        perror("No se pudo enviar la senhal SIGUSR1 al proceso hijo");
        exit(EXIT_FAILURE);
    }

    printf("Senhal SIGUSR1 enviada\n");
    exit(EXIT_SUCCESS);
}

// Función gestora para la senhal SIGUSR1
// Simplemente imprime un mensaje notificando de la recepción
void gestion(int numero_de_senhal){
    // No hace falta comprobar que la senhal sea SIGUSR1 porque es la única
    // que capturamos
    printf("Recibida SIGUSR1!\n");
    return;
}
