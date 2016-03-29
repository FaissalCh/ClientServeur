#ifndef Fonction_Jeu_H
#define Fonction_Jeu_H

#include <types_jeu.h>

char *getBilanSession(Session *s);
int solutionAccepte(char *sol, Session *s, Joueur *myJoueur); 
int isObstacle(Plateau *p, int x, int y, Direction d);
Deplacements *getDeplacements(char *sol);

Robot *getRobot(Robot *robots, Couleur col);
Couleur getCol(char c);
Direction getDir(char d);

void deplacement(Plateau *p, Deplacement *d, Robot *r);

Plateau *getPlateau(int nb);

int getNbMurs(char *murs);
Mur *getMurs(char *c, int *nb);
void setCible(char *c, Cible *cible);
Robot *getRobots(char *c);

char *nextVirgule(char *c);
Couleur numToCol(int i);
char directionToChar(Direction d);
Cible getCible(char *c);
char colToChar(Couleur c);
char *mursToString(Mur *murs, int nbMurs);
char *enigmeToString(Enigme *e);


#endif 
