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
Joueur *getNewJoueurActif(Session *s);


// La session va contenir un plateau et un tableau d'enigme
void *gestionSession(void *arg) { // Mutex sur la session ?
  Session *s = (Session *)arg;
  char buf[TBUF];
  // getEnigme s->enigme[nbTour % nbEnigme]
  char *enigme = s->p->enigme.enigmeString;
  char *bilan;
  Joueur *joueurActif;

  while(1) { // En faite tant que y a moins d'1 joueur, ou qu'un joueur atteint le score objectif
    // Verifier si un joueur a le score objectif


    // Debut du tour
    /* ---------------------  Attente d'au moins 2 joueurs ------------------- */
    pthread_mutex_lock(&(s->liste->mutex));
    while(s->liste->nbJoueur < 2) 
      pthread_cond_wait(&(s->condConnexion), &(s->liste->mutex));
    pthread_mutex_unlock(&(s->liste->mutex));
    sleep(TEMPS_ATTENTE_JOUEURS); // On laisse un peu de temps aux autres d'arriver
    beActif(s->liste);   // Les rendres actifs


    printf("[GESTIONNAIRE] Le tour commence avec %d joueurs\n", nbJoueurActifListe(s->liste));
    

    // Partie initSession
    setEnchereToInit(s);

    /* ------------------- Phase reflexion -------------------------- */
    pthread_mutex_lock(&(s->mutex));
    s->nbTour++;
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
    sendToAll(buf, s->liste, NULL, 1);
    // Active le timer, faudrait le desactiver si on recoit une reponse avant la fin !!!
    timer(&(s->timerThread), TEMPS_REFLEXION, &(s->tempsReflexionFini), &(s->condFinReflexion), &(s->mutex));

    // recoit une reponse avant la fin ou fin timer (si reponse recu avant timer => gestion dans fonction trouve()
    pthread_mutex_lock(&(s->mutex));
    while(!(s->tempsReflexionFini))
      pthread_cond_wait(&(s->condFinReflexion), &(s->mutex));
    if(s->timerOut)
      sendToAll("FINREFLEXION/\n", s->liste, NULL, 1);
    s->phase = ENCHERE;
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

    pthread_mutex_lock(&(s->mutex));
    s->phase = REFLEXION; // Deja fait apres mais pas grave
    pthread_mutex_unlock(&(s->mutex));
    printf("[GESTIONNAIRE] Fin de la phase de resolution !!\n");

  }
  return NULL;
}















// Si client send reponse notifier le thread gestion_session
void phaseResolution(Session *s, Joueur *jActif) {
  char buf[TBUF];

  pthread_mutex_lock(&(s->mutex));  

  if(jActif == NULL) { // A TESTER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NEW
    sprintf(buf, "FINRESO/\n");
    sendToAll(buf, s->liste, NULL, 1);
    pthread_mutex_unlock(&(s->mutex));
    return;
  }
  
  // Attente d'une reponse ou fin timer
  s->timerResolutionFini = 0;
  s->timerOut = 1;
  timer(&(s->timerThread), TEMPS_RESOLUTION, &(s->timerResolutionFini), &(s->condFinResolution), &(s->mutex));
  while(!(s->timerResolutionFini))
    pthread_cond_wait(&(s->condFinResolution), &(s->mutex));

  /* Reponse recu ou fin timer */

  if(s->timerOut) { // Pas recu de reponse dans le temps imparti
    jActif->actif = 0;
    jActif->enchere = -1;
    jActif = getNewJoueurActif(s);
    if(jActif == NULL) { // Plus de joueur actif on termine le tour
      sprintf(buf, "FINRESO/\n");
      sendToAll(buf, s->liste, NULL, 1);
      pthread_mutex_unlock(&(s->mutex));
    }
    else { // On choisit un autre jActif et on recommence phase reso
      jActif->actif = 1;
      sprintf(buf, "TROPLONG/%s/\n", jActif->pseudo);
      s->timerResolutionFini = 0; // Certainement doublon
      sendToAll(buf, s->liste, NULL, 1);
      pthread_mutex_unlock(&(s->mutex));
      phaseResolution(s, jActif);
    }
  } 
  else { // Reponse recu
    // Signalement aux clients de la solution propose
    sprintf(buf, "SASOLUTION/%s/%s\n", jActif->pseudo, s->deplacementCur);
    sendToAll(buf, s->liste, jActif, 0);
    printf("[Debug] test solution\n");
    if(solutionAccepte(s->deplacementCur, s, jActif)) {
      printf("[Debug] test solution ACCEPTE\n");
      jActif->actif = 0;
      jActif->enchere = -1;// Peut etre pas ici !!!!!!!, mais au debut du tour !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      jActif->score++;
      sprintf(buf, "BONNE\n");
      sendToAll(buf, s->liste, NULL, 0);
      pthread_mutex_unlock(&(s->mutex));
    } else { // Mauvaise solution
      printf("[Debug] test solution REJETE\n");
      jActif->actif = 0;
      jActif->enchere = -1;
      jActif = getNewJoueurActif(s);
      // ENORME DOUBLON
      if(jActif == NULL) { // Plus de joueur actif on termine le tour
	sprintf(buf, "FINRESO/\n");
	sendToAll(buf, s->liste, NULL, 1);
	pthread_mutex_unlock(&(s->mutex));
      } else { // On choisit un autre jActif et on recommence phase reso
	jActif->actif = 1;
	sprintf(buf, "MAUVAISE/%s/\n", jActif->pseudo);
	s->timerResolutionFini = 0; // Certainement doublon
	sendToAll(buf, s->liste, NULL, 1);
	pthread_mutex_unlock(&(s->mutex));
	phaseResolution(s, jActif);
      }
    }
  }
}


// Lock la liste
Joueur *getNewJoueurActif(Session *s) {
  ListeJoueurs *l = s->liste;
  Joueur *cur;
  Joueur *res = NULL;
  int enchereMin;

  pthread_mutex_lock(&(l->mutex));
  cur = l->j;
  while(cur != NULL) { // Peut etre tester playSessionEnCours !!!!!!!!!!!!!!!!!!!!!!! Sinon il sert peut etre a rien
    if(res == NULL && (cur->enchere > 0)) {
      res = cur;
      enchereMin = cur->enchere;
    }
    else if((cur->enchere > 0) && (cur->enchere < enchereMin)) {
      res = cur;
      enchereMin = cur->enchere;
    }
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return res;
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
  s->phase = ENCHERE; // Deja fait avant mais pas grave
  pthread_mutex_unlock(&(s->mutex));
}


// Apparament parfois retourne le mauvais joueur
// Gerer cas ou joueurs a pas fait d'enchere
Joueur *terminerEnchere(Session *s) { // Return le joueur qui a fait l'enchere minimal
  Joueur *jActif = NULL;
  int resMin = -1;
  Joueur *cur;
  char buf[TBUF];

  pthread_mutex_lock(&(s->mutex));
  s->finEnchere = 1;

  pthread_mutex_unlock(&(s->liste->mutex));
  cur = (s->liste)->j;
  while(cur != NULL) {
    if(cur->playSessionEnCours && cur->enchere > 0) {
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
  if(jActif == NULL) { // Auncune enchere
    printf("JActif == NULL, gestion_session[%d]\n", __LINE__);
    return NULL;
  }
  jActif->actif = 1; // Verfier si on le remet bien a 0 plus tard
  sprintf(buf, "FINENCHERE/%s/%d/\n", jActif->pseudo, jActif->enchere); // Parfois me renvois n'importe quoi
  s->timerResolutionFini = 0; // Certainement doublon
  sendToAll(buf, s->liste, NULL, 0);
  pthread_mutex_unlock(&(s->liste->mutex));
  pthread_mutex_unlock(&(s->mutex));

  return jActif;
}
// Fin Enchere


/* Met la variable playSessionEnCours a true et pas la varible ACTIF !!*/
void beActif(ListeJoueurs *l) {
  Joueur *j;
  pthread_mutex_lock(&(l->mutex));
  j = l->j;
  while(j != NULL) {
    j->playSessionEnCours = 1; // Peut etre le remettre a 0 a la fin d'un tour
    j->enchere = -1;
    j = j->next;
  }
  pthread_mutex_unlock(&(l->mutex));
}


