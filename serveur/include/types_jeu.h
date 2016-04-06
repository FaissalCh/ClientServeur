#ifndef TYPES_JEU_H
#define TYPES_JEU_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 2016
#define MAX_JOUEUR 30
#define T_PSEUDO 124 /* renommer T_BUF */

#define NB_PLATEAU 2

#define SCORE_OBJECTIF 20

#define TEMPS_ATTENTE_JOUEURS 5 // secondes
#define TEMPS_REFLEXION 5*60 // minutes
#define TEMPS_ENCHERE 30*1 // minutes
#define TEMPS_RESOLUTION 1*60 // minute


/* Grille de 16x16 cases */
#define X_PLATEAU 16
#define Y_PLATEAU 16

#define NB_ROBOTS 4

#define DIR_PLATEAUX "plateaux/"

/* Couleurs des robots */
typedef enum Couleur Couleur;
enum Couleur { Rouge, Jaune, Vert, Bleu };

typedef enum Direction Direction;
enum Direction { H, B, D, G};


typedef enum Phase Phase;
enum Phase { REFLEXION, ENCHERE, RESOLUTION };

typedef struct _Robot {
  Couleur col;
  int x;
  int y;
} Robot;

typedef struct _Cible {
  int x;
  int y;
} Cible;

typedef struct _Mur {
  int x;
  int y;
  Direction d;
} Mur;


typedef union _Case {
  Robot r;
  Cible c;
  int nbMur;
  Direction *murs;
} Case;


/* Structure qui definie un plateau de jeu */
typedef struct _Enigme {
  char *enigmeString;
  Robot robots[4];
  Cible cible;
  Couleur colRobot;
} Enigme;

typedef struct _Plateau {
  char *plateauString;
  Mur *murs; // tableau
  int nbMurs;
  Enigme enigme;
  
  //////////////
  int nbEnigme;
  Enigme *tabEnigme;
  int curEnigme;
} Plateau;


typedef struct _Deplacement {
  Couleur col;
  Direction dir;
} Deplacement;

typedef struct _Deplacements {
  Deplacement *tabDep; // tab
  int nbDeplacement;
} Deplacements;




//////////////////////////////////////////

/* Structure qui definie un joueur */
typedef struct _Joueur {
  char pseudo[T_PSEUDO];
  int socket;
  int playSessionEnCours;
  int actif; // si c'est le j actif lors de la resolution
  int enchere;
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
  pthread_mutex_t mutex;
  pthread_t thread;
  char nomSession[T_PSEUDO];
  char mdp[T_PSEUDO];
  Plateau *p;
  int nbTour;
  //int tourEnCours; // ??
  ListeJoueurs *liste; 
  Phase phase;
  int finEnchere; 
  char deplacementCur[2056];

  pthread_t timerThread; // peut etre l'erreur vient de la

  // Phase resolution
  int timerResolutionFini;
  int timerOutResolution;
  pthread_cond_t condFinResolution; ////////////

  // Phase reflexion
  int tempsReflexionFini; 
  pthread_cond_t condFinReflexion; 
  int timerOut; // boolean qui dit si fin reflexion a cause du timer ou a cause reponse d'un joueur
  pthread_cond_t condConnexion; // signal quand connexion, pour debuter quand au moins 2 joueurs

  struct _Session *next;
} Session;

typedef struct _ListeSessions {
  Session *s;
  int nbSession;
  pthread_mutex_t mutex;
} ListeSession;


/* Argument des threads */
typedef struct _ArgThread {
  int socket;
  ListeSession *listeSession;
  Session *sessionDeBase;
} ArgThread;

// Pour le timer, faire un fichier a part pour timer...
typedef struct _ArgTimer {
  int temps;
  int *flag;
  pthread_cond_t *cond;
  pthread_mutex_t *mut;
} ArgTimer;



#endif 
