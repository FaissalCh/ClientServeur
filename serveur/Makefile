CC=gcc -Wall
BIN=bin
INC=include
OBJ=obj
SRC=src


all: ${BIN}/serveur


$
# ------------ OBJETS  ------------ #
${OBJ}/%.o: ${SRC}/%.c
	@if [ -d $(OBJ) ]; then : ; else mkdir $(OBJ); fi
	@$(CC) -o $@ -c $< -I${INC}

# ------------ EXECUTABLE  ------------ #
${BIN}/serveur: ${OBJ}/serveur.o ${OBJ}/timer.o ${OBJ}/gestion_client.o ${OBJ}/gestion_session.o ${OBJ}/tools.o ${OBJ}/fonctions_jeu.o ${OBJ}/liste_joueurs.o ${OBJ}/liste_sessions.o
	@if [ -d $(BIN) ]; then : ; else mkdir $(BIN); fi
	@$(CC) -o $@ $^ -lpthread

# ------------ OTHERS ------------ #
clean:
	rm -f ${OBJ}/* ${BIN}/* 

# cleanall:
# 	rm -rf ${OBJ} ${BIN} 
# 	rm -f ${INC}/*~ ${SRC}/*~ *~
