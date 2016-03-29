#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>
#include <liste_joueurs.h>

#define TBUF 512

void setEnchereToInit(Session *s);
char *nbCoupsDejaPropose(int nbCoups, Joueur *myJoueur, Session *s);


// Enchere
Joueur *terminerEnchere(Session *s);

void resolution(Session *s, Joueur *myJoueur);
void enchere(Session *s, Joueur *myJoueur);
Joueur *connex(int sock, ListeJoueurs *liste);
void sort(ListeJoueurs *liste, Joueur *myJoueur);
void trouve();



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
      //char plateau[] = "(4,0,D)(9,0,D)(2,1,D)(2,1,B)(13,1,H)(13,1,D)"; // juste pour test
      char *plateau = session->p->plateauString;
      sprintf(buf, "SESSION/%s/\n", plateau); // plateau provient de session normalement
      sendTo(buf, listeJ, myJoueur, 1); // Send le plateau
      break;
    case 2: /* SORT */
      printf("[Deconnexion] '%s'\n", myJoueur->pseudo);
      sort(listeJ, myJoueur);
      decoProprement = 1;
      break;
    case 3: // SOLUTION // Reflexion ou resolution
      // Prevenir l'arbitre de la session
      pthread_mutex_lock(&(session->mutex));
      if(session->phase == REFLEXION) {
	pthread_mutex_unlock(&(session->mutex));
	trouve(session, myJoueur);
      }
      else if(session->phase == RESOLUTION) {
	pthread_mutex_unlock(&(session->mutex));
	resolution(session, myJoueur);
      }
      else {
	printf("WHAT not reflexion or resolution...\n");
	exit(1); /////////////// A supp
      }
      printf("[Trouve] '%s'\n", myJoueur->pseudo);
      break;
    case 4: // ENCHERE
      enchere(session, myJoueur);
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






void resolution(Session *s, Joueur *myJoueur) {
  char *deplacement;

  strtok(NULL, "/"); /* Le nom */
  deplacement =  strtok(NULL, "/");

  // Verifier si timer pas ecoule 
  pthread_mutex_lock(&(s->mutex));
  if(s->timerResolutionFini) {
    pthread_mutex_unlock(&(s->mutex));
    return;
  }

  // Si je suis pas le joueur actif (pas de triche)
  if(!myJoueur->actif) { 
    pthread_mutex_unlock(&(s->mutex));
    return;
  }
  
  // Termine le timer
  s->timerResolutionFini = 1;
  s->timerOut = 0;
  if(pthread_cancel(s->timerThread)) {
      perror("pthread_cancel");
  }

  strcpy(s->deplacementCur, deplacement);// Indique sa reponse au gestionnaire de session et le reveille
  pthread_cond_signal(&(s->condFinResolution));
  pthread_mutex_unlock(&(s->mutex));
}




///////////////////////////////////////////////////////////////////////////::

void enchere(Session *s, Joueur *myJoueur) {
  int nbCoups;
  char *otherUser; // pseudo du joueur qui a deja propose la meme enchere
  char buf[TBUF];
  char buf2[TBUF];

  strtok(NULL, "/"); /* Le nom */
  nbCoups = atoi(strtok(NULL, "/"));
  pthread_mutex_lock(&(s->mutex));

  if(s->phase != ENCHERE) { // Pas encore la phase d'enchere
    printf("Doucement la phase d'enchere a pas commence ...., gestion_client[%d]\n", __LINE__);
    sprintf(buf, "ECHEC/%s/\n", myJoueur->pseudo);
    sendTo(buf, s->liste, myJoueur, 0);
    pthread_mutex_unlock(&(s->mutex));
    return;
  }

  if((myJoueur->enchere != -1) && (nbCoups >= myJoueur->enchere)) {
    sprintf(buf, "ECHEC/%s/\n", myJoueur->pseudo);
    sendTo(buf, s->liste, myJoueur, 0);
  }
  else if((otherUser = nbCoupsDejaPropose(nbCoups, myJoueur, s)) != NULL) {
    sprintf(buf, "ECHEC/%s/\n", otherUser);
    sendTo(buf, s->liste, myJoueur, 0);
  }
  else { // Ok
    myJoueur->enchere = nbCoups;
    sprintf(buf, "VALIDATION/\n");
    sprintf(buf2, "NOUVELLEENCHERE/%s/%d/\n", myJoueur->pseudo, nbCoups);
    sendTo(buf, s->liste, myJoueur, 0);
    sendToAll(buf2, s->liste, myJoueur, 0);
  }  

  pthread_mutex_unlock(&(s->mutex));
}


char *nbCoupsDejaPropose(int nbCoups, Joueur *myJoueur, Session *s) {
  Joueur *j = (s->liste)->j;
  while(j != NULL) {
    if( (j != myJoueur) && (j->enchere > 0) && (j->enchere == nbCoups))
      return j->pseudo;
    j = j->next;
  }
  return NULL;
}


void trouve(Session *s, Joueur *myJoueur) {
  int nbCoup;
  char buf[TBUF];
 
  pthread_mutex_lock(&(s->mutex));
  strtok(NULL, "/"); /* Le nom */
  nbCoup = atoi(strtok(NULL, "/"));
  if(s->tempsReflexionFini) { // Au cas ou plrs j propose en meme temps, (et que le temps de traiter un le chrono est fini je crois
    printf("Attention : temps reflexion fini, gestion_client[%d]\n", __LINE__);
    // trop tard
    // Je sais plus quoi mettre
  }
  else {
    printf("[NBCOUP] = %d, gestion_client[%d]\n", nbCoup, __LINE__);
    s->tempsReflexionFini = 1;
    s->timerOut = 0;
    myJoueur->enchere = nbCoup;
    if(pthread_cancel(s->timerThread)) {
      perror("pthread_cancel");
    }
    sprintf(buf, "ILATROUVE/%s/%d\n", pseudoJoueur(myJoueur), nbCoup);
    sendToAll(buf, s->liste, myJoueur, 1);
    sprintf(buf, "TUASTROUVE/\n");
    sendTo(buf, s->liste, myJoueur, 1);
    pthread_cond_signal(&(s->condFinReflexion));
  }
  pthread_mutex_unlock(&(s->mutex));
}



void sort(ListeJoueurs *liste, Joueur *myJoueur) {
  char buf[TBUF];
  sprintf(buf, "SORTI/%s/\n", myJoueur->pseudo);    
  suppJoueurListe(liste, myJoueur);
  detruire_joueur(myJoueur);
  sendToAll(buf, liste, NULL, 1); 
}



/* Faudra gerer si plusieurs joueurs ont le meme nom */
Joueur *connex(int sock, ListeJoueurs *liste) {
  char buf[TBUF];
  Joueur *myJoueur = create_joueur(strtok(NULL, "/"), sock); 
  addJoueurListe(liste, myJoueur);
  /* Validation de la connexion a user */
  sprintf(buf, "BIENVENUE/%s/\n", pseudoJoueur(myJoueur));
  sendTo(buf, liste, myJoueur, 1);
  /* Signalement connexion user aux autres joueurs */
  sprintf(buf, "CONNECTE/%s/\n", pseudoJoueur(myJoueur));
  sendToAll(buf, liste, myJoueur, 1); // All sauf myJoueur
  return myJoueur;
}










