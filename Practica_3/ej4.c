#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 3 - Ejercicio 4 
 *
 * Este programa desactiva el funcionamiento de la señal de interrupción
 * SIGINT (lanzada, por ejemplo, cuando el usuario corta la ejecución con
 * CtrlC). Se cambia también la señal SIGHUP (que normalmente termina los
 * procesos) para que reactive el comportamiento habitual de SIGINT. SIGHUP
 * será enviada por el usuario con el comando kill, desde la termianl.
 */


/*
 * Comentarios sobre la práctica:
 *
 * En lugar de realizar una única función capturadora, se ha optado por separar
 * el comportamiento para cada señal, de forma que se evita introducir un
 * switch adicional de comprobación del tipo de señal en las funciones.
 *
 * Los capturadores no se han declarado static ya que este programa va a ser
 * ejecutado de forma aislada. Por tanto, que las funciones sean visibles
 * únicamente desde el mismo no tendría ningún efecto.
 *
 * Se ha optado por utilizar signal en lugar de sigaction en aras de la
 * simplificación del código, ya que en este programa no resultan necesarias
 * ninguna de las funcionalidades extra que permite sigaction.
 */


// Función capturadora de la señal sigint
void gestion_sigint(int numero_de_senhal);
// Función capturadora de la señal sighup
void gestion_sighup(int numero_de_senhal);


int main(){
    /*
     * Cambiamos la reacción a la señal SIGINT de la interrupción del programa
     * (CtrlC) a la ejecución de una función personalizada, gestion_sigint.
     */
    if (signal(SIGINT, gestion_sigint) == SIG_ERR){
        perror("Error al crear el gestor de SIGINT\n");
        exit(EXIT_FAILURE);
    }

    /*
     * Cambiamos la reacción a la señal SIGHUP, que se envía cuando se cierra
     * la terminal que está controlando el proceso. Se deriva a una función
     * personalizada, gestion_sighup.
     */
    if (signal(SIGHUP, gestion_sighup) == SIG_ERR){
        perror("Error al crear el gestor de SIGHUP\n");
        exit(EXIT_FAILURE);
    }

    // Imprimimos el PID del proceso para facilitar la ejecución de kill()
    printf("Mi PID es %d\n", getpid());

    printf("Esperando a la introducción de SIGHUP desde la terminal...\n");
    /*
     * En este punto, el usuario debe enviar SIGHUP desde consola
     * (como con "kill -SIGHUP  <pid>" o "kill -s SIGHUP <pid>", siendo
     * <PID> el PID del proceso).
     */
    for(;;);        // Bucle infinito

    exit(EXIT_SUCCESS);   // En la ejecución planeada, no se llegaría aquí
}


/*
 * Función manejadora de la señal SIGINT
 *
 * Esta función provoca que la señal SIGINT (generada, por ejemplo, con
 * CtrlC) no cierre el proceso, y que únicamente muestre un mensaje.
 * Tiene un argumento de entrada, el número de la señal que ha provocado su
 * ejecución (por construcción del programa, siempre será SIGINT).
 */
void gestion_sigint(int numero_de_senhal){
    printf("Señal SIGINT atrapada y desactivada\n");
}

/*
 * Función manejadora de la señal SIGHUP
 * Esta función restaura SIGINT a su funcionamiento habitual.
 *
 * Recibe un argumento de entrada, el número de la señal que ha provocado su
 * ejecución (por construcción del programa, siempre será SIGHUP).
 *
 * Nótese que SIGHUP se envía automáticamente al proceso cuando se cierra
 * la terminal desde donde se ejecuta.
 */
void gestion_sighup(int numero_de_senhal){
    if (signal(SIGINT, SIG_DFL) == SIG_ERR){
        perror("Señal SIGHUP atrapada. Error en la restauración de "
                "SIGINT\n");
        exit(EXIT_FAILURE);
    }

    /*
     * Se ha optado por imprimir el mensaje de éxito después de restaurar
     * el funcionamiento habitual de CtrlC y no antes, ya que la función
     * signal() puede fallar. Si no, estaríamos imprimiendo un mensaje de
     * éxito seguido de uno de error, con lo que no quedaría claro lo ocurrido.
     */
    printf("Señal SIGHUP atrapada. Restaurado comportamiento de "
            "CtrlC!\n");
}
