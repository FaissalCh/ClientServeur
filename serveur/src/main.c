#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <types_jeu.h>
#include <serveur.h>

ListeJoueurs listeJ;

int main() {
  
  int sock;
  int s_client;
  int *sTmp;
  struct sockaddr_in exp;
  socklen_t fromlen = sizeof(exp);
  pthread_t tmp;

  sock = getSocketServeur(PORT);
  printf("[Creation du serveur] port : %d\n", PORT);

  initListeJoueurs(&listeJ);

  while(1) {
    s_client = accept(sock, (struct sockaddr *)&exp, &fromlen);
    if(s_client == -1) {
      perror("accept");
      exit(1);
    }
    sTmp = (int *)malloc(sizeof(int)); /* Warning sinon */
    *sTmp = s_client;
    pthread_create(&tmp, NULL, gestionClient, (void*)sTmp);
  }


  close(sock);
  printf("[Fermeture du serveur]\n");
  return 0;
}



