#include <liste_joueurs.h>

#include <pthread.h>
#include <unistd.h>


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

  printf("Fonc supp\n");
  prec = NULL;
  cur = l->j;
  while(cur != NULL) {
    if(cur == j) {
      if(prec == NULL) // 1er de la liste
	l->j = j->next;
      else 
	prec->next = j->next;
      l->nbJoueur--;
      printf("SUP\n");
      ret = 1;
      break;
    }
    prec = cur;
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return ret;
}


Joueur *create_joueur(char *pseudo, int socket) {
  Joueur *j_tmp = (Joueur *)malloc(sizeof(Joueur));
  if(j_tmp == NULL) {
    perror("malloc");
    exit(1);
  }
  strcpy(j_tmp->pseudo, pseudo);
  j_tmp->socket = socket;
  j_tmp->next = NULL;
  pthread_mutex_init(&(j_tmp->mutex), NULL);
  return j_tmp;
}

void detruire_joueur(Joueur *j) {
  pthread_mutex_destroy(&(j->mutex));
  //close(j->socket);
  //free(j);
}


char *pseudoJoueur(Joueur *j) {
  // Change jamais, pas besoin de lock
  return j->pseudo;
}
