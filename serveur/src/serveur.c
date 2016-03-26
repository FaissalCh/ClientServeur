#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <types_jeu.h>
#include <tools.h>
#include <fonctions_jeu.h>


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
  pthread_t tmp;
  Session *sessionDeBase;

  sock = getSocketServeur(PORT);
  sessionDeBase = createSession("Session_1", ""); // Dans create session choisir un plateau aleatoire et mettre les enigmes dedans ....
  sessionDeBase->p = getPlateau(1);
  //affPlateau(sessionDeBase->p);
  test(sessionDeBase);



  printf("[Creation du serveur] port : %d\n", PORT);

  
  pthread_create(&tmp, NULL, gestionSession, sessionDeBase);
  while(1) {
    s_client = accept(sock, (struct sockaddr *)&exp, &fromlen);
    if(s_client == -1) {
      perror("accept");
      exit(1);
    }
    argThread = createArgThread(s_client, sessionDeBase);
    pthread_create(&tmp, NULL, gestionClient, (void*)argThread);
  }


  close(sock);
  printf("[Fermeture du serveur]\n");
  return 0;
}



