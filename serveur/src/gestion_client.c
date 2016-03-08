#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <types_jeu.h>
#include <tools.h>


#define TBUF 256
Joueur *connex(int sock, ListeJoueurs *liste);
void sort(ListeJoueurs *liste, Joueur *myJoueur);



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
      printf("[Connexion] '%s'\n", myJoueur->pseudo);
      break;
    case 2: /* SORT */
      printf("[Deconnexion] '%s'\n", myJoueur->pseudo);
      sort(listeJ, myJoueur);
      decoProprement = 1;
      break;
    case 3: // TROUVE
      
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




/* Recupere le pseudo grace a strtok */
/* Faudra gerer si plusieurs joueurs ont le meme nom */
Joueur *connex(int sock, ListeJoueurs *liste) {
  char buf[TBUF];
  Joueur *myJoueur = create_joueur(strtok(NULL, "/"), sock); 
  addJoueurListe(liste, myJoueur);
  printf("[Connexion] : '%s'\n", myJoueur->pseudo);

  /* Validation de la connexion a user */
  sprintf(buf, "BIENVENUE/%s/\n", pseudoJoueur(myJoueur));
  // Reflechir si pas mettre (un)lock dans sendTo directement
  pthread_mutex_lock(&(liste->mutex)); 
  sendTo(buf, myJoueur);
  pthread_mutex_unlock(&(liste->mutex));
  /* Signalement connexion user aux autres joueurs */
  sprintf(buf, "CONNECTE/%s/\n", pseudoJoueur(myJoueur));
  sendToAll(buf, liste, myJoueur); // All sauf myJoueur
  return myJoueur;
}










