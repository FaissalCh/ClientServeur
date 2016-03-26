#ifndef Fonction_Jeu_H
#define Fonction_Jeu_H

#include <types_jeu.h>

char *getBilanSession(Session *s);
int solutionAccepte(char *sol, Session *s, Joueur *myJoueur); 
int isObstacle(Plateau *p, int x, int y, Direction d);
Deplacements *getDeplacements(char *sol);

Robot getRobot(Plateau *plateau, Couleur col);
Couleur getCol(char c);
Direction getDir(char d);

void deplacement(Plateau *p, Deplacement *d, Robot *r);

Plateau *getPlateau(int nb);

int getNbMurs(char *murs);
Mur *getMurs(char *c, int *nb);
void setCible(char *c, Cible *cible);
Robot *getRobots(char *c);

#endif 
