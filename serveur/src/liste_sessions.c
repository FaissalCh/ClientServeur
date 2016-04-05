#include <liste_sessions.h>
#include <liste_joueurs.h>
#include <fonctions_jeu.h>
#include <pthread.h>

Session *createSession(char *nomSession, char *mdp) {
  // Pas de gestion de l'id pour l'instant
  Session *s = (Session *)malloc(sizeof(Session));
  if(s == NULL) {
    perror("malloc");
    exit(1);
  }
  s->liste = (ListeJoueurs *)malloc(sizeof(ListeJoueurs));
  if(s->liste == NULL) {
    perror("malloc");
    exit(1);
  }
  pthread_mutex_init(&(s->mutex), NULL);
  if(pthread_cond_init(&(s->condConnexion), NULL)) {
    perror("pthread_cond_init");
    exit(1);
  }
  if(pthread_cond_init(&(s->condFinReflexion), NULL)) {
    perror("pthread_cond_init");
    exit(1); 
  }
  if(pthread_cond_init(&(s->condFinResolution), NULL)) {
    perror("pthread_cond_init");
    exit(1); 
  }

  strncpy(s->nomSession, nomSession, T_PSEUDO);
  strncpy(s->mdp, mdp, T_PSEUDO);
  initListeJoueurs(s->liste);
  s->nbTour = 0;
  //s->tourEnCours = 0;

  s->p = getPlateau( (rand() % NB_PLATEAU) + 1);

  return s;
}



void destroy_session(Session *s) {
  printf("[Suppression de la session] %s\n", s->nomSession);
  free(s->liste);
  pthread_mutex_destroy(&(s->mutex));
  pthread_cond_destroy(&(s->condConnexion));
  pthread_cond_destroy(&(s->condFinReflexion));
  pthread_cond_destroy(&(s->condFinResolution));
  free(s);
}


Session *getSession(ListeSession *l, char *name) {
  Session *cur;
  if(name == NULL)
    return NULL;

  pthread_mutex_lock(&(l->mutex));
  cur = l->s;
  while(cur != NULL) {
    if(!strcmp(name, cur->nomSession)) {
      pthread_mutex_unlock(&(l->mutex));
      return cur;
    }
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return NULL;
}


void initListeSession(ListeSession *liste) {
  liste->s = NULL;
  liste->nbSession = 0;
  pthread_mutex_init(&(liste->mutex), NULL);
}

void addSessionListe(ListeSession *l, Session *s) {
  pthread_mutex_lock(&(l->mutex));
  if(l->nbSession == 0) {
    l->s = s;
  }
  else {
    s->next = l->s;
    l->s = s;
  }
  l->nbSession++;
  pthread_mutex_unlock(&(l->mutex));
}

int suppSessionListe(ListeSession *l, Session *s) {
  Session *cur, *prec;
  int ret = 0;
  pthread_mutex_lock(&(l->mutex));

  prec = NULL;
  cur = l->s;
  while(cur != NULL) {
    if(cur == s) {
      if(prec == NULL) // 1er de la liste
	l->s = s->next;
      else 
	prec->next = s->next;
      l->nbSession--;
      ret = 1;
      break;
    }
    prec = cur;
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return ret;
}


// Prendre lock sur liste avant
int session_deja_present(ListeSession *l, char *nom) {
  Session *cur = l->s;
  while(cur != NULL) {
    if(!strcmp(nom, cur->nomSession)) 
      return 1;
    cur = cur->next;
  }
  return 0;
}
