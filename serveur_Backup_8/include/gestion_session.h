#ifndef GESTION_SESSION_H
#define GESTION_SESSION_H

#include <types_jeu.h>


void *gestionSession(void *arg);
void setEnchereToInit(Session *s);
void initEnchere(Session *s);
Joueur *terminerEnchere(Session *s);
void beActif(ListeJoueurs *l);


#endif
