#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>


/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 3 - Ejercicio 5
 *
 * En este programa, el proceso padre, P, crea dos hijos, H1 y H2. P espera
 * por H1, que es finalizado por H2 con kill() por indicación del usuario.
 */

// Función personalizada que envuelve a fork()
int crear_proceso();

int main(){
    int h1, h2;     // Resultados de fork() al generar los procesos hijos
    char input;     // Input del usuario
    int estado;     // Estado en la llamada a waitpid()

    // El padre crea dos procesos hijos, H1 y H2
    if (h1 = crear_proceso()) h2 = crear_proceso();

    if (!h1){            // Trabajo de H1
        // Bucle infinito - H1 quedará aquí durante el resto de su ejecución
        for (;;);
    } else if (!h2){     // Trabajo de H2
        do{
            printf("¿Debo finalizar a H1? (Introduce 's' para aceptar)\n");
            scanf(" %c", &input); // Dejamos un espacio por el retorno de carro
        } while (input != 's');

        // H2 tiene copiado el PID de H1, porque su padre lo tenía guardado
        if (kill(h1, SIGKILL) == -1){
            perror("H2 ha tenido un error al intentar finalizar a H1");
            exit(EXIT_FAILURE);
            // Tras leer el error, el usuario puede cerrar manualmente P y H2
        }

        printf("Enviada senhal de terminación desde H2 a H1\n");

        // H2 finaliza
        exit(EXIT_SUCCESS);
    }

    // Trabajo del padre
    // Espera a que H1 finalice
    if (waitpid(h1, &estado, 0) == -1){
        perror("P ha tenido un error al esperar por H1");
        exit(EXIT_FAILURE);
    }
    printf("Soy P. H1 ha finalizado\n");

    // Para saber si H2 ha finalizado correctamente, recogemos su valor de exit
    // con waitpid
    if (waitpid(h2, &estado, 0) == -1){
        perror("P ha tenido un error al esperar por H2");
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(estado) && WEXITSTATUS(estado) == EXIT_SUCCESS)
        printf("Se ha comprobado que H2 finalizó correctamente\n");
    else {
        if (WIFEXITED(estado))    // WEXITSTATUS(estado) != EXIT_SUCCESS
            fprintf(stderr, "La finalización de H2 fue errónea\n");
        else if (WIFSIGNALED(estado))
            fprintf(stderr, "H2 finalizó de forma inesperada debido a una "
                    "senhal\n");

        fprintf(stderr, "Finalizando P...\n");
        exit(EXIT_FAILURE);
    }

    printf("P también termina correctamente\n");
    exit(EXIT_SUCCESS);
}

/*
 * Función que encapsula un fork() junto al tratamiento de sus errores.
 * Si hay algún fallo, el proceso termina. En caso contrario, la función
 * devuelve el resultado del fork().
 */
int crear_proceso(){
    int res;

    if ((res = fork()) == -1){
        perror("Error - creación de un proceso hijo fallida");
        exit(EXIT_FAILURE);
    } else return res;       // Se devuelve el valor para el padre/hijo
}
