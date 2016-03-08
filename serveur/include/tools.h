#ifndef Liste_Joueurs_H
#define Liste_Joueurs_H

#include <arpa/inet.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <types_jeu.h>


/* Fonction de gestion de section */
Session *createSession(char *nomSession, char *mdp);
ArgThread *createArgThread(int socket, Session *session);

/* Fonction sur le protocole */
int hash_protocole(char *req);


/* Fonction sur la liste chainee */
void initListeJoueurs(ListeJoueurs *liste);
void addJoueurListe(ListeJoueurs *l, Joueur *j);
int suppJoueurListe(ListeJoueurs *l, Joueur *j);
Joueur *getJoueurListe(ListeJoueurs *l, char *pseudo);
int nbJoueurListe(ListeJoueurs *l);


/* Fonction sur les joueurs */
Joueur *create_joueur(char *pseudo, int socket);
void detruire_joueur(Joueur *j);
char *pseudoJoueur(Joueur *j);


/* Fonction sur les socket */
int getSocketServeur(int port);
void *gestionClient(void *arg);
void *gestionSession(void *arg);
void sendTo(char *buf, ListeJoueurs *liste, Joueur *j);
void sendToAll(char *buf, ListeJoueurs *liste, Joueur *saufMoi);

#endif 
