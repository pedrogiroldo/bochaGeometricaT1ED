# Makefile atualizado para automatizar OBJETOS e dependências
PROJ_NAME = ted
LIBS = -lm
# Tenta find primeiro, se falhar usa wildcard
SRC_FILES := $(shell find src -name "*.c" 2>/dev/null)
ifeq ($(SRC_FILES),)
    SRC_FILES := $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)
endif
OBJETOS := $(SRC_FILES:.c=.o)       # Substitui .c por .o

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0

# Regra principal
$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Regra para compilar arquivos .c em .o com dependências
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@


# Target para limpeza
clean:
	rm -f $(OBJETOS) $(PROJ_NAME)

# Target para debug (mostra variáveis)
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "OBJETOS: $(OBJETOS)"

run:
	./$(PROJ_NAME) -f src/test/test.geo -o src/test/results -q src/test/test.qry