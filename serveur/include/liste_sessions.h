#ifndef LISTE_SESSIONS_H
#define LISTE_SESSIONS_H

#include <stdlib.h>
#include <stdio.h>

#include <types_jeu.h>

void initListeSession(ListeSession *liste);
void addSessionListe(ListeSession *l, Session *s);
int suppSessionListe(ListeSession *l, Session *s);

#endif
