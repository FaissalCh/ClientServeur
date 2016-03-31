#ifndef Fonction_Jeu_H
#define Fonction_Jeu_H

#include <types_jeu.h>

char *getBilanSession(Session *s, int withLock);

int solutionAccepte(char *sol, Session *s, Joueur *myJoueur); 
void deplacement(Plateau *p, Deplacement *d, Robot *r);
int isObstacle(Plateau *p, int x, int y, Direction d);

int getNbMurs(char *murs);
Robot *getRobot(Robot *robots, Couleur col);
Couleur getCol(char c);
Direction getDir(char d);

Deplacements *getDeplacements(char *sol);
Plateau *getPlateau(int nb);
Cible getCible(char *c);
Mur *getMurs(char *c, int *nb);
Robot *getRobots(char *c);
void setCible(char *c, Cible *cible);

char *enigmeToString(Enigme *e);
char *mursToString(Mur *murs, int nbMurs);
Couleur numToCol(int i);
char directionToChar(Direction d);
char colToChar(Couleur c);

char *nextVirgule(char *c);

#endif 
