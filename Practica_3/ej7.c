#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 *
 */



void gestion_sigusr1(int numero_de_senhal);
void gestion_sigusr2(int numero_de_senhal);
int crear_proceso();
void ejecutar_h1(int p, int n1);
void ejecutar_n1(int p);
void imprimir_mensaje_y_hora(char * msg, int error);

int main(){
    int p;
    int div_h1, div_n1;
    int status;
    struct sigaction nueva_accion;
    sigset_t mascara, pendientes;
    int es_miembro;


    /*
     * Para evitar posibles retrasos en puntos posteriores del programa,
     * establecemos los gestores de señales antes de realizar cualquier fork().
     * Además, esto nos permite comprobar con prontitud que este paso se
     * realiza sin errores, ya que es uno de los puntos críticos del programa.
     *
     * Esto implica que tanto H1 como N1 tendrán el mismo comportamiento
     * que establezcamos para P con respecto a las señales SIGUSR1 y
     * SIGUSR2, pero no es un detalle relevante, puesto que ni H1 ni N1
     * recibirán estas señales.
     */


    // Damos la dirección de la función a ejecutar al recibir la señal
    nueva_accion.sa_handler = gestion_sigusr1;
    // Dejamos vacío el conjunto de señales bloqueadas (de indicar alguna,
    // quedaría como pendiente hasta que se libere el bloqueo).
    sigemptyset(&nueva_accion.sa_mask);
    // Usamos SA_RESTART para forzar el reiniciado de ciertas llamadas del
    // sistema (como waitpid()) cuando son interrumpidas por un gestor de señal
    nueva_accion.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &nueva_accion, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo establecer el gestor "
                "de SIGUSR1", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    // El gestor de SIGUSR2 será la función gestion_sigusr2().
    // Reutilizamos el resto de parámetros.
    nueva_accion.sa_handler = gestion_sigusr2;
    if (sigaction(SIGUSR2, &nueva_accion, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo establecer el gestor "
                "de SIGUSR2", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy P.\n\tSe han establecido los gestores de "
            "SIGUSR1 y SIGUSR2", 0);

    // Antes de crear a H1, P bloquea la señal SIGUSR1. De recibirla, quedará
    // pendiente hasta que se desbloquee.
    sigemptyset(&mascara);      // En primer lugar, vaciamos la máscara
    sigaddset(&mascara, SIGUSR1);   // Añadimos la señal SIGUSR1 a la máscara
    // Añadimos la máscara al conjunto de señales bloqueadas.
    if (sigprocmask(SIG_BLOCK, &mascara, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo bloquear SIGUSR1", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy P.\n\tSe ha bloqueado la senhal SIGUSR1", 0);


    // El padre crea al hijo H1
    if (!(div_h1 = crear_proceso())){
        p = getppid();      // H1 guarda el PID de P (también lo tendrá N1)
        // H1 crea N1 y este salta a su repectiva función
        if (!(div_n1 = crear_proceso())) ejecutar_n1(p);
        ejecutar_h1(p, div_n1);    // Trabajo de H1
    }

    // Solo P sigue ejecutando el main() a partir de este punto

    imprimir_mensaje_y_hora("Soy P.\n\tVoy a dormir hasta que me despierte "
            "una senhal", 0);
    pause();   // pause() solo termina cuando se recibe una señal que termina
               // el proceso o que se deriva a un gestor de señales
    imprimir_mensaje_y_hora("Soy P.\n\tHe despertado", 0);


    /*
     * El padre comprueba que SIGUSR1 está pendiente (la recibió mientras
     * la señal aún estaba bloqueada). Para ello, guarda en pendientes el
     * conjunto de señales que aún no han sido procesadas.
     */
    if (sigpending(&pendientes) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se ha podido comprobar qué "
                "señales están pendientes en P", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    // Comprobamos si SIGUSR1 está presente en pendientes
    if ((es_miembro = sigismember(&pendientes, SIGUSR1)) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se ha podido comprobar si "
            "SIGUSR1 es una de las senhales que están pendientes", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    } else if (es_miembro == 0){
        imprimir_mensaje_y_hora("Soy P.\n\tError: No se ha recibido la senhal "
                "SIGUSR1", 1);
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy P.\n\tEl procesamiento de la senhal "
            "SIGUSR1 está pendiente. Procedo a desloquearla", 0);

    // Podemos reutilizar mascara, ya que sigprocmask no la modifica
    // (es un parámetro constante)
    // Eliminamos SIGUSR1 del conjunto de señales bloqueadas
    if (sigprocmask(SIG_UNBLOCK, &mascara, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se ha podido desbloquear la "
                "senhal SIGUSR1", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    // P espera a que termine H1, que a su vez espera a N1
    if (waitpid(div_h1, &status, 0) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tHa tenido lugar un error al "
                "esperar por H1", 1);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    if (!WIFEXITED(status)){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo recuperar el PID del "
                "nieto.", 1);
        exit(EXIT_FAILURE);
    }

    /*
     * Si el hijo ha terminado correctamente (WIFEXITED() != 0),
     * podemos utilizar WEXITSTATUS(() para comprobar los 8 bits menos
     * significativos del código de salida de H1. Aunque no podremos
     * comprobar el valor del código completo, H1 imprimirá también
     * el resultado de operar en módulo 2^8 = 256 para compararlos.
     */
    imprimir_mensaje_y_hora("Soy P", 0);
    // Separamos los mensajes para pasar argumentos (no hay riesgo de
    // solapamiento con líneas de otros procesos porque ya han acabado)
    printf("PID del nieto mod 256: %d\n", WEXITSTATUS(status));

    exit(EXIT_SUCCESS);
}


/*
 * Por motivos de eficiencia, se ha decidido separar el manejo de las señales
 * en 2 funciones distintas, para no tener que realizar comprobaciones sobre
 * el número de señal que estas reciben.
 */

/*
 * Función gestora de la señal SIGUSR1. Imprime un mensaje sobre su recepción.
 * Recibe un único argumento de entrada: el número de la señal que ha
 * provocado su ejecución.
 */
void gestion_sigusr1(int numero_de_senhal){
    printf("Recibida SIGUSR1!\n");
}

/*
 * Función gestora de la señal SIGUSR2. Imprime un mensaje sobre su recepción.
 * Recibe un único argumento de entrada: el número de la señal que ha
 * provocado su ejecución.
 */
void gestion_sigusr2(int numero_de_senhal){
    printf("Recibida SIGUSR2!\n");
}


/*
 * Función en la que trabaja H1.
 *
 * Parámetros:
 *      - p: (entrada) PID de P.
 *      - n1: (entrada) PID de N1.
 *
 * H1 finaliza en esta función.
 */
void ejecutar_h1(int p, int n1){
    int status;

    sleep(1);

    imprimir_mensaje_y_hora("Soy H1.\n\tEmpiezo mi trabajo");

    if (kill(p, SIGUSR1) == -1){
        perror("H1 no pudo enviar SIGUSR1 a P");
        exit(n1);
    }

    imprimir_mensaje_y_hora("Soy H1\n\tSenhal SIGUSR1 enviada a P");

    if (waitpid(-1, &status, 0) == -1){
        perror("Error en H1 al esperar por N1");
        exit(n1);
    }

    imprimir_mensaje_y_hora("Soy H1\n\tTras finalizar N1, yo también "
            "termino...");

    exit(n1);
}


/*
 * Función en la que trabaja N1.
 *
 * Recibe un único parámetro: el PID de P (entrada).
 *
 * N1 finaliza en esta función.
 */
void ejecutar_n1(int p){

    sleep(1);

    imprimir_mensaje_y_hora("Soy N1.\n\tEmpiezo mi trabajo");
    printf("\tPID de N1: %d\n", getpid());

    if (kill(p, SIGUSR2) == -1){
        perror("N1 no pudo enviar SIGUSR2 a P");
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy N1.\n\tSenhal SIGUSR2 enviada a P");

    sleep(5);
    imprimir_mensaje_y_hora("Soy N1.\n\tHan pasado 5 s; finalizando...");
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
    } else return res;
}

void imprimir_mensaje_y_hora(char * msg, int error){
    time_t t;                // Guardará el resultado de time()
    struct tm *t_local;      // Tiempo en la zona horaria local
    struct timeval t_milis;  // Guardará el resultado de gettimeofday()
    // La estructura timeval permite guardar segundos y milisegundos

    // La función time() devuelve el número de segundos que han pasado desde
    // el inicio del día 1 de enero de 1970.
    if ((t = time(NULL)) == -1){
        perror("Error al comprobar la hora actual");
        exit(EXIT_FAILURE);
    }

    // localtime() devuelve un puntero a una estructura tm con campos con
    // el valor temporal de t convertido a la zona horaria local
    t_local = localtime(&t);

    /*
     * Utilizamos también la función gettimeofday() para conseguir
     * precisión en milisegundos (gettimeofday() también indica los segundos,
     * pero estos ya son accesibles desde t_local).
     * El segundo argumento es NULL, ya que su uso está obsoleto (se mantiene
     * por interés histórico; contendría una struct timezone).
     */
    if (gettimeofday(&t_milis, NULL) == -1){
        perror("Error al comprobar los milisegundos de la hora actual");
        exit(EXIT_FAILURE);
    }

    if (error){
        // Imprimimos 6 dígitos para los milisegundos
        fprintf(stderr, "Hora: %d:%d:%d:%06ld. %s\n", t_local->tm_hour,
                t_local->tm_min, t_local->tm_sec, t_milis.tv_usec, msg);
        return;
    }

    printf("Hora: %d:%d:%d:%06ld. %s\n", t_local->tm_hour, t_local->tm_min,
        t_local->tm_sec, t_milis.tv_usec, msg);
}
