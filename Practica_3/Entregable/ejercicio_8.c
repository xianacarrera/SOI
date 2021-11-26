#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

/*
 * Xiana Carrera Alonso
 * Sistemas Operativos I - Curso 2021/2022
 * Práctica 3 - Ejercicio 8
 *
 * Este programa calcula la sucesión 2^n con n creciente infinitamente,
 * comenzando en n = 0. Se utiliza la señal SIGALRM junto a la función alarm()
 * para mostrar el último valor capturado en cada segundo, hasta un máximo
 * número de veces establecido por el usuario.
 */


static void gestion(int numero_de_senhal);  // Función de gestión de SIGALRM
void calcular_potencias_2();         // Función que calcula la secuencia 2^n


/*
 * Al manejador de señales solo le podemos pasar un argumento, el número
 * de señal, de modo que tendremos que declarar la variable que almacenará
 * los valores de la secuencia como global para que el manejador tenga acceso.
 * También el contador de alarmas y el máximo número de alarmas deben ser
 * globales, para poder realizar esta comprobación desde el gestor en lugar
 * de desde el main(), puesto que sería más ineficiente.
 */
unsigned long x = 1;              // Valor actual de la secuencia 2^n
int max_capturas;                 // Número máximo de alarmas a establecer
int n_alarmas = 1;                // Contador de alarmas

int main(){
    struct sigaction nueva_accion;   // Estructura para la definición
        // personalizada de la reacción a la señal SIGALRM con sigaction()

    printf("¿Cuántas veces se debe capturar la senhal?\n");
    printf("Introduce un número mayor que 0. Para cerrar el programa, "
            "introduce -1\n");
    scanf("%d", &max_capturas);
    while (max_capturas <= 0){
        // Si el usuario ha seleccionado -1, finalizamos la ejecución
        if (max_capturas == -1) exit(EXIT_SUCCESS);
        printf("El número de capturas máximo debe ser mayor que 0.\n");
        printf("¿Cuántas veces se debe capturar la senhal?\n");
        printf("Introduce -1 si quieres salir del programa\n");
        scanf("%d", &max_capturas);
    }

    // Establecemos la nueva gestión que tendrá SIGALRM

    // Dirección de la función que se ejecutará al recibir SIGALRM
    nueva_accion.sa_handler = gestion;
    // Ninguna señal será bloqueada durante la ejecución del manejador de
    // SIGALRM. Por tanto, vaciamos su máscara.
    sigemptyset(&nueva_accion.sa_mask);
    // Usamos SA_RESTART para forzar el reiniciado de ciertas llamadas,
    // como la de printf(), cuando son interrumpidas por un gestor de señal
    nueva_accion.sa_flags = SA_RESTART;

    // Establecemos como nuevo comportamiento al recibir SIGALRM la
    // ejecución de la función gestion()
    if (sigaction(SIGALRM, &nueva_accion, NULL) == -1){
        perror("No se pudo establecer el gestor de SIGALRM");
        exit(EXIT_FAILURE);
    }

    alarm(1);       // Ponemos una alarma para dentro de 1 segundo
    calcular_potencias_2();     // Entramos en la generación de la secuencia

    // Dentro de calcular_potencias_2() se entra en un bucle infinito
    // No se vuelve al main()
}


/*
 * Función que calcula valores de la secuencia 2^n dentro de un bucle
 * infinito.
 */
void calcular_potencias_2(){
    unsigned long i = 0;             // Contador de iteraciones

    // El siguiente bucle solo será interrumpido para manejar señales.
    // Cuando se recibe SIGALRM, se salta a gestion(), para después continuar
    // con la ejecución del bucle.
    while (1){
        /*
         * Dado que por construcción de la secuencia, x corre el riesgo de
         * desbordarse muy rápidamente (generando resultados incorrectos),
         * realizamos una pausa cada 100 iteraciones. Es decir, calculamos
         * 100 nuevos elementos de la secuencia y después, esperamos hasta
         * recibir la llegada de SIGALRM.
         */
        if (i % 100 == 0){
            pause();
        }
        /*
         * 2^n se puede calcular como sumas de cada elemento consigo mismo.
         * Esto resulta más eficiente que calcular la secuencia directamente.
         * Para evitar comportamientos inesperados por overflow, trabajamos
         * en módulo ULONG_MAX. Nótese que cuando se alcanza este valor límite,
         * no debemos reiniciar x a 0, sino a 1, el primer valor de la
         * secuencia.
         */
        if (x >= ULONG_MAX - x) x = 1;  // Habría overflow con el próximo valor
        else x += x;     // No hay overflow (x + x < ULONG_MAX)

        /*
         * Aumentamos en 1 al contador y lo reiniciamos a 0 si alcanza el
         * máximo número representable para un unsigned long (aunque es
         * muy improbable por su elevada magnitud).
         */
        i = (i + 1) % ULONG_MAX;
    }
}


/*
 * Función manejadora para SIGALRM
 *
 * Tiene un único parámetro, el número de señal que provoca la ejecución de
 * la función. Se trata de un parámetro de entrada que en este programa
 * siempre será igual al número de SIGALRM, ya que es la única señal que
 * capturamos.
 */
static void gestion(int numero_de_senhal){
    time_t t;               // Guardará el resultado de time()
    struct tm tm;           // Tiempo en la zona horaria local

    // time() devuelve el número de segundos transcurridos desde el inicio del
    // 1 de enero de 1970.
    if ((t = time(NULL)) == -1){
        perror("Error al comprobar la hora actual");
        exit(EXIT_FAILURE);
    }
    tm = *localtime(&t);    // Pasamos t al formato de hora local

    // Imprimimos la hora actual junto al número de secuencia. Dado que las
    // alarmas van de 1 segundo en 1 segundo, no hace falta una precisión
    // mayor a la de los segundos.
    printf("Hora: %d:%d:%d. Número de secuencia: %ld\n",
            tm.tm_hour, tm.tm_min, tm.tm_sec, x);

    // Comparamos n_alarmas con max_capturas
    // Después, aumentamos n_alarmas en 1
    if (n_alarmas++ < max_capturas){
        alarm(1);           // Ponemos una nueva alarma
        printf("Alarma puesta\n");
    } else {
        printf("Se ha alcanzado el número de alarmas máximo (%d)\n",
                max_capturas);
        // La ejecución "normal" del programa finaliza en este exit()
        exit(EXIT_SUCCESS);
    }
    return;
}
