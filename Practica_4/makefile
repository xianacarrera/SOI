# Opciones de compilación, mostrando todos los warnings (-Wall)
CC = gcc -Wall
# Opciones de compilación para incluir la librería para hilos y la matemática
INCLUDE_PTH = -lpthread
INCLUDE_LM = -lm

# Ficheros fuente
SRCS_1 = ej1.c ej2.c ej3.c    # No requieren la librería matemática
SRCS_2 = ej4.c ej5.c          # Requieren la librería matemática

# Nombres de los ejecutables: nombres de los ficheros fuente sin extensión
OUTPUTS_1 = $(SRCS_1:.c=)
OUTPUTS_2 = $(SRCS_2:.c=)

# Archivos objeto (.o con un .c análogo como fichero fuente)
OBJS_1 = $(SRCS_1:.c=.o)
OBJS_2 = $(SRCS_2:.c=.o)

# Regla 1
# Creamos el ejecutable de cada programa
all: $(OUTPUTS_1) $(OUTPUTS_2)

# Regla 2
# Creamos el ejecutable de los programas principales que no requieren la librería matemática
$(OUTPUTS_1): %: %.o
	$(CC) -o $@ $< $(INCLUDE_PTH)
	
# Regla 3
# Creamos los ejecutables que sí requieren la librería matemática	
$(OUTPUTS_2): %: %.o 
	$(CC) -o $@ $< $(INCLUDE_PTH) $(INCLUDE_LM)

# Regla 4
# Creamos los archivos .o a partir de sus correspondientes .c
# $@ es el nombre del archivo que se está generando, $< es el primer prerrequisito
$(OBJS_1): %.o: %.c 
	$(CC) -c -o $@ $< $(INCLUDE_PTH)
$(OBJS_2): %.o: %.c 
	$(CC) -c -o $@ $< $(INCLUDE_PTH) $(INCLUDE_LM)

# Regla 4
# Borra los ejecutables y ejecuta clean dentro del directorio actual
cleanall: clean 
	rm -f $(OUTPUTS)

# Regla 5
# Borra todos los archivos .o utilizando el wildcard * (match con cualquier carácter)
# dentro del directorio actual
clean: 
	rm -f *.o
