# Makefile atualizado para automatizar OBJETOS e dependências
PROJ_NAME = ted
LIBS =
SRC_FILES := $(wildcard src/**/*.c)  # Busca recursiva por arquivos .c
OBJETOS := $(SRC_FILES:.c=.o)       # Substitui .c por .o

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration -MMD
LDFLAGS = -O0

# Regra principal
$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Regra para compilar arquivos .c em .o com dependências
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Incluir dependências geradas automaticamente
-include $(OBJETOS:.o=.d)