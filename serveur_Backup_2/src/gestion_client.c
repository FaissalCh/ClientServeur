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

int solutionAccepte(char *sol, Session *s); // A FAIRE
Joueur *getNewJoueurActif(Session *s);

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
      char plateau[] = "(4,0,D)(9,0,D)(2,1,D)(2,1,B)(13,1,H)(13,1,D)"; // juste pour test
      sprintf(buf, "SESSION/%s/\n", plateau); // plateau provient de session normalement
      sendTo(buf, listeJ, myJoueur, 1); // Send le plateau
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
    case 4: // ENCHERE
      enchere(session, myJoueur);
      break;
    case 5: // SOLUTION
      resolution(session, myJoueur);
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
  char buf[TBUF];
  Joueur *newActif;

  strtok(NULL, "/"); /* Le nom */
  deplacement =  strtok(NULL, "/");

  // Si je suis pas le joueur actif (pas de triche)
  pthread_mutex_lock(&(s->mutex));
  if(!myJoueur->actif) { 
    pthread_mutex_unlock(&(s->mutex));
    return;
  }

  // Signalement aux clients de la solution propose
  sprintf(buf, "SASOLUTION/%s/%s\n", myJoueur->pseudo, deplacement);
  pthread_mutex_unlock(&(s->mutex));
  sendToAll(buf, s->liste, myJoueur);
  
  pthread_mutex_lock(&(s->mutex));
  if(solutionAccepte(deplacement, s)) {
    myJoueur->score++;
    sprintf(buf, "BONNE\n");
    pthread_mutex_unlock(&(s->mutex));
    sendToAll(buf, s->liste, NULL);
    pthread_mutex_lock(&(s->mutex));
  }
  else { // Mauvaise solution
    myJoueur->actif = 0;
    myJoueur->enchere = -1;
    newActif = getNewJoueurActif(s);
    if(newActif == NULL) { // Plus de joueurs restants ==> fin du tour
      sprintf(buf, "FINRESO/\n", newActif->pseudo);
    } 
    else {
      newActif->actif = 1;
      sprintf(buf, "MAUVAISE/%s/\n", newActif->pseudo);
    }
  }
  
}


Joueur *getNewJoueurActif(Session *s) {
  return NULL;
}

int solutionAccepte(char *sol, Session *s) { // Mettre dans fonction_jeu.c
  return 0;
}


void enchere(Session *s, Joueur *myJoueur) {
  int nbCoups;
  char *otherUser; // pseudo du joueur qui a deja propose la meme enchere
  char buf[TBUF];
  char buf2[TBUF];

  strtok(NULL, "/"); /* Le nom */
  nbCoups = atoi(strtok(NULL, "/"));
  pthread_mutex_lock(&(s->mutex));

  if(s->phase != ENCHERE) { // Pas encore la phase d'enchere
    sprintf(buf, "ECHEC/%s/\n", myJoueur->pseudo);
    pthread_mutex_unlock(&(s->mutex));
    sendTo(buf, s->liste, myJoueur);
    return;
  }

  if((myJoueur->enchere != -1) && (nbCoups >= myJoueur->enchere)) {
    sprintf(buf, "ECHEC/%s/\n", myJoueur->pseudo);
    pthread_mutex_unlock(&(s->mutex));
    sendTo(buf, s->liste, myJoueur);
  }
  else if((otherUser = nbCoupsDejaPropose(nbCoups, myJoueur, s)) != NULL) {
    sprintf(buf, "ECHEC/%s/\n", otherUser);
    pthread_mutex_unlock(&(s->mutex));
    sendTo(buf, s->liste, myJoueur);
  }
  else { // Ok
    myJoueur->enchere = nbCoups;

    sprintf(buf, "VALIDATION/\n");
    sprintf(buf2, "NOUVELLEENCHERE/%s/%d/\n", myJoueur->pseudo, nbCoups);
    pthread_mutex_unlock(&(s->mutex));
    sendTo(buf, s->liste, myJoueur);
    sendToAll(buf2, s->liste, myJoueur);
  }  

}


char *nbCoupsDejaPropose(int nbCoups, Joueur *myJoueur, Session *s) {
  Joueur *j = (s->liste)->j;
  while(j != NULL) {
    if( (j != myJoueur) && (j->enchere == nbCoups))
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










