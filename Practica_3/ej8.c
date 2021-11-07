#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

// SA_ONSTACK??
static void gestion(int numero_de_senhal);
unsigned long calcular_potencias_2();

// Al manejador de senhales solo le podemos pasar un argumento, el número
// de senhal, de modo que tendremos que declarar la variable que almacenará
// los valores de la secuencia como global para que el manejador tenga acceso.
unsigned long x = 1;
long y = 5;
int max_capturas;
int n_alarmas = 1;

int main(){
    struct sigaction nueva_accion;

    printf("¿Cuántas veces se debe capturar la senhal?\n");
    scanf("%d", &max_capturas);
    while (max_capturas <= 0){
        printf("El número de capturas máximo debe ser mayor que 0.\n");
        printf("¿Cuántas veces se debe capturar la senhal?\n");
        scanf("%d\n", &max_capturas);
    }

/*
    printf("¿Quieres seleccionar un límite superior para la secuencia?\n");
*/;

    // Dirección de la función a ejecutar al recibir SIGALRM
    nueva_accion.sa_handler = gestion;
    // Necesario vaciar la máscara de senhales???
    sigemptyset(&nueva_accion.sa_mask);
    // El flag SA_RESTART fuerza el reiniciado de ciertas llamadas del sistema
    // cuando son interrumpidas por un gestor de señal
    // ??
    nueva_accion.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &nueva_accion, NULL) == -1){
        perror("No se pudo establecer el gestor de SIGALRM");
        exit(EXIT_FAILURE);
    }

    alarm(1);
    while (1){
        x = x + x;
                if (x >= INT_MAX - x) printf("Overflow\n");

    }
    //calcular_potencias_2();
}

unsigned long calcular_potencias_2(){
    while (1){
        x += x;

    }
}

static void gestion(int numero_de_senhal){
    // No hace falta comprobar que la senhal sea SIGALRM porque es la única
    // que capturamos???
    /*
    time_t t;
    struct tm tm;

    if ((t = time(NULL)) == -1){
        perror("Error al comprobar la hora actual");
        exit(EXIT_FAILURE);
    }
    tm = *localtime(&t);*/

    printf("%lu\n", x);
    //printf("Hora: %d:%d:%d. Número de secuencia: %ld\n",
            //tm.tm_hour, tm.tm_min, tm.tm_sec, x);

    if (n_alarmas < max_capturas){
        alarm(1);
        printf("Alarma puesta\n");
        n_alarmas++;  // Poner en el if
    } else {
        printf("Se ha alcanzado el número de alarmas máximo (%d)\n",
                max_capturas);
        exit(EXIT_SUCCESS);
    }
    return;
}
