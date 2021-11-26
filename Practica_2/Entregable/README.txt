Xiana Carrera Alonso
Sistemas Operativos I - Curso 2021/2022
Práctica 2 - Ejercicio 7

                                Compilación
Se ha incluido un makefile que genera los ejecutables del programa principal y del programa del
quinto hijo como opción por defecto, "make". Se incluye también una opción para eliminar los .o
residuales, utilizando "make clean". Para eliminar tanto los .o como los ejecutables, se puede 
usar "make cleanall".

Si se prefiere realizar la compilación manualmente, debe tenerse en cuenta que el nombre del 
ejecutable del programa del quinto hijo ha de ser "quinto_hijo". Por ejemplo, se puede realizar
gcc -o quinto_hijo quinto_hijo.c

La compilación del programa principal no tiene ningún requisito particular, con la salvedad de
que se debe incluir la librería matemática con la opción -lm. Ejemplo:
gcc -o ejercicio_7 ejercicio_7.c -lm


                                Notas sobre el programa
 * No se ha considerado el 0 como número natural (no influiría en el cálculo salvo en la 
   elección del límite superior como 49.999.999 en lugar de 50.000.000).
 * Se ha dejado 50.000.000 como el total de naturales a considerar, pero se recomienda variar
   dicho valor (es una constante definida antes del main) en función de las capacidades del
   ordenador, con el objetivo de que el programa tarde entre 10 y 20 segundos.
   
   
                                Comentarios sobre la práctica
Los resultados obtenidos muestran una pequeña aunque perceptible diferencia entre los métodos
seguidos para llevar a cabo el cálculo. El motivo subyace en que las operaciones en punto
flotante del ordenador no son asociativas, sino que debido a las limitaciones de la precisión
finita, el orden en el que se realizan las operaciones es significativo de cara al resultado. 
Además, el paso decimal-binario y binario-decimal no siempre es exacto, lo que constituye
otra fuente de errores. 

Estas peculiaridades se traducen en la diferencia apreciable cuando se utiliza un número 
de decimales alto. Nótese que, cuando se reduce la precisión (por ejemplo, mostrando un 
menor número de decimales), puede aparentar que los resultados son iguales, pero esto se debe
a que los errores acumulados son mínimos, de forma que si se admite un margen de error alto,
no veremos discrepancias.

Si probamos a aumentar el número de naturales, veremos que la diferencia tiende también
a incrementarse, lo cual es coherente con nuestra hipótesis: dado que se van a tener que
realizar más sumas, se notará más el efecto de la falta de asociatividad.

Como valoración personal, lo que más he disfrutado ha sido experimentar con el uso de los 
fork() y con el cambio de imagen a través de la familia exec. La práctica me ha permitido
comprender mejor el funcionamiento de estas últimas, así como pulir mi código de cara a un
control más eficiente del flujo de ejecución con procesos padre e hijos coexistententes.

Creo que el programa permite entender a un nivel bastante profundo el manejo de procesos
por parte del sistema operativo. Además, me ha hecho prestar más atención a la localización
de posibles situaciones de error, ya que se utilizan numerosas funciones que pueden dar 
errores "inesperados", y en consecuencia he visto prudente tratar de que el programa las
reconociera para no fallar inesperadamente, de modo que he tenido que asegurarme de 
comprender bien el funcionamiento del código. 

En conclusión, considero que ha resultado un ejercicio ciertamente provechoso de cara a
la comprensión de la asignatura por parte del alumno.
