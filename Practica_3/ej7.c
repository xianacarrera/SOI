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
 * Práctica 3 - Ejercicio 7
 *
 * Este programa utiliza una configuración personalizada de las señales
 * SIGUSR1 y SIGUSR2 para un proceso padre, P. El flujo es el siguiente:
 *   - P asigna gestores a SIGUSR1 y SIGUSR2.
 *   - P bloquea la señal SIGUSR1.
 *   - P crea un proceso hijo, H1, que a su vez genera un nieto, N1. Después,
 *          P se bloquea con pause().
 *   - H1 envía la señal SIGUSR1 a P y espera a que termine N1.
 *   - N1 envía la señal SIGUSR2 a P y termina.
 *   - H1 termina. Al mismo tiempo, P despierta al recibir SIGUSR2.
 *   - P comprueba que la senhal SIGUSR1 está pendiente y la desbloquea.
 *   - P procesa SIGUSR1.
 *   - P espera a que H1 acabe (ya habrá sucedido en este punto). Recupera
 *          el valor de su exit(), que será el PID de N1, y lo muestra.
 *   - P termina.
 *
 * Estos pasos se indican por consola mostrando la hora con precisión de
 * milisegundos.
 */

#define ROJO "\e[1;31m"        // Proceso padre
#define VERDE "\e[1;32m"       // Proceso hijo
#define MAGENTA "\e[1;35m"     // Gestión de señales
#define CYAN "\e[1;36m"        // Proceso nieto
#define RESET "\e[0m"          // Errores (color predeterminado)


// Función gestora de SIGUSR1
void gestion_sigusr1(int numero_de_senhal);
// Función gestora de SIGUSR2
void gestion_sigusr2(int numero_de_senhal);

// Función en la que trabaja H1
void ejecutar_h1(int p, int n1);
// Función en la que trabaja N1
void ejecutar_n1(int p);

// Encapsulación de un fork() con tratamiento de errores
pid_t crear_proceso();
// Función que imprime la fecha y hora actuales junto con un mensaje
// (que puede ser de error o no) pudiendo elegir el color
void imprimir_mensaje_y_hora(char * msg, int error, char * color);



int main(){
    struct sigaction nueva_accion;   // Estructura para definir con sigaction()
                                     // la manera de reaccionar a una
    sigset_t mascara;        // Conjunto de señales bloqueadas
    sigset_t pendientes;     // Conjunto de señales pendientes de gestionarse
    pid_t p;                 // PID del proceso padre
    pid_t div_h1, div_n1;    // Resultados de fork() para H1 y N1
    int es_miembro;          // Para comprobar si SIGUSR1 está pendiente
    int status;              // Estado para la función watipid()


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
     *
     * Lo mismo aplica para la máscara de señales bloqueadas, que también se
     * hereda. Dado que ni H1 ni N1 reciben SIGUSR1, nos es indiferente que
     * ellos no puedan procesar dicha señal.
     */


    // Damos la dirección de la función a ejecutar al recibir SIGUSR1
    nueva_accion.sa_handler = gestion_sigusr1;
    // Dejamos vacío el conjunto de señales bloqueadas (de indicar alguna,
    // quedaría como pendiente hasta que se libere el bloqueo).
    sigemptyset(&nueva_accion.sa_mask);
    // Usamos SA_RESTART para forzar el reiniciado de ciertas llamadas del
    // sistema (como waitpid()) cuando son interrumpidas por un gestor de señal
    nueva_accion.sa_flags = SA_RESTART;

    // Establecemos el gestor para SIGUSR1 con sigaction()
    if (sigaction(SIGUSR1, &nueva_accion, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo establecer el gestor "
                "de SIGUSR1", 1, RESET);
        perror("Error");    // Imprimos el mensaje de error que da errno
        exit(EXIT_FAILURE);
    }

    // El gestor de SIGUSR2 será la función gestion_sigusr2().
    // Reutilizamos el resto de parámetros.
    nueva_accion.sa_handler = gestion_sigusr2;
    if (sigaction(SIGUSR2, &nueva_accion, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo establecer el gestor "
                "de SIGUSR2", 1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy P.\n\tSe han establecido los gestores de "
            "SIGUSR1 y SIGUSR2", 0, ROJO);

    // Antes de crear a H1, P bloquea la señal SIGUSR1. De recibirla, quedará
    // pendiente hasta que se desbloquee.
    sigemptyset(&mascara);      // En primer lugar, vaciamos la máscara
    sigaddset(&mascara, SIGUSR1);   // Añadimos la señal SIGUSR1 a la máscara
    // Añadimos la máscara al conjunto de señales bloqueadas.
    if (sigprocmask(SIG_BLOCK, &mascara, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo bloquear SIGUSR1",
                1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy P.\n\tSe ha bloqueado la senhal SIGUSR1",
            0, ROJO);


    // El padre crea al hijo H1
    if (!(div_h1 = crear_proceso())){
        p = getppid();      // H1 guarda el PID de P (también lo tendrá N1)
        // H1 crea N1 y este salta a su repectiva función
        if (!(div_n1 = crear_proceso())) ejecutar_n1(p);
        ejecutar_h1(p, div_n1);    // Trabajo de H1
    }


    // Solo P sigue ejecutando el main() a partir de este punto

    imprimir_mensaje_y_hora("Soy P.\n\tVoy a dormir hasta que me despierte "
            "una senhal", 0, ROJO);
    pause();   // pause() solo termina cuando se recibe una señal que termina
               // el proceso o que se deriva a un gestor de señales
    // Al desbloquearse, lo primero que hará P será gestionar la señal que
    // lo ha despertado (será SIGUSR2). Después, vuelve a este punto
    imprimir_mensaje_y_hora("Soy P.\n\tEstoy despierto", 0, ROJO);


    /*
     * El padre comprueba que SIGUSR1 está pendiente (la recibió mientras
     * la señal aún estaba bloqueada). Para ello, guarda en pendientes el
     * conjunto de señales que aún no han sido procesadas.
     */
    if (sigpending(&pendientes) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se ha podido comprobar qué "
                "señales están pendientes en P", 1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    // Comprobamos si SIGUSR1 está presente en pendientes
    if ((es_miembro = sigismember(&pendientes, SIGUSR1)) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se ha podido comprobar si "
            "SIGUSR1 es una de las senhales que están pendientes", 1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    } else if (es_miembro == 0){     // SIGUSR1 no está en pendientes
        imprimir_mensaje_y_hora("Soy P.\n\tError: No se ha recibido la senhal "
                "SIGUSR1", 1, RESET);
        exit(EXIT_FAILURE);
    }

    // sigismember() ha devuelto 1 (SIGUSR1 es un elemento de pendientes)
    imprimir_mensaje_y_hora("Soy P.\n\tSe ha verificado que el procesamiento "
            "de la senhal SIGUSR1 está pendiente. Procedo a desloquearla",
            0, ROJO);

    // Podemos reutilizar mascara, ya que sigprocmask no la modifica
    // (es un parámetro constante)
    // Eliminamos SIGUSR1 del conjunto de señales bloqueadas
    if (sigprocmask(SIG_UNBLOCK, &mascara, NULL) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se ha podido desbloquear la "
                "senhal SIGUSR1", 1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    /*
     * Al desbloquear SIGUSR1, automáticamente se gestiona la recepción de la
     * señal que estaba pendiente. Es decir, se llama a gestion_sigusr1.
     */

     imprimir_mensaje_y_hora("Soy P.\n\tVoy a esperar a que H1 termine",
             0, ROJO);

    // P espera a que termine H1, que a su vez esperó a N1
    if (waitpid(div_h1, &status, 0) == -1){
        imprimir_mensaje_y_hora("Soy P.\n\tHa tenido lugar un error al "
                "esperar por H1", 1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    if (!WIFEXITED(status)){
        imprimir_mensaje_y_hora("Soy P.\n\tNo se pudo recuperar el PID del "
                "nieto.", 1, RESET);
        exit(EXIT_FAILURE);
    }

    /*
     * Si el hijo ha terminado correctamente (WIFEXITED() != 0),
     * podemos utilizar WEXITSTATUS(() para comprobar los 8 bits menos
     * significativos del código de salida de H1. Aunque no podremos
     * comprobar el valor del código completo, H1 imprimirá también
     * el resultado de operar en módulo 2^8 = 256 para compararlos.
     */
    imprimir_mensaje_y_hora("Soy P.\n\tH1 ha terminado y ha indicado como "
            "valor de su exit() el PID de N1", 0, ROJO);
    // Separamos los mensajes para pasar argumentos (no hay riesgo de
    // solapamiento con líneas de otros procesos porque ya han acabado)
    printf("\t%sPID del nieto mod 256: %d%s\n",
            ROJO, WEXITSTATUS(status), RESET);

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
    /*
     * Aunque al dividir los mensajes para imprimir la hora y el PID nos
     * arriesgamos a que se solapen con mensajes de otros procesos, al solo
     * usar magenta para los gestores de señales será sencillo distinguirlos.
     */
    imprimir_mensaje_y_hora("", 0, MAGENTA);
    printf("\t%sMi PID es %d\n\tRecibida SIGURS1!%s\n",
            MAGENTA, getpid(), RESET);
}


/*
 * Función gestora de la señal SIGUSR2. Imprime un mensaje sobre su recepción.
 * Recibe un único argumento de entrada: el número de la señal que ha
 * provocado su ejecución.
 */
void gestion_sigusr2(int numero_de_senhal){
    /*
     * Aunque al dividir los mensajes para imprimir la hora y el PID nos
     * arriesgamos a que se solapen con mensajes de otros procesos, al solo
     * usar magenta para los gestores de señales será sencillo distinguirlos.
     */
    imprimir_mensaje_y_hora("", 0, MAGENTA);
    printf("\t%sMi PID es %d\n\tRecibida SIGURS2!%s\n",
            MAGENTA, getpid(), RESET);
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

    /*
     * Para tener la seguridad de que P imprime sus mensajes con respecto a
     * pause() sin interferencia por parte de H1, este espera 2 segundos
     * sin hacer nada.
     */
    sleep(2);

    imprimir_mensaje_y_hora("Soy H1.\n\tEmpiezo mi trabajo", 0, VERDE);
    printf("\t%sPID de H1: %d%s\n", VERDE, getpid(), RESET);

    /*
     * H1 le envía SIGUSR1 a P, que mantendrá la señal bloqueada hasta que
     * termine de ejecutar pause(). Esto ocurrirá cuando reciba SIGUSR2
     * por parte de N1.
     */
    if (kill(p, SIGUSR1) == -1){
        imprimir_mensaje_y_hora("Soy H1.\n\tNo pude enviar SIGUSR1 a P",
                1, RESET);
        perror("Error");
        exit(n1);
    }

    imprimir_mensaje_y_hora("Soy H1.\n\tSenhal SIGUSR1 enviada a P\n"
            "\tVoy a esperar a que N1 termine", 0, VERDE);

    // H1 espera a que N1 termine de ejecutarse
    if (waitpid(-1, &status, 0) == -1){
        imprimir_mensaje_y_hora("Soy H1.\n\tError al esperar por N1",
                1, RESET);
        perror("Error");
        exit(n1);
    }

    imprimir_mensaje_y_hora("Soy H1.\n\tTras finalizar N1, yo también "
            "termino...", 0, VERDE);

    /*
     * H1 indica el PID de N1 como valor de exit(). Una opción sería recuperar
     * dicho PID a través de waitpid(), si N1 eligiera tal valor como código
     * de su respectivo exit(). No obstante, dado que el PID de N1 fue
     * accesible por H1 directamente al ejecutar fork(), lo tomamos de ahí.
     */
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

    /*
     * Para tener la seguridad de que a P le da tiempo a ejecutar pause()
     * antes de que N1 le envíe una señal SIGUSR2, N1 se pone a dormir durante
     * 4 segundos preventivos. Además, dado que H1 solo va a dormir 2
     * segundos, tenemos la seguridad de que N1 enviará SIGUSR2 después
     * de que H1 haga lo propio con SIGUSR1.
     */
    sleep(4);

    imprimir_mensaje_y_hora("Soy N1.\n\tEmpiezo mi trabajo", 0, CYAN);
    printf("\t%sPID de N1: %d%s\n", CYAN, getpid(), RESET);
    /*
     * P no podrá ver el valor del PID de N1 completo, pues accederá a él a
     * través de WEXITSTATUS(), que solo devuelve los 8 bits menos
     * significativos del código de salida. Para poder determinar si el valor
     * es correcto, N1 muestra cuál es su PID en módulo 2^8=256.
     */
    printf("\t%sPID de N1 mod 256: %d%s\n", CYAN, getpid() % 256, RESET);

    /*
     * H1 envía SIGUSR2 al proceso padre. Esto lo despertará del pause(), y
     * gestionará la señal inmediatamente después.
     * Como SIGUSR1 seguirá bloqueada, P procesará SIGUSR2 antes que SIGUSR1,
     * a pesar de que las recibió en orden inverso.
     */
    if (kill(p, SIGUSR2) == -1){
        imprimir_mensaje_y_hora("Soy N1.\n\tNo pude enviar SIGUSR2 a P",
                1, RESET);
        perror("Error");
        exit(EXIT_FAILURE);
    }

    imprimir_mensaje_y_hora("Soy N1.\n\tSenhal SIGUSR2 enviada a P\n"
            "\tVoy a esperar 5 segundos", 0, CYAN);

    sleep(5);    // N1 duerme 5 segundos antes de terminar
    imprimir_mensaje_y_hora("Soy N1.\n\tHan pasado 5 s; finalizando...",
            0, CYAN);
    exit(EXIT_SUCCESS);
}



/*
 * Función que encapsula un fork() junto al tratamiento de sus errores.
 * Si hay algún fallo, el proceso termina. En caso contrario, la función
 * devuelve el resultado del fork().
 */
pid_t crear_proceso(){
    pid_t res;

    if ((res = fork()) == -1){
        perror("Error - creación de un proceso hijo fallida");
        exit(EXIT_FAILURE);
    } else return res;
}


/*
 * Función auxiliar que imprime la hora actual junto a un mensaje pasado por
 * el usuario. Si se trata de un mensaje de error, utiliza fprintf y el
 * stream stderr. En caso contrario, usa printf. Además, el texto se muestra
 * con el color indicado como parámetro.
 *
 * Parámetros (todos de entrada):
 *      - msg: cadena de texto a imprimir después de la hora
 *      - error: debe ser 0 si no se trata de un error y !0 en caso contrario
 *      - color: color en el que se mostrará el texto (una de las constantes
 *               definidas en el programa: CYAN, MAGENTA, VERDE, ROJO o
 *               RESET, para usar el color predeterminado).
 */
void imprimir_mensaje_y_hora(char * msg, int error, char * color){
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
        fprintf(stderr, "%sHora: %d:%d:%d:%06ld. %s%s\n",
                color, t_local->tm_hour, t_local->tm_min, t_local->tm_sec,
                t_milis.tv_usec,msg, RESET);
    }

    printf("%sHora: %d:%d:%d:%06ld. %s%s\n",
            color, t_local->tm_hour, t_local->tm_min, t_local->tm_sec,
            t_milis.tv_usec,msg, RESET);
}
