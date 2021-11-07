# Opciones de compilación, mostrando todos los warnings (-Wall)
CC = gcc -Wall

# Ficheros fuente
SRCS = ejercicio_4.c ejercicio_7.c ejercicio_8.c

# Nombres de los ejecutables: nombres de los ficheros fuente sin extensión
OUTPUTS = $(SRCS_1:.c=)

# Archivos objeto (.o con un .c análogo como fichero fuente)
OBJS = $(SRCS_1:.c=.o)

# Regla 1
# Creamos el ejecutable de cada programa
all: $(OUTPUTS)

# Regla 2
# Creamos el ejecutable de los programas principales
$(OUTPUTS): $(OBJS) 
	$(CC) -o $(OUTPUTS) $(OBJS)

# Regla 3
# Creamos los archivos .o a partir de sus correspondientes .c
# $@ es el nombre del archivo que se está generando, $< es el primer prerrequisito
%.o: %.c 
	$(CC) -c -o $@ $<

# Regla 4
# Borra los ejecutables y ejecuta clean dentro del directorio actual
cleanall: clean 
	rm -f $(OUTPUTS)

# Regla 5
# Borra todos los archivos .o utilizando el wildcard * (match con cualquier carácter)
# dentro del directorio actual
clean: 
	rm -f $(OUTPUTS) *.o
