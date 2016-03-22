#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <types_jeu.h>
#include <tools.h>

int main() {
  
  int sock;
  int s_client;
  ArgThread *argThread;
  struct sockaddr_in exp;
  socklen_t fromlen = sizeof(exp);
  pthread_t tmp;
  Session *sessionDeBase;

  sock = getSocketServeur(PORT);
  sessionDeBase = createSession("Session_1", "");
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



