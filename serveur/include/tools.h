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

void affPlateau(Plateau *p);

/* Fonction de gestion de section */
ArgThread *createArgThreadClient(int socket, ListeSession *sessions);
/* Fonction sur le protocole */
int hash_protocole(char *req);



/* Fonction sur les socket */
int getSocketServeur(int port);
void sendTo(char *buf, ListeJoueurs *liste, Joueur *j, int withLock);
void sendToAll(char *buf, ListeJoueurs *liste, Joueur *saufMoi, int withLock);
void sendToAllActif(char *buf, ListeJoueurs *liste, Joueur *saufMoi, int withLock);
  
#endif 
