#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 2 - Ejercicio 7
 *
 * Este programa compara los resultados de realizar la media de las tangentes
 * de las raíces cuadradas de los primeros 50.000.000 números naturales
 * cuando se calcula para números pares e impares por separado, sumando
 * después lo obtenido, con cuando se calcula para todos los naturales de
 * forma conjunta.
 * Para ello, el proceso principal crea 5 hijos. Los hijos 1, 2 y 4 se encargan
 * de las operaciones con pares e impares; el hijo 3, del cálculo conjunto; y
 * el hijo 5 (ejecutado en otro programa a través de un execv) de la .
 * comparación final. Los resultados intermedios se almacenan en ficheros.
 */


# define N 50000000
/*
 * Aunque N se ha dejado como 50.000.000 como indica el enunciado, es
 * recomendable cambiarlo en función de las capacidades de la máquina en la
 * que se pruebe el programa. En mi caso, he realizado las pruebas con
 * 500.000.000.
 */

/*
 * Opciones de los argumentos:
 * Por defecto, este programa utilizará el fichero
 * resultados_medias_separadas.txt para las operaciones de los hijos 1, 2 y 4
 * y resultado_media_conjunta.txt para las operaciones del hijo 3. No obstante,
 * se pueden elegir otros a través de los argumentos.
 *
 * - Si no hay argumentos, se usan los ficheros predeterminados.
 * - Si hay 1 argumento, este se cambia por resultados_medias_separadas.txt
 *   como el fichero de los hijos 1, 2 y 4. El del hijo 3 se mantiene.
 * - Si hay 2 argumentos,
 *      + Si el primer argumento es "x", se sustituye el fichero del hijo 3
 *        por el segundo argumento. El de los hijos 1, 2 y 4 se mantiene.
 *      + Si el primer argumento no es "x", los ficheros
 *        "resultados_medias_separadas.txt" y
 *        "resultado_media_conjunta.txt" se sustituyen por el primer y
 *        segundo argumento, respectivamente.
 *
 * El fichero seleccionado, sea uno de los predeterminados o no, se crea
 * si no existía previamente. Si ya existía, se sobreescribe por completo.
 */

// Funciones auxiliares
int abrir_fichero(char * nombre_fichero, int leer);
int crear_proceso();
void cerrar_con_error(char * mensaje, int ver_errno);
void liberar_buffer_y_cerrar(int fichero);

// Funciones de los hijos 1, 2, 3 y 4
void calcular_media_par();
void calcular_media_impar();
void calcular_media_conjunta();
void sumar_media_par_impar();

int file_sep, file_conj;     // Descriptores de los ficheros
// Son variables globales por comodidad para la gestión de errores


int main(int argc, char* argv[]){
    char *args_exec[] = {"./quinto_hijo", "resultados_medias_separadas.txt",
                         "resultado_media_conjunta.txt", NULL};
                            // Argumentos para el execv del hijo 5
    int div[4];             // Resultados de los 4 primeros fork()
    int status;             // Almacena el estado en las llamadas a waitpid
    int i;                  // Variable de iteración


    // Abrimos el primer archivo en modo escritura (para los hijos 1, 2 y 4) y
    // lectura (para el hijo 4). El segundo archivo se abre únicamente en modo
    // escritura, para el hijo 3.
    // Ambos ficheros se crean si no existían previamente.
    if (argc == 3){
        if (strcmp(argv[1], "x") != 0){    // Cadenas de texto distintas
            file_sep = abrir_fichero(argv[1], 1);
            // Actualizamos los argumentos que recibirá el quinto hijo
            args_exec[1] = argv[1];
        } else
            file_sep = abrir_fichero("resultados_medias_separadas.txt", 1);

        file_conj = abrir_fichero(argv[2], 0);
        args_exec[2] = argv[2];

    } else if (argc <= 2){     // argc es 0, 1 o 2
        // Nótese que argc valdría 0 si, por ejemplo, lanzamos el programa a
        // través de una función de la familia exec
        if (argc == 2){
            file_sep = abrir_fichero(argv[1], 1);
            args_exec[1] = argv[1];
        } else
            file_sep = abrir_fichero("resultados_medias_separadas.txt", 1);

        file_conj = abrir_fichero("resultado_media_conjunta.txt", 0);

    } else {
        fprintf(stderr, "Número de argumentos incorrecto\n");
        exit(EXIT_FAILURE);
    }

    // Creamos los 3 primeros hijos. Los hijos 4 y 5 aún no son necesarios;
    // se generarán en puntos posteriores del programa.
    for (i = 0; i < 3; i++)
        if (!(div[i] = crear_proceso())) break;
        /*
         * Para facilitar la gestión de fallos, encapsulamos fork() en la
         * función crear_proceso(), que corta la ejecución en caso de error.
         * Guardamos los resultados de los fork() en el array div.
         *
         * Los hijos salen inmediatamente del bucle, de forma que solo el padre
         * pasa a la siguiente iteración. Solo él genera nuevos procesos.
         */

    /*
     * Dado que los hijos son clones del padre en el momento de su nacimiento,
     * el hijo de índice i tendrá div[i] = 0 y div[j] != 0 para todo j < i,
     * ya que este último fue el valor con el que se quedó el padre.
     * Por tanto, en el siguiente condicional, el hijo i entrará en la rama
     * número i, para cada i entre 1 y 3. El padre no entra en ninguna.
     */
    if (!div[0]) calcular_media_par();                 // Hijo 1
    else if (!div[1]) calcular_media_impar();          // Hijo 2
    else if (!div[2]) calcular_media_conjunta();       // Hijo 3

    // Solo el padre sigue vivo en este punto

    // Espera por los dos primeros hijos antes de crear un cuarto
    for (i = 0; i < 2; i++){
        // Esperamos por el proceso de PID div[i]. Guardamos en status
        // información sobre el mismo, y no indicamos opciones extra (0).
        if (waitpid(div[i], &status, 0) == -1)
            // Imprimimos errno, cerramos ficheros y cortamos la ejecución
            cerrar_con_error("Error en la invocación de waitpid", 1);

        // Comprobamos también que la finalización del hijo fuera correcta
        if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
             /*
             * Si WIFEXITED(status) es 0, el hijo no terminó con un exit() o
             * volviendo del main. Sería el caso de finalización por una señal.
             * Si WIFEXITED(status) es 1, podemos comprobar WEXITSTATUS para
             * determinar cuál fue el valor del exit del hijo.
             *
             * Si hay error también cerramos los ficheros y cortamos la
             * ejecución, pero en este caso no imprimimos errno.
             */
             cerrar_con_error("Finalización incorrecta de proceso hijo", 0);
    }

    // Generamos el hijo 4, que entra en el if. El padre continúa fuera.
    if (!(div[3] = crear_proceso())) sumar_media_par_impar();

    // Antes de continuar, se espera a que todos los procesos hijos hayan
    // terminado. Como ya se esperó por 1 y 2, solo 3 y 4 podrían seguir vivos.
    for (i = 2; i < 4; i++){
        // Esperamos por el proceso de PID div[i]. Guardamos en status
        // información sobre el mismo, y no indicamos opciones extra (0).
        if (waitpid(div[i], &status, 0) == -1)
            // Imprimimos errno, cerramos ficheros y cortamos la ejecución
            cerrar_con_error("Error en la invocación de waitpid", 1);

        // Comprobamos también que la finalización del hijo fuera correcta
        if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
             /*
             * Si WIFEXITED(status) es 0, el hijo no terminó con un exit() o
             * volviendo del main. Sería el caso de finalización por una señal.
             * Si WIFEXITED(status) es 1, podemos comprobar WEXITSTATUS para
             * determinar cuál fue el valor del exit del hijo.
             *
             * Si hay error también cerramos los ficheros y cortamos la
             * ejecución, pero en este caso no imprimimos errno.
             */
             cerrar_con_error("Finalización incorrecta de proceso hijo", 0);
    }


    // Antes de crear el quinto hijo, cerramos ambos ficheros ya que, dado que
    // va a realizar un execv, este no tendría acceso aunque los dejáramos
    // abiertos en el proceso padre indefinidamente.
    if (close(file_sep) < 0){
        // No se usa cerrar_con_error porque llamaría a close de nuevo
        perror("Error al cerrar el fichero de medias separadas");
        exit(EXIT_FAILURE);
    }
    if (close(file_conj) < 0){
        perror("Error al cerrar el fichero de la media conjunta");
        exit(EXIT_FAILURE);
    }

    if (!crear_proceso()){           // El hijo 5 entra en el if; el padre no
        /*
         * Borramos los datos del espacio de direcciones con execv, y cargamos
         * un nuevo programa (nuevo código, pila y datos).
         * Indicamos como primer argumento de execv la dirección del programa
         * a cargar. El segundo argumento es un array de strings. Su primer
         * elemento es también la dirección del programa que sustituirá al
         * actual. El resto de elementos son los argumentos que se le pasarán.
         * El array debe terminar en NULL.
         * El hijo seguirá ejecutará la siguiente línea ya en el otro programa.
         */
        execv(args_exec[0], args_exec);
    }

    exit(EXIT_SUCCESS);
}

/*
 * Función auxiliar que abre un fichero.
 * En caso de error, imprime un mensaje de error, la descripción de errno, y
 * finaliza la ejecución. En caso de éxito, devuelve el descriptor del fichero.
 *
 * Parámetros (ambos de entrada):
 *     - nombre_fichero: nombre del fichero a abrir.
 *     - leer: 0 si solo se quiere escribir; !0 si se quiere leer y escribir.
 */
int abrir_fichero(char * nombre_fichero, int leer){
    int fichero;

    /*
     * Abrimos el archivo con la opción O_RDWR, en caso de lectura y escritura,
     * o O_WRONLY, en caso de solo lectura. Además, indicamos 0_TRUNC para que
     * el fichero se borre si existía previamente, y O_CREAT para que, en caso
     * contrario, se cree. De generarse un nuevo archivo, lo hará con modo
     * 0664, donde el 0 indica que es un archivo y 664 establece los permisos
     * (rw-rw-r--, esto es, 6, 6 y 4 para el usuario actual, el grupo y los
     * otros, respectivamente).
     */
    if ((fichero = open(nombre_fichero,
                        (leer? O_RDWR : O_WRONLY) | O_CREAT | O_TRUNC, 0664)
                    ) < 0){
        // Imprimimos con fprintf antes de llamar a perror para mostrar el
        // nombre del fichero
        fprintf(stderr, "Error en la apertura del fichero %s", nombre_fichero);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fichero;
}

/*
 * Función auxiliar que ejecuta un fork y gestiona los errores.
 * Si es exitosa, devuelve el resultado del fork: 0 para el proceso hijo
 * y !0, el PID del hijo, para el padre. En caso de error, termina el programa.
 */
int crear_proceso(){
    int div;

    if ((div = fork()) == -1){
        // Imprimimos un mensaje de error junto con la descripción del código
        // al que se ha puesto errno y salimos del programa.
        cerrar_con_error("Error en la creación de un proceso hijo", 1);
    }
    // Otros casos: 0 (para el hijo) o un entero positivo (PID del hijo, para
    // el padre)
    return div;
}

/*
 * Función auxiliar que cierra los dos ficheros utilizados, imprime un mensaje
 * de error, y finaliza la ejecución.
 *
 * Parámetros (ambos de entrada):
 *   - mensaje: descripción personalizada del error que será imprimida.
 *   - ver_errno: !0 para indicar mensaje junto a la descripción de errno,
 *                 0 para mostrar solo el argumento "mensaje".
 */
void cerrar_con_error(char * mensaje, int ver_errno){
    /*
     * Si ver_errno es !0, se indica qué ha ido mal en el sistema a través de
     * la macro errno. Se imprime mensaje seguido de ": " y el significado del
     * código que tiene errno.
     */
    if (ver_errno) perror(mensaje);
    else fprintf(stderr, "%s", mensaje);    // Solo se imprime mensaje

    // Se intenta cerrar ambos ficheros. Si close devuelve -1, se indica qué
    // ha ocurrido a través de errno.
    if (close(file_sep) < 0)
        perror("Error al cerrar el fichero de medias separadas");
    if (close(file_conj) < 0)
        perror("Error al cerrar el fichero de la media conjunta");

    exit(EXIT_FAILURE);
}


/*
 * Función auxiliar que aúna las operaciones finales de cada hijo:
 * liberar el buffer en el que ha escrito y cerrar los dos ficheros que tiene
 * abiertos.
 * En caso de error, finaliza la ejecución con un mensaje de fallo.
 * Si hay éxito, simplemente finaliza.
 *
 * Parámetros (entrada):
 *    - fichero: fichero cuyo buffer se debe vaciar
 */
void liberar_buffer_y_cerrar(int fichero){
    /*
     * Para asegurar que el resultado se ha escrito sobre el fichero, liberamos
     * el buffer con fsync. Además, la ejecución del programa no continuará
     * hasta que fsync haya acabado, lo que nos permite trabajar con la
     * seguridad de que la información del fichero está completa.
     */
    if (fsync(fichero) < 0)
        // En caso de error, lo notificamos, llamamos a perror y salimos.
        cerrar_con_error("Error en la liberación de buffer", 1);

    // El hijo dispone de ambos ficheros abiertos, pero sus descriptores son
    // independientes de los del padre, al ser copias. Por tanto, debe cerrar
    // los ficheros para sí mismo, y esto no afectará al resto de procesos.
    if (close(file_sep) < 0){
        // No usamos cerrar_con_error, porque llamaría a close de nuevo
        perror("Error al cerrar el fichero de medias separadas");
        exit(EXIT_FAILURE);
    }
    if (close(file_conj) < 0){
        // No usamos cerrar_con_error, porque llamaría a close de nuevo
        perror("Error al cerrar el fichero de la media conjunta");
        exit(EXIT_FAILURE);
    }
}


/*
 * Función a través de la cual trabaja el hijo 1. Realiza la media de la
 * tangente de la raíz cuadrada de los primeros 50.000.000 números naturales,
 * operando únicamente sobre los naturales pares. Imprime su resultado a
 * través de la consola y lo guarda en un fichero.
 * Solo el hijo 1 ejecuta esta función. Es paralelo a los hijos 2 y 3.
 */
void calcular_media_par(){
    double media_par = 0;      // Almacenará el resultado final
    int i;                     // Variable de iteración

    // Sumamos las tangentes de las raíces de los números pares
    // No se ha considerado al 0 como número natural
    for (i = 2; i <= N; i += 2) media_par += tan(sqrt(i));
    media_par /= N;    // Calculamos la media diviendo la suma total por N

    // Imprimimos el resultado por consola. Para dar una mayor precisión,
    // utilizamos 15 decimales.
    printf("Soy el primer hijo. He realizado el cálculo sobre los primeros "
            "%d números naturales pares. Resultado: media par = %.15f\n",
            N, media_par);

    // Registramos media_par en formato binario sobre file_sep. Dado que es un
    // double, el número de bytes que se deben escribir será sizeof(double)
    if (write(file_sep, &media_par, sizeof(double)) < 0)
        // Si la escritura del fichero falla, cortamos la ejecución
        cerrar_con_error("Hijo 1 - error en la escritura del fichero", 1);

    // Vaciamos el buffer para file_sep, además de cerrar este y file_conj
    liberar_buffer_y_cerrar(file_sep);

    exit(EXIT_SUCCESS);
}

/*
 * Función a través de la cual trabaja el hijo 2. Realiza la media de la
 * tangente de la raíz cuadrada de los primeros 50.000.000 números naturales,
 * operando únicamente sobre los naturales impares. Imprime su resultado a
 * través de la consola y lo guarda en un fichero.
 * Solo el hijo 2 ejecuta esta función. Es paralelo a los hijos 1 y 3.
 */
void calcular_media_impar(){
    double media_impar = 0;    // Almacenará el resultado final
    int i;                     // Variable de iteración

    // Sumamos las tangentes de las raíces de los números impares
    for (i = 1; i < N; i += 2) media_impar += tan(sqrt(i));
    media_impar /= N;   // Calculamos la media diviendo la suma total por N

    // Imprimimos el resultado por consola. Para dar una mayor precisión,
    // utilizamos 15 decimales.
    printf("Soy el segundo hijo. He realizado el cálculo sobre los primeros "
            "%d números naturales impares. Resultado: media impar "
            "= %.15f\n", N, media_impar);

    // Registramos media_impar en formato binario sobre file_sep. Dado que es
    // un double, el número de bytes que se deben escribir será sizeof(double)
    if (write(file_sep, &media_impar, sizeof(double)) < 0)
        // Si la escritura del fichero falla, cortamos la ejecución
        cerrar_con_error("Hijo 1 - error en la escritura del fichero", 1);

    // Vaciamos el buffer para file_sep, además de cerrar este y file_conj
    liberar_buffer_y_cerrar(file_sep);

    exit(EXIT_SUCCESS);
}

/*
 * Función a través de la cual trabaja el hijo 3. Realiza la media de la
 * tangente de la raíz cuadrada de los primeros 50.000.000 números naturales,
 * Imprime su resultado a través de la consola y lo guarda en un fichero,
 * distinto al utilizado por los hijos 1 y 2.
 * Solo el hijo 3 ejecuta esta función. Es paralelo a los hijos 1 y 2.
 */
void calcular_media_conjunta(){
    double media_conjunta = 0;      // Almacenará el resultado final
    int i;                          // Variable de iteración

    // Sumamos la tangente de la raíz de cada número
    for (i = 1; i <= N; i++) media_conjunta += tan(sqrt(i));
    media_conjunta /= N;      // Hacemos la media

    // Imprimimos el resultado por consola. Para dar una mayor precisión,
    // utilizamos 15 decimales.
    printf("Soy el tercer hijo. He realizado el cálculo sobre los primeros "
            "%d números naturales. Resultado: media = %.15f\n",
            N, media_conjunta);

    // Registramos la media en formato binario sobre file_conj. Dado que es un
    // double, el número de bytes que se deben escribir será sizeof(double)
    if (write(file_conj, &media_conjunta, sizeof(double)) < 0)
        // Si la escritura del fichero falla, cortamos la ejecución
        cerrar_con_error("Hijo 3 - error en la escritura del fichero", 1);

    // Vaciamos el buffer para file_conj, además de cerrar este y file_sep
    liberar_buffer_y_cerrar(file_conj);

    exit(EXIT_SUCCESS);
}

/*
 * Función a través de la cual trabaja el hijo 4. Suma las contribuciones que
 * los hijos 1 y 2 han guardado en el fichero file_sep y añade el total
 * a ese mismo archivo.
 * Solo el hijo 4 ejecuta esta función. Se ejecuta una vez han finalizado los
 * hijos 1 y 2.
 */
void sumar_media_par_impar(){
    double media_total = 0;    // Almacenará la suma de las medias par e impar
    double aux;                // Variable auxiliar
    int i;                     // Variable de iteración

    // Tenemos que volver al inicio del fichero, ya que ahora mismo el control
    // se encuentra en el punto final, después de lo escrito por ambos hijos.
    lseek(file_sep, 0, SEEK_SET);    // Utilizamos un offset de 0 bytes

    // Como forzamos la liberación del buffer en los dos primeros hijos con
    // fsync, sabemos que ambos han grabado sus resultados sobre el fichero.
    // Nótese que no sabemos en qué orden escribieron.
    for (i = 0; i < 2; i++){
        // Leemos un double en binario y lo guardamos en la variable auxiliar
        if (read(file_sep, &aux, sizeof(double)) == -1)
            // Si hay un error, llamamos a perror y cerramos el programa
            cerrar_con_error("Hijo 4 - Error al leer el fichero con los "
                    "resultados de los hijos 1 y 2", 1);
        media_total += aux;   // Añadimos la contribución al total
    }

    // La suma se añade a continuación, también en binario
    if (write(file_sep, &media_total, sizeof(double)) == -1)
        // Si la escritura del fichero falla, cortamos la ejecución
        cerrar_con_error("Hijo 4 - error en la escritura del fichero", 1);

    // Vaciamos el buffer para file_sep, además de cerrar este y file_conj
    liberar_buffer_y_cerrar(file_sep);

    exit(EXIT_SUCCESS);
}
