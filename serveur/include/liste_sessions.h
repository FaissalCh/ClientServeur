#ifndef LISTE_SESSIONS_H
#define LISTE_SESSIONS_H

#include <stdlib.h>
#include <stdio.h>

#include <types_jeu.h>

int session_deja_present(ListeSession *l, char *nom);

void initListeSession(ListeSession *liste);
void addSessionListe(ListeSession *l, Session *s);
int suppSessionListe(ListeSession *l, Session *s);
Session *getSession(ListeSession *l, char *name);

Session *createSession(char *nomSession, char *mdp);
void destroy_session(Session *s);

#endif
