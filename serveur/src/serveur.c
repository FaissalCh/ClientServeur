#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <serveur.h>
#include <liste_joueurs.h>

#define TBUF 256


/* Mais en vrai sock est dans myJoueur pas besoin de parametre
 dans les fonctions */

void sort(ListeJoueurs *liste);



extern ListeJoueurs listeJ;
Joueur *myJoueur = NULL; /* Joueur du thread courant */
//int play;

void *gestionClient(void *arg) {
  int sock = *((int*)arg);
  int nbRead;
  char buf[TBUF];
  char *req;
  int cpt=0;
  int decoProprement = 0;
  free(arg); 
  //play = 1;


  while(/* play && */ (cpt<TBUF) && (nbRead=read(sock, &buf[cpt++], 1)) > 0) {
    if(buf[cpt-1] != '\n') 
      continue;
    buf[cpt-1] = '\0';
    cpt = 0;
  


    printf("[Receive] : %s\n", buf); 
    req = strtok(buf, "/");
    if(!strcmp(req, "CONNEX")) {
      connex(sock, &listeJ); 
    }
    else if(!strcmp(req, "SORT")) {
      sort(&listeJ);
      decoProprement = 1;
      break;
    }
    else {
      printf("[Requete inconnue] '%s'\n", req);
    }
  }

  if(!decoProprement) {
    printf("ICI\n");
    sort(&listeJ);
  }


  return NULL;
}

void sort(ListeJoueurs *liste) {
  char buf[TBUF];
  char *pseudo = pseudoJoueur(myJoueur);

  printf("[Deconnexion] '%s'\n", pseudo);
  sprintf(buf, "SORTI/%s/\n", pseudo);    
  suppJoueurListe(liste, myJoueur);
  printf("NbJoueur = %d\n", nbJoueurListe(&listeJ));
  detruire_joueur(myJoueur);
  printf("DEBUG %d\n", __LINE__);
  sendToAll(buf, liste, NULL); // All sauf myJoueur
}








/* Recupere le pseudo grace a strtok */
/* Faudra gerer si plusieurs joueurs ont le meme nom */
void connex(int sock, ListeJoueurs *liste) {
  char buf[TBUF];
  myJoueur = create_joueur(strtok(NULL, "/"), sock); 
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

}








void sendTo(char *buf, Joueur *j) {
  //pthread_mutex_lock(&(j->mutex));
  write(j->socket, buf, strlen(buf));
  //pthread_mutex_unlock(&(j->mutex));
}

void sendToAll(char *buf, ListeJoueurs *liste, Joueur *saufMoi) {
  Joueur *jTmp;
  pthread_mutex_lock(&(liste->mutex));
  jTmp = liste->j;
  while(jTmp != NULL) {
    if(jTmp == saufMoi) { // Si pas de saufMoi on le met a NULL
      jTmp = jTmp->next; // jTmp sera jamais egal
      continue; 
    }
    sendTo(buf, jTmp);
    jTmp = jTmp->next;
  }
  pthread_mutex_unlock(&(liste->mutex));
}


int getSocketServeur(int port) {
  int s_XTMP;
  struct sockaddr_in sin;
  
  /* Creation socket */
  s_XTMP = socket(AF_INET, SOCK_STREAM, 0);
  if(s_XTMP < 0) {
    perror("socket");
    exit(2);
  }
  /* Nommage de la socket */
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;  
  
  if(bind(s_XTMP, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("bind");
    exit(3);
  }
  
  /* Creation de la file d'attente en indiquant le nombre max de client */
  listen(s_XTMP, 8); //////////////////////////// 20 ???
  
  return s_XTMP;
}
