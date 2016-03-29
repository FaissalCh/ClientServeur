CC=gcc -Wall
BIN=bin
INC=include
OBJ=obj
SRC=src


all: ${BIN}/test ${BIN}/serveur

# ------------ OBJETS  ------------ #
${OBJ}/%.o: ${SRC}/%.c
	@$(CC) -o $@ -c $< -I${INC}

# ------------ EXECUTABLE  ------------ #
${BIN}/serveur: ${OBJ}/serveur.o ${OBJ}/timer.o ${OBJ}/gestion_client.o ${OBJ}/gestion_session.o ${OBJ}/tools.o ${OBJ}/fonctions_jeu.o ${OBJ}/liste_joueurs.o ${OBJ}/liste_sessions.o
	@$(CC) -o $@ $^ -lpthread

${BIN}/test: ${OBJ}/test.o ${OBJ}/timer.o ${OBJ}/gestion_client.o ${OBJ}/gestion_session.o ${OBJ}/tools.o ${OBJ}/fonctions_jeu.o ${OBJ}/liste_joueurs.o
	@$(CC) -o $@ $^ -lpthread

# ------------ OTHERS ------------ #
clean:
	rm -f ${OBJ}/* ${BIN}/* 
	@(cd src/;rftmp;cd ../include;rftmp)
cleanall:
	rm -rf ${OBJ} ${BIN} 
	rm -f ${INC}/*~ ${SRC}/*~ *~
