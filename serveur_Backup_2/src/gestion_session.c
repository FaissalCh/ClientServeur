#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <gestion_session.h>
#include <liste_joueurs.h>
#include <fonctions_jeu.h>
#include <tools.h>
#include <timer.h>

#define TBUF 512


void phaseResolution(Session *s, Joueur *jActif);


void *gestionSession(void *arg) { // Mutex sur la session ?
  Session *s = (Session *)arg;
  char buf[TBUF];
  char enigme[] = "(2,3,2,5,10,9,11,0,9,1,V)";
  char *bilan;
  Joueur *joueurActif;

  while(1) { // En faite tant que y a moins d'1 joueur, ou qu'un joueur atteint le score objectif

    // Debut du tour
    /* ---------------------  Attente d'au moins 2 joueurs ------------------- */
    pthread_mutex_lock(&(s->liste->mutex));
    while(s->liste->nbJoueur < 2) 
      pthread_cond_wait(&(s->condConnexion), &(s->liste->mutex));
    pthread_mutex_unlock(&(s->liste->mutex));
    sleep(5); // On laisse un peu de temps aux autres d'arriver
    beActif(s->liste);   // Les rendres actifs


    printf("[GESTIONNAIRE] Le tour commence avec %d joueurs\n", nbJoueurActifListe(s->liste));


    // Partie initSession
    setEnchereToInit(s);

    /* ------------------- Phase reflexion -------------------------- */
    pthread_mutex_lock(&(s->mutex));
    s->tempsReflexionFini = 0;
    s->timerOut = 1;
    s->phase = REFLEXION;
    pthread_mutex_unlock(&(s->mutex));
    
    // Send l'enigme
    bilan = getBilanSession(s);
    sprintf(buf, "TOUR/%s/%s\n", enigme, bilan);
    free(bilan);

    printf("[GESTIONNAIRE] Debut de la phase de reflexion !!\n");
    // att rep ou fin timer
    sendToAll(buf, s->liste, NULL);
    // Active le timer, faudrait le desactiver si on recoit une reponse avant la fin !!!
    timer(&(s->timerThread), TEMPS_REFLEXION*60, &(s->tempsReflexionFini), &(s->condFinReflexion), &(s->mutex));

    // recoit une reponse avant la fin ou fin timer (si reponse recu avant timer => gestion dans fonction trouve()
    pthread_mutex_lock(&(s->mutex));
    while(!(s->tempsReflexionFini))
      pthread_cond_wait(&(s->condFinReflexion), &(s->mutex));
    if(s->timerOut)
      sendToAll("FINREFLEXION/\n", s->liste, NULL);
    pthread_mutex_unlock(&(s->mutex));
    printf("[GESTIONNAIRE] Fin de la phase de reflexion !!\n");


    /* ---------- Phase enchere ------------- */
    printf("[GESTIONNAIRE] Debut de la phase d'enchere !!\n");
    initEnchere(s);
    sleep(TEMPS_ENCHERE); // Temps en seconde
    joueurActif = terminerEnchere(s); // Si personne n'a fait d'enchere BOOOM !!!!!!!!!!!!!!!!!!!!!!!!!
    printf("[GESTIONNAIRE] Fin de la phase d'enchere !!\n");


    /* ---------- Phase resolution ------------- */
    printf("[GESTIONNAIRE] Debut de la phase de resolution !!\n");
    pthread_mutex_lock(&(s->mutex));
    s->phase = RESOLUTION; // Peut etre le mettre avant l'envoie du message FINENCHERE
    pthread_mutex_unlock(&(s->mutex));
    phaseResolution(s, joueurActif);
    printf("[GESTIONNAIRE] Fin de la phase de resolution !!\n");

  }
  return NULL;
}


void phaseResolution(Session *s, Joueur *jActif) {

  sleep(TEMPS_RESOLUTION);
  // Je crois gerer FINRESO et TROPLONG

}



// Enchere
void setEnchereToInit(Session *s) {
  /* Initialise la valeur d'enchere (meme des joueurs qui jouent pas encore, mais pas grave) */
  Joueur *cur = (s->liste)->j;
  while(cur != NULL) {
    cur->enchere = -1;
    cur->actif = 0;
    cur = cur->next;
  }
}

void initEnchere(Session *s) {
  pthread_mutex_lock(&(s->mutex));
  s->finEnchere = 0;
  s->phase = ENCHERE;
  pthread_mutex_unlock(&(s->mutex));
}

Joueur *terminerEnchere(Session *s) { // Return le joueur qui a fait l'enchere minimal
  Joueur *jActif;
  int resMin = -1;
  Joueur *cur;
  char buf[TBUF];

  pthread_mutex_lock(&(s->mutex));
  s->finEnchere = 1;

  cur = (s->liste)->j;
  while(cur != NULL) {
    if(cur->playSessionEnCours) {
      if(resMin == -1) {
	resMin = cur->enchere;
	jActif = cur;
      } else {
	if(cur->enchere < resMin) {
	  resMin = cur->enchere;
	  jActif = cur;
	}
      }
    }
    cur = cur->next;
  }
  jActif->actif = 1;
  sprintf(buf, "FINENCHERE/%s/%d/\n", jActif->pseudo, jActif->enchere);
  pthread_mutex_unlock(&(s->mutex));
  sendToAll(buf, s->liste, NULL);
  return jActif;
}
// Fin Enchere


/* Met la variable playSessionEnCours a true et pas la varible ACTIF !!*/
void beActif(ListeJoueurs *l) {
  Joueur *j;
  pthread_mutex_lock(&(l->mutex));
  j = l->j;
  while(j != NULL) {
    j->playSessionEnCours = 1;
    j->enchere = -1;
    j = j->next;
  }
  pthread_mutex_unlock(&(l->mutex));
}

