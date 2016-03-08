#ifndef TYPES_JEU_H
#define TYPES_JEU_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

/* Gestion socket */
#define PORT 2016

#define MAX_JOUEUR 30

#define T_PSEUDO 124 /* renommer T_BUF */

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



//////////////////////////////////////////

/* Structure qui definie un joueur */
typedef struct _Joueur {
  pthread_mutex_t mutex;
  char pseudo[T_PSEUDO];
  int socket;
  int actif;
  int score;
  struct _Joueur *next;
} Joueur;

/* Liste des joueurs */
typedef struct _ListeJoueurs {
  Joueur *j;
  int nbJoueur;
  pthread_mutex_t mutex;
} ListeJoueurs;


/* Session pour amelioration */
typedef struct _Session {
  int id; // Voir si utile
  char nomSession[T_PSEUDO];
  char mdp[T_PSEUDO];
  Plateau p;
  ListeJoueurs *liste; 
} Session;

/* Argument des threads */
typedef struct _ArgThread {
  int socket;
  Session *session;
} ArgThread;



#endif 
