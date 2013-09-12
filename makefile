.PHONY: install
.PHONY: test
.PHONY: doc

SRC=$(wildcard mydir/*.c)
OBJ=$(patsubst %.c,%,$(SRC))

install: $(OBJ) shell

$(OBJ): $(SRC)
	gcc -o $(OBJ) $(SRC)
shell: shell.c
	gcc -o shell shell.c

test:
	./shell test.txt

doc:
	pdflatex doc.tex


