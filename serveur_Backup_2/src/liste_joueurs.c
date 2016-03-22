#include <liste_joueurs.h>

#include <pthread.h>


void initListeJoueurs(ListeJoueurs *liste) {
  liste->j = NULL;
  liste->nbJoueur = 0;
  pthread_mutex_init(&(liste->mutex), NULL);
}

int nbJoueurListe(ListeJoueurs *l) {
  int res;
  pthread_mutex_lock(&(l->mutex));
  res = l->nbJoueur;
  pthread_mutex_unlock(&(l->mutex));
  return res;
}

int nbJoueurActifListe(ListeJoueurs *l) {
  int res = 0;
  pthread_mutex_lock(&(l->mutex));
  Joueur *cur = l->j;
  while(cur != NULL) {
    if(cur->playSessionEnCours)
      res++;
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return res;
}


void addJoueurListe(ListeJoueurs *l, Joueur *j) {
  pthread_mutex_lock(&(l->mutex));
  if(l->nbJoueur == 0) {
    l->j = j;
  }
  else {
    j->next = l->j;
    l->j = j;
  }
  l->nbJoueur++;
  pthread_mutex_unlock(&(l->mutex));
}

int suppJoueurListe(ListeJoueurs *l, Joueur *j) {
  Joueur *cur, *prec;
  int ret = 0;
  pthread_mutex_lock(&(l->mutex));

  prec = NULL;
  cur = l->j;
  while(cur != NULL) {
    if(cur == j) {
      if(prec == NULL) // 1er de la liste
	l->j = j->next;
      else 
	prec->next = j->next;
      l->nbJoueur--;
      ret = 1;
      break;
    }
    prec = cur;
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return ret;
}
