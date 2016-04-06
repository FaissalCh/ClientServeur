#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>
#include <liste_joueurs.h>
#include <liste_sessions.h>
#include <gestion_session.h>
#include <gestion_client.h>

#define TBUF 512

extern Session *sessionDeBase; //pour reinit la session de base si plus de joueurs

extern pthread_mutex_t mutexAllSession; /* Pour eviter qu'un joueur tente de se connecter a la session  
					   pendant qu'elle soit re-cree apres sa destruction, ou pendant ce destruction, ceci arrive 
					   lorsque le dernier joueur quitte la session on la re initialise
					   pour eviter que les nouveaux joueurs attendent la fin du dernier tour
					   alors qu'il n y a plus de joueur dans le cas de la session public sinon on la detruit */


// Fonction qu'execute les threads de gestion de client
void *gestionClient(void *argThread) {
  ArgThread *arg = (ArgThread *)argThread;
  int sock = arg->socket;
  Session *session = NULL;// = arg->session;
  ListeJoueurs *listeJ = NULL;//session->liste;
  pthread_t tmp;
  char *nomSession;
  int nbRead;
  char buf[TBUF];
  char *req;
  int cpt=0;
  int decoProprement = 0;
  Joueur *myJoueur = NULL; /* Joueur du thread courant */
  free(arg); 

  while((!decoProprement) && (cpt<TBUF) && (nbRead=read(sock, &buf[cpt++], 1)) > 0) {
    /* Lecture d'une ligne */
    if(buf[cpt-1] != '\n') 
      continue;
    buf[cpt-1] = '\0';
    cpt = 0;
  
    printf("[Receive] : %s\n", buf); 
    req = strtok(buf, "/");

    switch(hash_protocole(req)) {
    case 1: /* CONNEX */
      pthread_mutex_lock(&mutexAllSession); // Au cas ou une destruction en cours
      pthread_mutex_lock(&(sessionDeBase->mutex));
      session = sessionDeBase;
      listeJ = session->liste;
      myJoueur = connex(sock, listeJ); 
      if(myJoueur == NULL) {
        pthread_mutex_unlock(&(sessionDeBase->mutex));
	pthread_mutex_unlock(&mutexAllSession);  
	sprintf(buf, "Erreur/Pseudo deja present/\n");
	write(sock, buf, strlen(buf));
	break;
      }
      indiquerConnexion(session, myJoueur); // Aux autres joueurs de la session
      pthread_mutex_unlock(&(sessionDeBase->mutex));
      pthread_mutex_unlock(&mutexAllSession);      
      break;
    case 2: /* SORT */
      printf("[Deconnexion] '%s'\n", myJoueur->pseudo);
      sort(listeJ, myJoueur);
      decoProprement = 1;
      break;
    case 3: // SOLUTION // Reflexion ou resolution
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
	printf("WHAT not reflexion or resolution... bad client...\n"); // Client mal fait
      }
      break;
    case 4: // ENCHERE
      enchere(session, myJoueur);
      break;
    case 6: // CHAT
      chat(session, myJoueur);
      break;
    case 7: // CREERSESSION
      session = creerSession(arg->listeSession, myJoueur);
      if(session == NULL) {
	sprintf(buf, "Erreur/Session deja existante/\n");
	write(sock, buf, strlen(buf));
	break;
      }
      printf("[Creation Session] name = %s, mdp = %s\n", session->nomSession, session->mdp);
      pthread_create(&tmp, NULL, gestionSession, session); // On lance le thread de gestion
      session->thread = tmp; 
      listeJ = session->liste;
      myJoueur = connex(sock, listeJ); 
      indiquerConnexion(session, myJoueur); // Au cas ou une connexion entre temps
      break;
    case 8: // CONNEXIONPRIVEE
      nomSession = strtok(NULL, "/");
      char *mdp = strtok(NULL, "/");
      printf("[Connexion privee] nomSession = %s, mdp = %s\n", nomSession, mdp);
      pthread_mutex_lock(&mutexAllSession); //////////////////////////////
      session = getSession(arg->listeSession, nomSession);
      if(session == NULL) {
	pthread_mutex_unlock(&mutexAllSession); ////////////////////////////////
	sprintf(buf, "Erreur/Session inexistante/\n");
	write(sock, buf, strlen(buf));
        break;
      }
      if(strcmp(mdp, session->mdp)) {
	pthread_mutex_unlock(&mutexAllSession);
	sprintf(buf, "Erreur/Mauvais mot de passe/\n");
	write(sock, buf, strlen(buf));
	break;
      }
      pthread_mutex_lock(&(session->mutex)); ///////////////:
      listeJ = session->liste;
      myJoueur = connex(sock, listeJ); 
      if(myJoueur == NULL) {
	pthread_mutex_unlock(&(session->mutex));
	pthread_mutex_unlock(&mutexAllSession);
	sprintf(buf, "Erreur/Pseudo deja present/\n");
	write(sock, buf, strlen(buf));
        break;
      }
      indiquerConnexion(session, myJoueur);
      pthread_mutex_unlock(&(session->mutex));
      pthread_mutex_unlock(&mutexAllSession);
      break;
    default:
      printf("[Requete inconnue] '%s'\n", req);
      break;
    }
  }

  if(myJoueur == NULL || myJoueur->pseudo == NULL) {
    printf("Un truc est null WARNING !!!\n");
    return NULL;
  }
  
  if(!decoProprement) { /* Cas ou fermeture socket sans SORT */
    printf("[Deconnexion] '%s'\n", myJoueur->pseudo);
    sort(listeJ, myJoueur);
  }

  pthread_mutex_lock(&mutexAllSession);
  pthread_mutex_lock(&(session->mutex));
  if(session->liste->nbJoueur == 0) { // Dernier joueur de la session Faudrait un mutex global
    pthread_mutex_unlock(&(session->mutex));
    printf("[Dernier joueur] destruction de la session '%s'\n", session->nomSession);
    pthread_cancel(session->thread); // Annuler le thread de gestion de la session
    if(session != sessionDeBase)
      suppSessionListe(arg->listeSession, session); // Si pas la public, on la supprime de la liste des session privees

    // Si sessionDeBase on le reinitialise pour pas devoir attendre la fin du tour lors des prochaines connexions
    if(session == sessionDeBase) {  // Prendre le mutex de sessionDeBase ou je sais pas quoi 
      printf("[Re-init session public]\n");
      sessionDeBase = createSession("Session_1", "");
      pthread_create(&tmp, NULL, gestionSession, sessionDeBase);
      sessionDeBase->thread = tmp;
    }
  }
  else {
    pthread_mutex_unlock(&(session->mutex));
  }
  pthread_mutex_unlock(&mutexAllSession);

  printf("Quit\n");
  return NULL;
}




// Fonction qui indique la connexion d'un joueur aux autres joueurs
void indiquerConnexion(Session *session, Joueur *myJoueur) {
  char buf[TBUF];
  char *plateau;
  ListeJoueurs *listeJ = session->liste;
  pthread_mutex_lock(&(session->liste->mutex));
  pthread_cond_signal(&(session->condConnexion)); // indique que y a 1 connexion
  pthread_mutex_unlock(&(session->liste->mutex));
  printf("[Connexion] : '%s', %d joueurs\n", myJoueur->pseudo, nbJoueurListe(listeJ));
  plateau = session->p->plateauString;
  sprintf(buf, "SESSION/%s/\n", plateau); // plateau provient de session normalement
  sendTo(buf, listeJ, myJoueur, 1); // Send le plateau
}



// CHAT/user/msg/
// Fonction de gestion du chat
void chat(Session *s, Joueur *myJoueur) {
  char buf[TBUF];
  char *message;

  strtok(NULL, "/"); /* Le nom */
  message = strtok(NULL, "/"); 
  if(message == NULL)
    sprintf(buf, "CHAT/%s//\n", pseudoJoueur(myJoueur));
  else
    sprintf(buf, "CHAT/%s/%s/\n", pseudoJoueur(myJoueur), message);
  sendToAll(buf, s->liste, myJoueur, 1);
}


// Fonction de gestion de la phase de resolution
void resolution(Session *s, Joueur *myJoueur) {
  char *deplacement;

  strtok(NULL, "/"); /* Le nom */
  deplacement = strtok(NULL, "/");
  if(deplacement == NULL) {
    printf("[WARNING] fonction resolution deplacement vide\n");
    return;
  }

  // Verifier si timer pas ecoule => trop tard
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


// Fonction de gestion de la phase d'enchere
void enchere(Session *s, Joueur *myJoueur) {
  int nbCoups;
  char *otherUser; // pseudo du joueur qui a deja propose la meme enchere
  char buf[TBUF];
  char buf2[TBUF];
  char *tmp;

  strtok(NULL, "/"); /* Le nom */
  tmp = strtok(NULL, "/");
  nbCoups = atoi(tmp);
  if(tmp == NULL) {
    printf("[WARNING] fonction enchere reponse vide\n");
    return;
  }


  pthread_mutex_lock(&(s->mutex));

  if(s->phase != ENCHERE) { // Pas encore la phase d'enchere
    printf("Doucement la phase d'enchere a pas commencee ...., gestion_client[%d]\n", __LINE__);
    sprintf(buf, "ECHEC/%s/\n", myJoueur->pseudo);
    sendTo(buf, s->liste, myJoueur, 0);
    pthread_mutex_unlock(&(s->mutex));
    return;
  }

  if((myJoueur->enchere == 0) || (myJoueur->enchere < -1) || ((myJoueur->enchere != -1) && (nbCoups >= myJoueur->enchere))) {
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
    sendToAllActif(buf2, s->liste, myJoueur, 0);
  }  

  pthread_mutex_unlock(&(s->mutex));
}


// return le joueur ayant deja propose cette valeur d'enchere
char *nbCoupsDejaPropose(int nbCoups, Joueur *myJoueur, Session *s) {
  Joueur *j = (s->liste)->j;
  while(j != NULL) {
    if( (j != myJoueur) && (j->enchere > 0) && (j->enchere == nbCoups))
      return j->pseudo;
    j = j->next;
  }
  return NULL;
}



// Fonction de gestion lors de la phase de reflexion
void trouve(Session *s, Joueur *myJoueur) {
  int nbCoup;
  char buf[TBUF];

  strtok(NULL, "/"); /* Le nom */
  char *tmp = strtok(NULL, "/");
  if(tmp == NULL) {
    printf("[WARNING] fonction trouve reponse vide\n");
    return;
  }
  nbCoup = atoi(tmp);

  pthread_mutex_lock(&(s->mutex));
  if(s->tempsReflexionFini) { // Au cas ou plrs j propose en meme temps, (et que le temps de traiter un le chrono est fini je crois
    printf("Attention : temps reflexion fini, gestion_client[%d]\n", __LINE__);
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
    sendToAllActif(buf, s->liste, myJoueur, 1);
    sprintf(buf, "TUASTROUVE/\n");
    sendTo(buf, s->liste, myJoueur, 1);
    pthread_cond_signal(&(s->condFinReflexion));
  }
  pthread_mutex_unlock(&(s->mutex));
}



// Fonction de gestion de deconnexion
void sort(ListeJoueurs *liste, Joueur *myJoueur) {
  char buf[TBUF];
  sprintf(buf, "DECONNEXION/%s/\n", myJoueur->pseudo);    
  suppJoueurListe(liste, myJoueur);
  detruire_joueur(myJoueur);
  sendToAll(buf, liste, NULL, 1); 
}


// Fonction de gestion lors de la connexion
Joueur *connex(int sock, ListeJoueurs *liste) {
  char buf[TBUF];
  char *pseudo = strtok(NULL, "/");

  pthread_mutex_lock(&(liste->mutex));
  if(pseudo_deja_present(liste, pseudo)) {
    pthread_mutex_unlock(&(liste->mutex));
    printf("[Pseudo deja present] %s\n", pseudo);
    return NULL;
  }
  pthread_mutex_unlock(&(liste->mutex));

  Joueur *myJoueur = create_joueur(pseudo, sock); 
  addJoueurListe(liste, myJoueur);
  /* Validation de la connexion a user */
  sprintf(buf, "BIENVENUE/%s/\n", pseudoJoueur(myJoueur));
  sendTo(buf, liste, myJoueur, 1);

  
  ///
  sprintf(buf, "COMPATIBLEAFFICHAGE/\n");
  sendTo(buf, liste, myJoueur, 1); // Pour dire que le serveur effectue un temps d'affichage

  
  /* Signalement connexion user aux autres joueurs */
  sprintf(buf, "CONNECTE/%s/\n", pseudoJoueur(myJoueur));
  sendToAll(buf, liste, myJoueur, 1); // All sauf myJoueur

  return myJoueur;
}



// Fonction de gestion lors de la creation d'une nouvelle session
Session *creerSession(ListeSession *l, Joueur *myJoueur) {
  char *nomSession = strtok(NULL, "/");
  char tmp[] = "";
  if(nomSession == NULL)
    return NULL;

  pthread_mutex_lock(&(l->mutex));
  if(session_deja_present(l, nomSession)) {
    pthread_mutex_unlock(&(l->mutex));
    return NULL;
  }
  pthread_mutex_unlock(&(l->mutex));
  
  printf("[New session] '%s'\n", nomSession);
  char *mdp = strtok(NULL, "/");
  if(mdp == NULL) 
    mdp = tmp;
  printf("[mdp] '%s'\n", mdp);

  Session *s = createSession(nomSession, mdp);
  addSessionListe(l, s);
  return s;
}










