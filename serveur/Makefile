CC=gcc -Wall
BIN=bin
INC=include
OBJ=obj
SRC=src


all: ${BIN}/serveur

# ------------ OBJETS  ------------ #
${OBJ}/%.o: ${SRC}/%.c
	@$(CC) -o $@ -c $< -I${INC}

# ------------ EXECUTABLE  ------------ #
${BIN}/serveur: ${OBJ}/main.o ${OBJ}/serveur.o  ${OBJ}/liste_joueurs.o
	@$(CC) -o $@ $^ -lpthread


# ------------ OTHERS ------------ #
clean:
	rm -f ${OBJ}/* ${BIN}/* 

cleanall:
	rm -rf ${OBJ} ${BIN} 
	rm -f ${INC}/*~ ${SRC}/*~ *~
