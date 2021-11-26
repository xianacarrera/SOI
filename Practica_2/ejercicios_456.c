#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 2 - Ejercicios 4, 5 y 6
 *
 * Ejercicio 4 -> En este fragmento del programa, el proceso principal crea
 * dos hijos. El primero se vuelve zombie rápidamente, y su valor de exit es
 * recogido por el padre. A continuación, el padre acaba y el segundo hijo
 * queda huérfano.
 *
 * Ejercicio 5 -> Al final de la ejecución del segundo hijo, se cambia su
 * imagen mediante la ejecución de una de las funciones del grupo exec. Hay
 * un printf que nunca se llega a ejecutar.
 *
 * Ejercicio 6 -> Comprobar con ps lo que sucede con un proceso zombie cuando
 * el padre acaba la ejecución sin ejecutar el wait.
 *
 * Para facilitar la legibilidad del código, se ha preferido estructurarlo
 * lo más secuencialmente posible, a costa de pérdidas pequeñas de eficiencia.
 */

/*
* Notas:
*
* Ejercicio 5 -> Familia de execs:
*       - ¿l o v?
*               l-> lista de argumentos
*               v -> vector de argumentos
*       - e -> Permite especificar el entorno. Sin e, el entorno se toma de la
*              variable environ en el proceso que ejecuta la función.
*       - p -> Utilizan la variable PATH para situarse y buscar el código si
*              el filename indicado no lleva '/' (si sí lo lleva, se toma
*              directamente como la dirección del propio archivo).
*
* Ejercicio 6 -> Aunque el padre no recoja el código de exit con wait, cuando
* este finaliza, el hijo es eliminado de la tabla de procesos, puesto que ya
* nadie podrá recoger su código.
*/


int main(){
    int par1, par2;          // Resultados de los fork
    int status;              // Variable estado para el waitpid
    // Argumentos para execv (array de strings terminado en NULL, siendo
    // el primer elemento la dirección del programa que sustituirá al anterior)
    char *args[] = {"./codigo_secundario_ej5", NULL};

    par1 = fork();
    // El padre genera otro hijo
    if (par1) par2 = fork();

    sleep(5);

    // Actualmente hay 3 procesos vivos. Podemos comprobarlo con ps.

    if (!par1){
        printf("Soy el hijo 1. Tengo PID %d. "
            "Mi padre es %d. Finalizando...\n", getpid(), getppid());
        exit(10);      // El primer hijo termina
    }

    // Si comprobamos ps en este punto, el hijo 1 es zombi (<defunct>)

    if (par2){           // Padre
        sleep(5);        // Damos tiempo a que el primer hijo se vuelva zombi

        // wait(&status);   // Esperamos a que cualquier hijo acabe
        // Análogo a waitpid(-1, &status, 0);
        // Podemos acceder al parámetro de exit del hijo a través de status

        // Otra opción es waitpid, que permite esperar a un hijo específico
        if (waitpid(par1, &status, 0) < 0){
            perror("Error en watipid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
            // WEXITSTATUS(status) solo tiene sentido si WIFEXITED(status) != 0
            printf("El hijo finalizó con exit(%d)\n", WEXITSTATUS(status));
        else{
            fprintf(stderr, "Error en la finalización del hijo\n");
            exit(EXIT_FAILURE);
        }

        printf("Soy el padre. Tengo PID %d. Finalizando...\n", getpid());
        // Solo el segundo hijo está vivo, si no se ha introducido input
        // Tanto el padre como el 1º hijo se eliminan de la tabla de procesos

    } else {         // Solo puede ser el segundo hijo
        getchar();   // Pensado para esperar hasta que su padre acabe

        printf("\nSoy el hijo 2. Tengo PID %d. "
                "Mi padre es %d\n", getpid(), getppid());
        // En este punto, será un huérfano. El PID que se imprime no coincide
        // con el del padre. Se puede ver que ha sido adoptado por systemd


        if (execv(args[0], args) < 0){   // Cambiamos la imagen del código
            perror("Error en la llamada a execv");
            exit(EXIT_FAILURE);
        }
        // args[0] indica la ruta del nuevo código a ejecutar
        printf("Soy un printf que nunca se llega a ejecutar\n");
    }

    exit(EXIT_SUCCESS);
}
