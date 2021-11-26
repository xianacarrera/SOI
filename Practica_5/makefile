# Opciones de compilación, mostrando todos los warnings (-Wall)
CC = gcc -Wall
# Opciones de compilación para incluir la librería matemática
INCLUDE_LM = -lm
STATIC = -static
INCLUDE_LPTHREAD = -lpthread

# Ficheros fuente
SRCS_1 = ej1.c ej2.c ej3.c ej4.c hijo4.c
SRCS_2 = ej5.c
SRCS_3 = ej6.c

# Nombres de los ejecutables: nombres de los ficheros fuente sin extensión
OUTPUTS_1 = $(SRCS_1:.c=)
OUTPUTS_2 = $(SRCS_2:.c=)
OUTPUTS_3 = $(SRCS_2:.c=_static)     # Compilación con -lm estática
OUTPUTS_4 = $(SRCS_3:.c=)

# Archivos objeto (.o con un .c análogo como fichero fuente)
OBJS_1 = $(SRCS_1:.c=.o)
OBJS_2 = $(SRCS_2:.c=.o)
OBJS_3 = $(SRCS_3:.c=.o)

# Regla 1
# Creamos el ejecutable de cada programa
all: $(OUTPUTS_1) $(OUTPUTS_2) $(OUTPUTS_3) $(OUTPUTS_4)

# Regla 2
# Creamos el ejecutable de los programas principales
$(OUTPUTS_1): %: %.o
	$(CC) -o $@ $<
	
# Regla 2
# Creamos el ejecutable de los programas principales
$(OUTPUTS_2): %: %.o
	$(CC) -o $@ $< $(INCLUDE_LM)
	

$(OUTPUTS_3): %: $(OBJS_2)
	$(CC) -o $@ $< $(STATIC) $(INCLUDE_LM)
	

$(OUTPUTS_4): %: $(OBJS_3)
	$(CC) -o $@ $< $(INCLUDE_LPTHREAD)

# Regla 3
# Creamos los archivos .o a partir de sus correspondientes .c
# $@ es el nombre del archivo que se está generando, $< es el primer prerrequisito
%.o: %.c 
	$(CC) -c -o $@ $<


# Regla 4
# Borra los ejecutables y ejecuta clean dentro del directorio actual
cleanall: clean 
	rm -f all

# Regla 5
# Borra todos los archivos .o utilizando el wildcard * (match con cualquier carácter)
# dentro del directorio actual
clean: 
	rm -f *.o
