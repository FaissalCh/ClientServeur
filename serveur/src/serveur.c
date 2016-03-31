#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>
#include <liste_sessions.h>


/* void test(Session *s) { */
/*   if(solutionAccepte("BBBHRDRHRD", s, NULL)) */
/*     printf("Good\n"); */
/*   else */
/*     printf("Pas good\n"); */

/* } */

//pour reinit la session de base si plus de joueurs
Session *sessionDeBase; 
pthread_mutex_t mutexSessionBase = PTHREAD_MUTEX_INITIALIZER;

int main() {
  
  int sock;
  int s_client;
  ArgThread *argThread;
  struct sockaddr_in exp;
  socklen_t fromlen = sizeof(exp);
  pthread_t tmp;
  ListeSession listeSessions; // Liste des sessions privees
  srand(time(NULL)); 
  initListeSession(&listeSessions);
  sock = getSocketServeur(PORT);

  printf("[Creation du serveur] port : %d\n", PORT);
  sessionDeBase = createSession("Session_1", "");
  pthread_create(&tmp, NULL, gestionSession, sessionDeBase);
  sessionDeBase->thread = tmp;
  //addSessionListe(&listeSessions, sessionDeBase);

  while(1) {
    s_client = accept(sock, (struct sockaddr *)&exp, &fromlen);
    if(s_client == -1) {
      perror("accept");
      exit(1);
    }
    argThread = createArgThreadClient(s_client, &listeSessions);
    pthread_create(&tmp, NULL, gestionClient, (void*)argThread);
  }

  close(sock);
  printf("[Fermeture du serveur]\n");
  return 0;
}

