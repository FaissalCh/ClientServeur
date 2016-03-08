#ifndef Liste_Joueurs_H
#define Liste_Joueurs_H

#include <arpa/inet.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//#include <serveur.h>


#define T_PSEUDO 124 /* Taille du buffer de pseudo */


/* Structure qui definie un joueur */
typedef struct _Joueur {
  pthread_mutex_t mutex;
  char pseudo[T_PSEUDO];
  int socket;
  struct _Joueur *next;
} Joueur;

/* Liste des joueurs */
typedef struct _ListeJoueurs {
  Joueur *j;
  int nbJoueur;
  pthread_mutex_t mutex;
} ListeJoueurs;


void initListeJoueurs(ListeJoueurs *liste);


int suppJoueurListe(ListeJoueurs *l, Joueur *j);
void addJoueurListe(ListeJoueurs *l, Joueur *j);
Joueur *getJoueurListe(ListeJoueurs *l, char *pseudo);
int nbJoueurListe(ListeJoueurs *l);

Joueur *create_joueur(char *pseudo, int socket);
void detruire_joueur(Joueur *j);
char *pseudoJoueur(Joueur *j);

#endif 
