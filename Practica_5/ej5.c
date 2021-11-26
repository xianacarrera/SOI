#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int main(){
    char llamada[50];

    snprintf(llamada, 50, "cat /proc/%d%s", getpid(), "/maps");
    system(llamada);
    
    printf("\n\n");
    printf("cos(3.141592) = %f\n", cos(3.141592));
    exit(EXIT_SUCCESS);
}
