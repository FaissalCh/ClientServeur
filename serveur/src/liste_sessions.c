#include <liste_sessions.h>

#include <pthread.h>

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
