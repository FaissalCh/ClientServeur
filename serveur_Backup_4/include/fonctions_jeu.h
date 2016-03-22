#ifndef Fonction_Jeu_H
#define Fonction_Jeu_H

char *getBilanSession(Session *s);
int solutionAccepte(char *sol, Session *s, Joueur *myJoueur); 
int isObstacle(Plateau *p, int x, int y, Direction d);
Deplacements *getDeplacements(char *sol);


Couleur getCol(char c);
Couleur getDir(char d);


#endif 
