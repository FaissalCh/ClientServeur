#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>
#include <liste_sessions.h>


void test(Session *s) {
  if(solutionAccepte("BBBHRDRHRD", s, NULL))
    printf("Good\n");
  else
    printf("Pas good\n");

}

int main() {
  
  int sock;
  int s_client;
  ArgThread *argThread;
  struct sockaddr_in exp;
  socklen_t fromlen = sizeof(exp);
  pthread_t tmp, tmp2;
  Session *sessionDeBase; //pour reinit la session de base si plus de joueurs
  ListeSession listeSessions; 
  initListeSession(&listeSessions);
  srand(time(NULL)); // Pour choisir un plateau aleatoirement
  sock = getSocketServeur(PORT);

  printf("[Creation du serveur] port : %d\n", PORT);
  sessionDeBase = createSession("Session_1", "");
  pthread_create(&tmp2, NULL, gestionSession, sessionDeBase);
  sessionDeBase->thread = tmp2;
  addSessionListe(&listeSessions, sessionDeBase);

  while(1) {
    s_client = accept(sock, (struct sockaddr *)&exp, &fromlen);
    if(s_client == -1) {
      perror("accept");
      exit(1);
    }
    printf("[MAIN] Debug %d\n", __LINE__);
    argThread = createArgThread(s_client, sessionDeBase, &listeSessions);
    if(argThread == NULL)
      printf("LOL\n");
    printf("[MAIN] Debug %d\n", __LINE__);
    sleep(3);
    pthread_create(&tmp, NULL, gestionClient, (void*)argThread);
    printf("[MAIN] Debug %d\n", __LINE__);
  }

  close(sock);
  printf("[Fermeture du serveur]\n");
  return 0;
}

