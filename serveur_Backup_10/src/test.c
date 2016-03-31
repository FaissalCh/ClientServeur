#include <stdio.h>
#include <stdlib.h>
#include "../include/fonctions_jeu.h"


int main() {
  
  Plateau *p = getPlateau(1);
  
  printf("[Murs] = %s", p->plateauString);
  printf("[Enigme] = %s", p->enigme.enigmeString);


  return 0;
}
