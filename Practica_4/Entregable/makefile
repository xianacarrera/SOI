# Opciones de compilación, mostrando todos los warnings (-Wall)
CC = gcc -Wall
# Opciones de compilación para incluir la librería para hilos y la matemática
INCLUDE_PTH = -lpthread
INCLUDE_LM = -lm

# Fichero fuente
SRCS = ejercicio_5.c

# Nombre del ejecutable: nombres del fichero fuente sin extensión
OUTPUTS = $(SRCS:.c=)

# Archivo objeto (.o con un .c análogo como fichero fuente)
OBJS = $(SRCS:.c=.o)

# Regla 1
# Creamos el ejecutable del programa principal incluyendo ambas librerías
$(OUTPUTS): %: %.o
	$(CC) -o $@ $< $(INCLUDE_PTH) $(INCLUDE_LM)

# Regla 2
# Creamos los archivos .o a partir de sus correspondientes .c
# $@ es el nombre del archivo que se está generando, $< es el primer prerrequisito
$(OBJS): %.o: %.c 
	$(CC) -o $@ -c $<

# Regla 3
# Borra el ejecutable y ejecuta clean dentro del directorio actual
cleanall: clean 
	rm -f $(OUTPUTS)

# Regla 4
# Borra todos los archivos .o utilizando el wildcard * (match con cualquier carácter)
# dentro del directorio actual
clean: 
	rm -f *.o
