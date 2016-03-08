
#ifndef TYPES_JEU_H
#define TYPES_JEU_H

#include <sys/types.h>
#include <sys/socket.h>

/* Grille de 16x16 cases */
#define X_PLATEAU 16
#define Y_PLATEAU 16


/* Couleurs des robots */
typedef enum Couleur Couleur;
enum Couleur { Rouge, Jaune, Vert, Bleu };

typedef enum Direction Direction;
enum Direction { H, B, D, G};

typedef struct _Robot {
  Couleur col;
  int x;
  int y;
} Robot;

typedef struct _Cible {
  int x;
  int y;
} Cible;


typedef union _Case {
  Robot r;
  Cible c;
  int nbMur;
  Direction *murs;
} Case;


/* Structure qui definie un plateau de jeu */
typedef struct _Plateau {
  /* Mutex mutex; */
  Case grille[X_PLATEAU][Y_PLATEAU];
  int nbTour;
  Cible cible;
} Plateau;


#endif 
