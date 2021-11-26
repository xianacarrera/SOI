#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>
#include<time.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 2 - ejercicios 1, 2 y 3
 *
 * Ejercicio 1 -> Creación de un proceso hijo e indicación de información
 * identificativa del mismo y de su padre a través de las llamadas al sistema
 * getpid, getppid, getuid, geteuid, getgid y getenv.
 *
 * Ejercicio 2 -> Comparativa de los valores y posiciones de memoria de
 * variables globales, locales y dinámicas del padre y del hijo.
 *
 * Ejercicio 3 -> Comprobación de que los ficheros abiertos por el padre
 * quedan abiertos para el proceso hijo.
 *
 */

// Funciones auxiliares
void imprimir_datos(short proceso);
void imprimir_variables(short proceso, int * var_local, int * p_a_var_dinam);
void imprimir_fichero(int file);

int var_global = 1;

int main(){
    int par;                 // Resultado del fork
    int status;              // Almacena el estado de la función waitpid()
    char c;                  // Input del scanf
    char v_env[1];           // Variable de entorno
    int file;                // Fichero compartido (ejercicio 3)
    char escr_hijo = 'H';    // Lo que escribe el hijo en el fichero
    char escr_padre = 'P';   // Lo que escribe el padre en el fichero

    // Variables del ejercicio 2
    int var_local = 2;
    int * p_a_var_dinam = (int *) malloc(sizeof(int));


    /********************* VARIABLES IDENTIFICATIVAS ************************/

    // Generamos una semilla para la generación aleatoria usando la hora actual
    srand(time(0));

    // Generamos una letra aleatoria para guardar en la variable de entorno
    v_env[0] = (char) 65 + rand() % 26; // Entre 65 y 90 (mayúsculas en ASCII)

    /*
     * Guardamos una nueva variable de entorno con nombre "SOI" y valor una
     * letra del abecedario aleatoria. El tercer argumento indica si se pueden
     * sobreescribir antiguos valores (!0) o no (0).
     * setenv devuelve 0 en caso de éxito y -1 en caso de error, guardando
     * en errno el código correspondiente al fallo
     */
    if (setenv("SOI", v_env, 1) == -1){
        // Podemos imprimir una descripción del error de errno con perror
        perror("Error en setenv");
        exit(EXIT_FAILURE);
    }

    // Imprimimos los datos del padre
    imprimir_datos(1);


    /************************ VARIABLES GENERALES **************************/

    * p_a_var_dinam = 3;

    // Imprimimos los valores de las variables del padre
    // Pasamos la referencia a la variable local, para imprimir su dirección
    // Pasamos la dirección (memoria dinámica) a la que apunta el puntero local
    imprimir_variables(1, &var_local, p_a_var_dinam);


    /********************** COMPARTICIÓN DE FICHEROS **********************/

    /*
     * Abrimos un fichero en modo lectura y escritura (O_RDWR) antes de
     * realizar el fork. O_TRUNC hace que se borre el contenido en caso de que
     * el fichero existiera previamente, y O_CREAT lo crea si no era así. En
     * este último caso, se utiliza el modo 664 (rw-rw-r--, esto es, 6, 6 y 4
     * para el usuario actual, el grupo y los otros, respectivamente). 0
     * indica que el número está en octal.
     */
    if ((file = open("fichero.txt", O_RDWR | O_CREAT | O_TRUNC, 0664)) < 0){
        perror("Error en la apertura del fichero (ejercicio 3)");
        exit(EXIT_FAILURE);
    }

    /****************************** FORK *********************************/

    if ((par = fork()) == -1){
        perror("Error en el fork");
        fprintf(stderr, "Abortando programa...\n");
        exit(EXIT_FAILURE);
    }

    /********************* EXTRA - QUIÉN RECIBE SCANF *********************/
    /*
    scanf("%c", &c);   // Para comprobar si le llega al padre o al hijo
    printf("He leído %c y soy el %s\n", c, par == 0? "hijo" : "padre");
    */


    /****************************** HIJO *********************************/

    if (par == 0){
        // Imprimimos los datos de los ejercicios 1 y 2
        imprimir_datos(0);
        imprimir_variables(0, &var_local, p_a_var_dinam);

        // Comprobamos que el hijo tiene acceso al fichero abierto por su padre
        // antes del fork. Para ello, escribimos una frase (ejercicio 3).
        if (write(file, &escr_hijo, sizeof(char)) < 0){
            perror("Error en la escritura del fichero por parte del hijo");
            exit(EXIT_FAILURE);
        }

        // El siguiente getchar() deja al hijo en espera cuando se usa el scanf
        // getchar();

        // Cambiamos los datos de las variables
        var_global = 4;
        var_local = 5;
        *p_a_var_dinam = 6;

        printf("Variables del hijo modificadas\n\n");
        // Imprimimos los nuevos resultados del hijo 2
        imprimir_variables(0, &var_local, p_a_var_dinam);

        /************************* HIJOS ADOPTIVOS *********************/
        /*
        printf("El hijo procede a matar a su padre...\n");
        // El hijo mata al padre para comprobar qué ocurre cuando es huérfano
        kill(getppid(), SIGKILL);

        sleep(2);   // Esperamos un tiempo para asegurar que el control llega
        // al padre. Entonces, este recibe la señal y acaba.

        printf("\n");
        printf("Ahora que el hijo es huérfano, el PID del padre es: %d\n\n",
                getppid());

        // Podemos ver quién es el padre adoptivo con pstree o ps: systemd
        // sleep(10);
        // printf("Temporizador finalizado\n");
        */


    } else {
        /*********************** FINAL DEL PADRE *************************/

        /*
         * El proceso padre espera a que el hijo escriba antes de terminar
         * Si el proceso padre terminara inmediatamente tras el fork() y
         * recibiera el control de la CPU antes que el hijo, este último
         * quedaría huérfano. En tal caso, getpid() del padre y getppid() del
         * hijo no coincidirían.
         */

        // Si tapamos el waitpid, podemos ver distinto orden en la impresión
        if (waitpid(-1, &status, 0) < 0){
            perror("Error en watipid");
            exit(EXIT_FAILURE);
        }

        imprimir_variables(1, &var_local, p_a_var_dinam);

        // El fichero sigue abierto para el padre.
        // Escribimos e imprimimos todo el fichero.
        if (write(file, &escr_padre, sizeof(char)) < 0){
            perror("Error en la escritura del fichero por parte del padre");
            exit(EXIT_FAILURE);
        }
        imprimir_fichero(file);

        close(file);
    }


    /*
     * NOTA: El prompt puede no aparecer al final, sino en el medio, ya que
     * el shell escribe cuando acaba el padre.
     * El prompt solo espera al padre.
     * Padre, hijo y prompt se pelean por la consola.
     */

    exit(EXIT_SUCCESS);
}




/* Ejercicio 1
 * Función auxiliar que imprime datos identificativos de un proceso a través
 * de las fuciones getpid, getppid, getuid, geteuid y getenv.
 *
 * Parámetros:
 *      proceso -> (entrada) 0 si es el proceso hijo, !0 para el padre. Solo
 *                           tiene efectos en la presentación de la salida.
 */
void imprimir_datos(short proceso){
    char letra_nueva;

    /*
     * getpid(), getppid(), getuid(), geteuid() y getgid() devuelven tipos de
     * variables específicas (pid_t, uid_t y gid_t), pero se utilizan como
     * enteros
     */

    printf("*********** PROCESO %s *********\n", proceso? "PADRE" : "HIJO");
    printf("\tEl PID del proceso actual es: %d\n", getpid());
    printf("\tEl PID del proceso padre es: %d\n", getppid());
    printf("\tEl ID del usuario real es: %d\n", getuid());
    printf("\tEl ID del usuario efectivo es: %d\n", geteuid());
    printf("\tEl ID del grupo real es: %d\n", getgid());

    printf("\tEl valor de SOI es: %s\n", getenv("SOI"));
    // Imprimimos el valor de la variable de entorno que acabamos de modificar

    // Entre 65 y 90 (mayúsculas en ASCII)
    letra_nueva = (char) 65 + rand() % 26;

    // Guardamos la letra generada. Este valor presumiblemente será diferente
    // en cada invocación de la función
    if (setenv("SOI_RAND", &letra_nueva, 1) == -1){
        // Podemos imprimir una descripción del error de errno con perror
        perror("Error en setenv");
        exit(EXIT_FAILURE);
    }

    printf("\tEl valor de SOI_RAND es: %s\n", getenv("SOI_RAND"));
}


/* Ejercicio 2
 * Función auxiliar que imprime la direción y el valor de las variables
 * locales, globales y dinámicas del ejercicio 2 de los procesos padre e hijo.
 *
 * Parámetros:
 *      proceso -> (entrada) 0 si es el proceso hijo, !0 para el padre. Solo
 *                           tiene efectos en la presentación de la salida.
 *      var_local -> (entrada) puntero a la variable local creada en el main
 *      p_a_var_dinam -> (entrada) puntero a la variable dinámica (no el
 *                                 puntero) creada en el main
 */
void imprimir_variables(short proceso, int * var_local, int * p_a_var_dinam){
    printf("*** ANÁLISIS DE VARIABLES DEL %s:\n", proceso? "PADRE" : "HIJO");
    printf("\tLa variable global tiene dirección %p y valor %d\n",
            &var_global, var_global);
    printf("\tLa variable local tiene dirección %p y valor %d\n",
            var_local, *var_local);
    printf("\tLa variable creada dinámicamente tiene dirección %p y "
            "valor %d\n", p_a_var_dinam, *p_a_var_dinam);

    printf("\n\n");
}


/* Ejercicio 3
 * Función auxiliar que lee todo el fichero e imprime su contenido.
 *
 * Parámetros:
 *    file -> (entrada) descriptor del fichero
 */
void imprimir_fichero(int file){
    char contenido;             // Guardará el contenido del fichero
    short res;                  // Control del bucle


    // Leemos todo el fichero
    printf("*********** LECTURA DEL FICHERO ***********\n");
    if (lseek(file, 0, SEEK_SET) < 0){    // Volvemos al inicio del fichero
        perror("Error en la función lseek");
        exit(EXIT_FAILURE);
    }

    do{     // Ejecutamos al menos una vez read
        res = read(file, &contenido, sizeof(char));
        if (res > 0){      // read ha tenido éxito y se ha leído algo
            printf("%c", contenido);
        } else if (res < 0){     // read ha dado error
            perror("Error en la lectura del fichero");
            exit(EXIT_FAILURE);
        }
    } while (res);
    printf("\n\n");
}
