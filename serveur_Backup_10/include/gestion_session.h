#ifndef GESTION_SESSION_H
#define GESTION_SESSION_H

#include <types_jeu.h>

void *gestionSession(void *arg);

void phaseResolution(Session *s, Joueur *jActif);
Joueur *terminerEnchere(Session *s);
Joueur *getNewJoueurActif(Session *s);
int joueurAScoreObjectif(Session *s);
void reinit(Session *s);

void setEnchereToInit(Session *s);
void initEnchere(Session *s);
void beActif(ListeJoueurs *l);



#endif
