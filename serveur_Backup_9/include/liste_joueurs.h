#ifndef LISTE_JOUEURS_H
#define LISTE_JOUEURS_H

#include <stdlib.h>
#include <stdio.h>

#include <types_jeu.h>

void initListeJoueurs(ListeJoueurs *liste);
int nbJoueurListe(ListeJoueurs *l);
int nbJoueurActifListe(ListeJoueurs *l);
void addJoueurListe(ListeJoueurs *l, Joueur *j);
int suppJoueurListe(ListeJoueurs *l, Joueur *j);

#endif
