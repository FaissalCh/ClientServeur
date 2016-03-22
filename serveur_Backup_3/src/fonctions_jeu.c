#include <tools.h>
#include <types_jeu.h>
#include <fonctions_jeu.h>

#define T_ENI 2048 // Faire petit calcul pour borner la taille


// Mettre dans session.(c|h)
char *getBilanSession(Session *s) {
  // Prendre mutex session, pas besoin je crois
  int nbTour = s->nbTour;
  ListeJoueurs *l = s->liste;
  char res[T_ENI];
  char tmp[T_ENI];
  Joueur *j;
  
  // Mutex liste
  pthread_mutex_lock(&(l->mutex));
  j = l->j;
  sprintf(res, "%d", nbTour);
  while(j != NULL) {
    sprintf(tmp, "(%s,%d)", j->pseudo, j->score);
    strcat(res, tmp);
    j = j->next;
  }
  pthread_mutex_unlock(&(l->mutex)); // Lacher mutex
  return strdup(res);
}
