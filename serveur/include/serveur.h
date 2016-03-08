#ifndef SERVEUR_H
#define SERVEUR_H

#include <arpa/inet.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <string.h>

#include <liste_joueurs.h>


#define PORT 2016

int getSocketServeur(int port);
void *gestionClient(void *arg);
void sendTo(char *buf, Joueur *j);
void sendToAll(char *buf, ListeJoueurs *liste, Joueur *saufMoi);

void connex(int sock, ListeJoueurs *liste);


#endif 
