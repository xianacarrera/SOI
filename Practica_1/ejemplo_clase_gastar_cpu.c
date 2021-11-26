#include <stdio.h>

int main(){
    int i, j, N = 8;
    volatile double x = 1.0001;
    // Volatile le indica al compilador que no optimice nada que tenga
    // que ver con la variable x

    for (i = 0; i < N; i++){
        for (j = 0; j < 1000000000; j++){
            x*=(1/1.0000001);
        }
    }

    printf("%e\n", x);

    /*
     * Es importante poner un printf. De lo contrario, el compilador
     * detecta que los resultados de los cálculos no se están utilizando
     * y se eliminan. Nótese que la optimización no ocurre por defecto, pero
     * podemos aplicarla en gcc activando el nivel 3:
     *                        gcc -03 -o archivo archivo.c.
     * Una opción de interés con efecto opuesto es marcar las variables
     * afectadas con el atributo volatile, que evita las optimizaciones.
     *
     * También debemos controlar el número de iteraciones, para
     * hacer un seguimiento de la ejecución.
     */

    return 0;
}
