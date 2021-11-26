#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(){
    int i;
    double a, b, c;

    for (i = 0; i < 1000000000; i++){
        a = (double) rand() / RAND_MAX;
        b = (double) rand() / RAND_MAX;
        c = 500*a - 125.15*b + 1.2597864;
    }

    return(EXIT_SUCCESS);
}
