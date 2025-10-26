# 🎯 Bocha Geométrica T1ED

### Pedro Andrade Giroldo 202500560625

Um processador de comandos geométricos desenvolvido em C99 que processa arquivos `.geo` e `.qry` para gerar visualizações SVG.

## 🚀 Como Executar

### 1. Compilar o Projeto

```bash
make
```

### 2. Executar o Programa

```bash
./ted -f <arquivo.geo> -o <diretorio_saida> [-q <arquivo.qry>] [sufixo]
```

#### Parâmetros Obrigatórios:

- `-f <arquivo.geo>`: Arquivo de entrada com comandos geométricos
- `-o <diretorio_saida>`: Diretório onde serão salvos os arquivos de saída

#### Parâmetros Opcionais:

- `-q <arquivo.qry>`: Arquivo de consultas (opcional)
- `sufixo`: Sufixo para os arquivos de saída (opcional)

## 📁 Exemplos de Uso

### Exemplo Básico (apenas arquivo .geo):

```bash
./ted -f test_files/geo/teste.geo -o output
```

### Exemplo Completo (com arquivo .qry):

```bash
./ted -f test_files/geo/complex.geo -o output -q test_files/qry/complex.qry
```

### Exemplo com Sufixo:

```bash
./ted -f test_files/geo/retg-cres.geo -o output -q test_files/qry/dsp-cruz-alt.qry sufixo
```

## 🗂️ Estrutura do Projeto

```
├── src/                    # Código fonte
│   ├── lib/             # Bibliotecas do projeto
│   └── main.c          # Arquivo principal
├── test_files/          # Arquivos de teste
│   ├── geo/            # Arquivos .geo de exemplo
│   └── qry/            # Arquivos .qry de exemplo
├── output/             # Diretório de saída (gerado automaticamente)
└── Makefile           # Arquivo de compilação
```

## 🧹 Limpeza

Para limpar os arquivos compilados:

```bash
make clean
```

## 📋 Requisitos

- Compilador GCC com suporte ao padrão C99
- Sistema operacional Linux/Unix

## 🎨 Saída

O programa gera arquivos SVG no diretório de saída especificado, contendo as visualizações geométricas processadas.

---

_Desenvolvido seguindo rigorosamente o padrão C99 sem extensões GNU._
