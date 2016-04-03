#include <timer.h>

/* ----------- Mettre dans fichier timer.(c|h) -------------------- */
// Signal la condition proteger par mut au bout de temps seconde
// Faudrait detruire le thread si on se debloque avant qu'il se termine
void timer(pthread_t *pt, int temps, int *flag, pthread_cond_t *cond, pthread_mutex_t *mut) {
  ArgTimer *at = (ArgTimer*)malloc(sizeof(ArgTimer));
  if(at == NULL) {
    perror("malloc");
    exit(1);
  }
  at->temps = temps;
  at->cond = cond;
  at->mut = mut;
  at->flag = flag;
  pthread_create(pt, NULL, timer_thread, (void *)(at));
}


// Peut etre annuler le thread si le temps un joueur a fait une requete dans gestion_session phase reflexion
// Signal la condition 
void *timer_thread(void *arg) {
  ArgTimer *at = (ArgTimer *)arg;
  sleep(at->temps); // en minute
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL); // Desactiver annulation
  pthread_mutex_lock(at->mut);
  *(at->flag) = 1;
  pthread_cond_signal(at->cond);
  pthread_mutex_unlock(at->mut);
  free(at);
  return NULL;
}
