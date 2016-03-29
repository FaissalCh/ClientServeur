#ifndef TYPES_JEU_H
#define TYPES_JEU_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 2016
#define MAX_JOUEUR 30
#define T_PSEUDO 124 /* renommer T_BUF */

#define SCORE_OBJECTIF 30

#define TEMPS_ATTENTE_JOUEURS 3 // secondes
#define TEMPS_REFLEXION 10 //5*60 // minutes
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
  pthread_mutex_t mutex; // A retirer 
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
  int id; // Voir si utile
  pthread_mutex_t mutex;

  
  char nomSession[T_PSEUDO];
  char mdp[T_PSEUDO];
  Plateau *p;
  int nbTour;
  //int tourEnCours; // ??
  ListeJoueurs *liste; 

  Phase phase;
  int finEnchere; ////////

  char deplacementCur[2056];

  pthread_t timerThread; // peut etre l'erreur vient de la

  // Phase resolution
  int timerResolutionFini;
  int timerOutResolution;
  pthread_cond_t condFinResolution; ////////////

  // Phase reflexion
  int tempsReflexionFini; 
  int timerOut; // boolean qui dit si fin reflexion a cause du timer ou a cause reponse d'un joueur
  pthread_cond_t condFinReflexion; ////////////

  // Connexion
  pthread_cond_t condConnexion; // signal quand connexion, pour debuter quand au moins 2 joueurs

} Session;




/* Argument des threads */
typedef struct _ArgThread {
  int socket;
  Session *session;
} ArgThread;

// Pour le timer, faire un fichier a part pour timer...
typedef struct _ArgTimer {
  int temps;
  int *flag;
  pthread_cond_t *cond;
  pthread_mutex_t *mut;
} ArgTimer;



#endif 
