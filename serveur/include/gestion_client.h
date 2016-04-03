#ifndef GESTION_CLIENT_H
#define GESTION_CLIENT_H

void *gestionClient(void *argThread);

char *nbCoupsDejaPropose(int nbCoups, Joueur *myJoueur, Session *s);
void indiquerConnexion(Session *session, Joueur *myJoueur);

void resolution(Session *s, Joueur *myJoueur);
void enchere(Session *s, Joueur *myJoueur);
Joueur *connex(int sock, ListeJoueurs *liste);
void sort(ListeJoueurs *liste, Joueur *myJoueur);
void trouve();
void chat(Session *s, Joueur *myJoueur);
Session *creerSession(ListeSession *l, Joueur *myJoueur);

#endif
