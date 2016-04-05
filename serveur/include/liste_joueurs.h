#ifndef LISTE_JOUEURS_H
#define LISTE_JOUEURS_H

#include <stdlib.h>
#include <stdio.h>

#include <types_jeu.h>

/* Fonction sur les joueurs */
Joueur *create_joueur(char *pseudo, int socket);
void detruire_joueur(Joueur *j);
char *pseudoJoueur(Joueur *j);

int pseudo_deja_present(ListeJoueurs *l, char *pseudo);
void initListeJoueurs(ListeJoueurs *liste);
int nbJoueurListe(ListeJoueurs *l);
int nbJoueurActifListe(ListeJoueurs *l);
void addJoueurListe(ListeJoueurs *l, Joueur *j);
int suppJoueurListe(ListeJoueurs *l, Joueur *j);

#endif
