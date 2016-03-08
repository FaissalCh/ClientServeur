#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>

#define TBUF 512

Joueur *connex(int sock, ListeJoueurs *liste);
void sort(ListeJoueurs *liste, Joueur *myJoueur);
void trouve();
void beActif(ListeJoueurs *l);
void timer(pthread_t *pt, int temps, int *flag, pthread_cond_t *cond, pthread_mutex_t *mut);
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
      trouve(session, myJoueur);
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



void trouve(Session *s, Joueur *myJoueur) {
  int nbCoup;
  char buf[TBUF];
 
  pthread_mutex_lock(&(s->mutex));
  strtok(NULL, "/"); /* Le nom */
  nbCoup = atoi(strtok(NULL, "/"));
  if(s->tempsReflexionFini) { // Au cas ou plrs j propose en meme temps
    // trop tard
    // Je sais plus quoi mettre
  }
  else {
    s->tempsReflexionFini = 1;
    s->timerOut = 0;
    myJoueur->enchere = nbCoup;
    if(pthread_cancel(s->timerThread)) {
      perror("pthread_cancel");
    }
    sprintf(buf, "ILATROUVE/%s/%d\n", pseudoJoueur(myJoueur), nbCoup);
    sendToAll(buf, s->liste, myJoueur);
    sprintf(buf, "TUASTROUVE/\n");
    sendTo(buf, s->liste, myJoueur);
    pthread_cond_signal(&(s->condFinReflexion));
  }
  pthread_mutex_unlock(&(s->mutex));
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



void *gestionSession(void *arg) { // Mutex sur la session ?
  Session *s = (Session *)arg;
  char buf[TBUF];
  char enigme[] = "(2,3,2,5,10,9,11,0,9,1,V)";
  char *bilan;


  // Debut du tour
  // Attente d'au moins 2 joueurs
  pthread_mutex_lock(&(s->liste->mutex));
  while(s->liste->nbJoueur < 2) 
    pthread_cond_wait(&(s->condConnexion), &(s->liste->mutex));
  pthread_mutex_unlock(&(s->liste->mutex));
  sleep(5); // On laisse un peu de temps aux autres d'arriver
  beActif(s->liste);   // Les rendres actifs


  printf("[GESTIONNAIRE] Le tour commence avec %d joueurs\n", nbJoueurListe(s->liste));

  /* ----- Phase reflexion ----- */
  pthread_mutex_lock(&(s->mutex));
  s->tempsReflexionFini = 0;
  s->timerOut = 1;
  pthread_mutex_unlock(&(s->mutex));
    
  // Send l'enigme
  bilan = getBilanSession(s);
  sprintf(buf, "TOUR/%s/%s\n", enigme, bilan);
  free(bilan);

  printf("[GESTIONNAIRE] Debut de la phase de reflexion !!\n");
  // att rep ou fin timer
  sendToAll(buf, s->liste, NULL);
  // Active le timer, faudrait le desactiver si on
  timer(&(s->timerThread), TEMPS_REFLEXION*60, &(s->tempsReflexionFini), &(s->condFinReflexion), &(s->mutex));

  // recoit une reponse avant la fin
  pthread_mutex_lock(&(s->mutex));
  while(!(s->tempsReflexionFini))
    pthread_cond_wait(&(s->condFinReflexion), &(s->mutex));
  if(s->timerOut)
    sendToAll("FINREFLEXION/\n", s->liste, NULL);
  pthread_mutex_unlock(&(s->mutex));
  printf("[GESTIONNAIRE] Fin de la phase de reflexion !!\n");


  /* ---------- Phase enchere ------------- */
  printf("[GESTIONNAIRE] Debut de la phase d'enchere !!\n");
  // S'endormir pendant le temps de la phase d'enchere le temps que les autres 
  // Fassent leurs mises, pas oublier de bien initialiser la valeur d'enchere
  // J'ai rien a faire la mettre une variabiel FinEnchere dans la struct Session
  // Quand je wakeup je la met a false comme ca ils peuvent plus encherir, et pas
  // zappe de proteger ak mutex bien sur apres on passe a la suite.
  printf("[GESTIONNAIRE] Fin de la phase d'enchere !!\n");


  /* ---------- Phase resolution ------------- */



  return NULL;
}



/* Rend actif les joueurs */
void beActif(ListeJoueurs *l) {
  Joueur *j;
  pthread_mutex_lock(&(l->mutex));
  j = l->j;
  while(j != NULL) {
    j->actif = 1;
    j->enchere = -1;
    j = j->next;
  }
  pthread_mutex_unlock(&(l->mutex));
}




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
