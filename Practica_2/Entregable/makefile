# Opciones de compilación, mostrando todos los warnings (-Wall)
CC = gcc -Wall
# Opción de compilación para incluir la librería matemática
INCLUDE = -lm

# Ficheros fuente
SRCS_1 = ejercicio_7.c
SRCS_2 = quinto_hijo.c

# Nombres de los ejecutables del programa principal y del programa del quinto hijo: 
# nombres de los ficheros fuente sin extensión
OUTPUT_1 = $(SRCS_1:.c=)
OUTPUT_2 = $(SRCS_2:.c=)

# Archivos objeto (.o con un .c análogo como fichero fuente)
OBJS_1 = $(SRCS_1:.c=.o)
OBJS_2 = $(SRCS_2:.c=.o)


# Regla 1
# Creamos el ejecutable de cada programa
all: $(OUTPUT_1) $(OUTPUT_2)

# Regla 2
# Creamos el ejecutable del programa principal incluyendo la librería matemática
# $@ es el nombre del archivo que se está generando, $< es el primer prerrequisito
$(OUTPUT_1): $(OBJS_1) 
	$(CC) -o $@ $< $(INCLUDE)

# Regla 3
# Creamos el ejecutable del programa del quinto hijo
$(OUTPUT_2): $(OBJS_2) 
	$(CC) -o $@ $<

# Regla 3
# Creamos el archivo .o de ejercicio_7 a partir de su correspondiente .c
# Si ejercicio_7.c cambia, solo ejercicio_7.o se recompilará. quinto_hijo.o no se ve afectado
$(OBJS_1): $(SRCS_1) 
	$(CC) -c -o $@ $< $(INCLUDE)

# Regla 4
# Análogo a la regla 3, pero para quinto_hijo
$(OBJS_2): $(SRCS_2) 
	$(CC) -c -o $@ $<

# Regla 5
# Borra los ejecutables y ejecuta clean dentro del directorio actual
cleanall: clean 
	rm -f $(OUTPUT)

# Regla 6
# Borra todos los archivos .o utilizando el wildcard * (match con cualquier carácter)
# dentro del directorio actual
clean: 
	rm -f $(OUTPUT) *.o
