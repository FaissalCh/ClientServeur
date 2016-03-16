#include <tools.h>

#include <pthread.h>
#include <unistd.h>


/* -------- Fonction de gestion de section -------- */

Session *createSession(char *nomSession, char *mdp) {
  // Pas de gestion de l'id pour l'instant
  Session *s = (Session *)malloc(sizeof(Session));
  if(s == NULL) {
    perror("malloc");
    exit(1);
  }
  s->liste = (ListeJoueurs *)malloc(sizeof(ListeJoueurs));
  if(s->liste == NULL) {
    perror("malloc");
    exit(1);
  }
  pthread_mutex_init(&(s->mutex), NULL);
  if(pthread_cond_init(&(s->condConnexion), NULL)) {
    perror("pthread_cond_init");
    exit(1);
  }
  if(pthread_cond_init(&(s->condFinReflexion), NULL)) {
    perror("pthread_cond_init");
    exit(1); 
  }

  strncpy(s->nomSession, nomSession, T_PSEUDO);
  strncpy(s->mdp, mdp, T_PSEUDO);
  initListeJoueurs(s->liste);
  s->nbTour = 0;
  //s->tourEnCours = 0;
  return s;
}

ArgThread *createArgThread(int socket, Session *session) {
  ArgThread *arg = (ArgThread *)malloc(sizeof(ArgThread));
  if(arg == NULL) {
    perror("malloc");
    exit(1);
  }
  arg->socket = socket;
  arg->session = session;
  return arg;
}



/* -------- Fonction sur la liste chainee -------- */

void initListeJoueurs(ListeJoueurs *liste) {
  liste->j = NULL;
  liste->nbJoueur = 0;
  pthread_mutex_init(&(liste->mutex), NULL);
}

int nbJoueurListe(ListeJoueurs *l) {
  int res;
  pthread_mutex_lock(&(l->mutex));
  res = l->nbJoueur;
  pthread_mutex_unlock(&(l->mutex));
  return res;
}

int nbJoueurActifListe(ListeJoueurs *l) {
  int res = 0;
  pthread_mutex_lock(&(l->mutex));
  Joueur *cur = l->j;
  while(cur != NULL) {
    if(cur->playSessionEnCours)
      res++;
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return res;
}


void addJoueurListe(ListeJoueurs *l, Joueur *j) {
  pthread_mutex_lock(&(l->mutex));
  if(l->nbJoueur == 0) {
    l->j = j;
  }
  else {
    j->next = l->j;
    l->j = j;
  }
  l->nbJoueur++;
  pthread_mutex_unlock(&(l->mutex));
}

int suppJoueurListe(ListeJoueurs *l, Joueur *j) {
  Joueur *cur, *prec;
  int ret = 0;
  pthread_mutex_lock(&(l->mutex));

  prec = NULL;
  cur = l->j;
  while(cur != NULL) {
    if(cur == j) {
      if(prec == NULL) // 1er de la liste
	l->j = j->next;
      else 
	prec->next = j->next;
      l->nbJoueur--;
      ret = 1;
      break;
    }
    prec = cur;
    cur = cur->next;
  }
  pthread_mutex_unlock(&(l->mutex));
  return ret;
}



/* ---------------- Fonction sur les joueurs ------------- */
Joueur *create_joueur(char *pseudo, int socket) {
  Joueur *j_tmp = (Joueur *)malloc(sizeof(Joueur));
  if(j_tmp == NULL) {
    perror("malloc");
    exit(1);
  }
  strcpy(j_tmp->pseudo, pseudo);
  j_tmp->socket = socket;
  j_tmp->next = NULL;
  j_tmp->playSessionEnCours = 0;
  j_tmp->score = 0;
  pthread_mutex_init(&(j_tmp->mutex), NULL);
  return j_tmp;
}

void detruire_joueur(Joueur *j) {
  pthread_mutex_destroy(&(j->mutex));
  //close(j->socket);
  //free(j);
}


char *pseudoJoueur(Joueur *j) {
  // Change jamais, pas besoin de lock
  return j->pseudo;
}


/* ------------- Fonction de hashage du protocole ------------- */

int hash_protocole(char *req) {
  if(!strcmp(req, "CONNEX")) 
    return 1;
  else if(!strcmp(req, "SORT")) 
    return 2;
 else if(!strcmp(req, "TROUVE")) 
   return 3;
 else if(!strcmp(req, "ENCHERE")) 
   return 4;
 else if(!strcmp(req, "SOLUTION")) 
   return 5;
 else 
   return -1;
}



/* -------- Fonction sur socket -------- */

void sendTo(char *buf, ListeJoueurs *liste, Joueur *j) {
  if(liste != NULL)
    pthread_mutex_lock(&(liste->mutex));
  write(j->socket, buf, strlen(buf));
  if(liste != NULL)
    pthread_mutex_unlock(&(liste->mutex));
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
    sendTo(buf, NULL, jTmp);
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
