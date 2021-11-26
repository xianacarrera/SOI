#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 2 - Ejercicio 7
 *
 * Este programa corresponde al quinto hijo del proceso principal del programa
 * del ejercicio 7. Su función es leer los resultados registrados por los
 * otros hijos en sus respectivos ficheros, calcular la diferencia entre los
 * mismos y mostrarla por pantalla. Se empieza a ejecutar tras un execv por
 * parte del hijo.
 */


// Función auxiliar
void cerrar_con_error(char * mensaje, int ver_errno);

int file_sep, file_conj;    // Descriptores de los ficheros
// Son variables globales por comodidad para la gestión de errores


int main(int argc, char* argv[]){
    double media_separada;      // Guardará el resultado del hijo 3
    double media_conjunta;      // Guardará el resultado del hijo 4

    printf("\n");      // Separación con el programa principal

    // Este programa espera recibir siempre 3 argumentos, pues sigue el flujo
    // lógico de ejercicio_7.c, que le pasa siempre el nombre de su ejecutable
    // y el nombre de los dos archivos que debe usar en sus cálculos.
    if (argc != 3){
        fprintf(stderr, "Hijo 5 - Error: número de argumentos incorrecto\n");
        exit(EXIT_FAILURE);
    }

    // Necesitamos leer los archivos, pero no escribir en ellos: O_RDONLY
    // Si los ficheros no existían previamente, como no indicamos O_CREAT,
    // open falla
    if ((file_sep = open(argv[1], O_RDONLY)) < 0){
        // Imprimimos con fprintf antes de llamar a perror para mostrar el
        // nombre del fichero
        perror("Hijo 5 - Error en la apertura del archivo con los resultados "
                "de la media de los hijos 1, 2 y 4");
        exit(EXIT_FAILURE);
    }
    if ((file_conj = open(argv[2], O_RDONLY)) < 0){
        // Imprimimos con fprintf antes de llamar a perror para mostrar el
        // nombre del fichero
        perror("Hijo 5 - Error en la apertura del archivo con el resultado "
                "de la media del hijo 3");
        exit(EXIT_FAILURE);
    }

    // Para ir directamente al resultado del hijo 4 en file_sep, avanzamos
    // 2 doubles (resultados de los hijos 1 y 2) desde el inicio del fichero
    if (lseek(file_sep, 2*sizeof(double), SEEK_SET) == -1)
        // Imprimimos el error con perror, cerramos los ficheros y finalizamos
        cerrar_con_error("Hijo 5 - Se ha detectado un error en los datos "
                "del fichero usado por los hijos 1, 2 y 4", 1);

    // Leemos el tercer número de file_sep
    if (read(file_sep, &media_separada, sizeof(double)) == -1)
        // Imprimimos el error con perror, cerramos los ficheros y finalizamos
        cerrar_con_error("Hijo 5 - Se ha detectado un error en los datos "
                "del fichero usado por los hijos 1, 2 y 4", 1);

    // Leemos el único double que contine file_conj
    if (read(file_conj, &media_conjunta, sizeof(double)) == -1)
        // Imprimimos el error con perror, cerramos los ficheros y finalizamos
        cerrar_con_error("Hijo 5 - Se ha detectado un error en los datos "
                "del fichero usado por el hijo 3", 1);

    // Se muestran los resultados leídos y la diferencia entre ellos
    // Se usan 15 decimales de precisión
    printf("Soy el quinto hijo (H5).\n");
    printf("H5 - Media separada leída (suma de las medias par e impar): "
            "%.15f\n", media_separada);
    printf("H5 - Media conjunta leída: %.15f\n", media_conjunta);
    printf("H5 - Resultado: media separada - media conjunta = %.15f\n",
            media_separada - media_conjunta);

    // Se intenta cerrrar ambos ficheros antes de terminar
    if (close(file_sep) < 0){
        // No se usa cerrar_con_error porque llamaría a close de nuevo
        perror("Hijo 5 - Error al cerrar el fichero de medias separadas");
        exit(EXIT_FAILURE);
    }
    if (close(file_conj) < 0){
        perror("Hijo 5 - Error al cerrar el fichero de la media conjunta");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
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
        perror("Hijo 5 - Error al cerrar el fichero de medias separadas");
    if (close(file_conj) < 0)
        perror("Hijo 5 - Error al cerrar el fichero de la media conjunta");

    exit(EXIT_FAILURE);
}
