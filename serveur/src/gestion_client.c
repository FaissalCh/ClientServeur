#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>

#define TBUF 256

Joueur *connex(int sock, ListeJoueurs *liste);
void sort(ListeJoueurs *liste, Joueur *myJoueur);
void trouve();
void beActif(ListeJoueurs *l);
void timer(int temps, pthread_cond_t *cond, pthread_mutex_t *mut);
void *timer_thread(void *arg);


void *gestionClient(void *argThread) {
  ArgThread *arg = (ArgThread *)argThread;
  int sock = arg->socket;
  Session *session = arg->session;
  ListeJoueurs *listeJ = session->liste;

  int nbRead;
  char buf[TBUF];
  char *req;
  int cpt=0;
  int decoProprement = 0;
  Joueur *myJoueur = NULL; /* Joueur du thread courant */
  free(arg); 


  while((cpt<TBUF) && (nbRead=read(sock, &buf[cpt++], 1)) > 0) {
    if(buf[cpt-1] != '\n') /* Lecture d'une ligne */
      continue;
    buf[cpt-1] = '\0';
    cpt = 0;
  
    printf("[Receive] : %s\n", buf); 
    req = strtok(buf, "/");
    
    switch(hash_protocole(req)) {
    case 1: /* CONNEX */
      myJoueur = connex(sock, listeJ); 
      pthread_mutex_lock(&(session->liste->mutex));
      pthread_cond_signal(&(session->condConnexion)); // indique que y a 1 connexion
      pthread_mutex_unlock(&(session->liste->mutex));
      printf("[Connexion] : '%s', %d joueurs\n", myJoueur->pseudo, nbJoueurListe(listeJ));
      char plateau[] = "(4,0,D)(9,0,D)(2,1,D)(2,1,B)(13,1,H)(13,1,D)"; // juste pour test
      sprintf(buf, "SESSION/%s/\n", plateau); // plateau provient de session normalement
      sendTo(buf, listeJ, myJoueur); // Send le plateau
      break;
    case 2: /* SORT */
      printf("[Deconnexion] '%s'\n", myJoueur->pseudo);
      sort(listeJ, myJoueur);
      decoProprement = 1;
      break;
    case 3: // TROUVE
      // Prevenir l'arbitre de la session
      trouve();
      printf("[Trouve] '%s'\n", myJoueur->pseudo);
      break;
    default:
      printf("[Requete inconnue] '%s'\n", req);
    }
  }

  if(!decoProprement) { /* Cas ou fermeture socket sans SORT */
    printf("[Deconnexion] '%s'\n", myJoueur->pseudo);
    sort(listeJ, myJoueur);
  }


  return NULL;
}

void sort(ListeJoueurs *liste, Joueur *myJoueur) {
  char buf[TBUF];
  sprintf(buf, "SORTI/%s/\n", myJoueur->pseudo);    
  suppJoueurListe(liste, myJoueur);
  detruire_joueur(myJoueur);
  sendToAll(buf, liste, NULL); 
}

/* Faudra gerer si plusieurs joueurs ont le meme nom */
Joueur *connex(int sock, ListeJoueurs *liste) {
  char buf[TBUF];
  Joueur *myJoueur = create_joueur(strtok(NULL, "/"), sock); 
  addJoueurListe(liste, myJoueur);
  /* Validation de la connexion a user */
  sprintf(buf, "BIENVENUE/%s/\n", pseudoJoueur(myJoueur));
  sendTo(buf, liste, myJoueur);
  /* Signalement connexion user aux autres joueurs */
  sprintf(buf, "CONNECTE/%s/\n", pseudoJoueur(myJoueur));
  sendToAll(buf, liste, myJoueur); // All sauf myJoueur
  return myJoueur;
}


void trouve() {
  //int nbCoup;
  strtok(NULL, "/"); /* Le nom */
  //nbCoup = atoi(strtok(NULL, "/"));
}

void *gestionSession(void *arg) { // Mutex sur la session ?
  Session *s = (Session *)arg;
  char buf[TBUF];
  char enigme[] = "(2,3,2,5,10,9,11,0,9,1,V)";
  char *bilan;

  // Attente d'au moins 2 joueurs
  pthread_mutex_lock(&(s->liste->mutex));
  while(s->liste->nbJoueur < 2) 
    pthread_cond_wait(&(s->condConnexion), &(s->liste->mutex));
  pthread_mutex_unlock(&(s->liste->mutex));
  sleep(5); // On laisse un peu de temps aux autres d'arriver
  beActif(s->liste);   // Les rendres actifs


  printf("[News] Le tour commence avec %d joueurs\n", nbJoueurListe(s->liste));

  /* ----- Phase reflexion ----- */
  // Send l'enigme
  bilan = getBilanSession(s);
  sprintf(buf, "TOUR/%s/%s\n", enigme, bilan);
  free(bilan);
  sendToAll(buf, s->liste, NULL);
  // Attend une reponse ou fin timer 
  timer(TEMPS_REFLEXION*60, s->condFinReflexion, s->mutex);
  // Phase enchere 

  // Phase resolution



  return NULL;
}



/* Rend actif les joueurs */
void beActif(ListeJoueurs *l) {
  Joueur *j;
  pthread_mutex_lock(&(l->mutex));
  j = l->j;
  while(j != NULL) {
    j->actif = 1;
    j = j->next;
  }
  pthread_mutex_unlock(&(l->mutex));
}




/* ----------- Mettre dans fichier timer.(c|h) -------------------- */
// Signal la condition proteger par mut au bout de temps seconde
// Faudrait detruire le thread si on se debloque avant qu'il se termine
void timer(int temps, pthread_cond_t *cond, pthread_mutex_t *mut) {
  ArgTimer at;
  pthread_t tmp;
  at.temps = temps;
  at.cond = cond;
  at.mut = mut;
  pthread_create(&tmp, NULL, timer_thread, (void *)(&at));
}

// Signal la condition 
void *timer_thread(void *arg) {
  ArgTimer *at = (ArgTimer *)arg;
  sleep(at->temps); // en minut
  pthread_mutex_lock(at->mut);
  pthread_cond_signal(at->cond);
  pthread_mutex_unlock(at->mut);
  return NULL;
}
