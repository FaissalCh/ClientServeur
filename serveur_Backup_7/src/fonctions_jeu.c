#include <tools.h>
#include <fonctions_jeu.h>


#include <unistd.h> // A supp

#define T_ENI 2048 // Faire petit calcul pour borner la taille
#define T_BUF 2048

// Peut etre faire getPlateau.c
char *nextVirgule(char *c);
Couleur numToCol(int i);
char directionToChar(Direction d);
Cible getCible(char *c);
char colToChar(Couleur c);
char *mursToString(Mur *murs, int nbMurs);
char *enigmeToString(Enigme *e);


Plateau *getPlateau(int nb) { // A free
  char buf[T_BUF];
  int i;
  int nbEnigme;
  Robot *robots;
  char *mursC, *robotsC, *cibleC;
  Plateau *res = (Plateau *)malloc(sizeof(Plateau));
  if(res == NULL) {
    perror("malloc");
    exit(1);
  }
  sprintf(buf, "%s/plateau%d", DIR_PLATEAUX, nb);
  FILE*f = fopen(buf, "r");
  if(f == NULL) {
    perror("fopen");
    exit(1);
  }

  fgets(buf, T_BUF, f);
  mursC = strdup(buf);
  mursC[strlen(mursC)-1] = '\0';
  fgets(buf, T_BUF, f);
  nbEnigme = atoi(buf);
  // Faut aussi charger toutes les enigmes dans un tableau
  fgets(buf, T_BUF, f);
  robotsC = strdup(buf);
  robotsC[strlen(robotsC)-1] = '\0';
  fgets(buf, T_BUF, f);
  cibleC = strdup(buf);  
  cibleC[strlen(cibleC)-1] = '\0';
  fclose(f);

  res->nbEnigme = nbEnigme;  
  res->murs = getMurs(mursC, &(res->nbMurs));
  robots = getRobots(robotsC);
  for(i=0 ; i<NB_ROBOTS ; i++) {
    res->enigme.robots[i].x = robots[i].x;
    res->enigme.robots[i].y = robots[i].y;
    res->enigme.robots[i].col = robots[i].col;
    //printf("[Col = %c, x = %d, y = %d]\n", colToChar(robots[i].col), robots[i].x, robots[i].y);
  }
  res->enigme.cible = getCible(cibleC);
  res->plateauString = mursToString(res->murs, res->nbMurs);
  res->enigme.enigmeString = enigmeToString(&(res->enigme));
  
  free(robots);
  return res;
}

char *enigmeToString(Enigme *e) {
  char col = 'R'; ///////////////////////////////////////////////// ?????????????????????,
  char *res = malloc(sizeof(char)*NB_ROBOTS*5+NB_ROBOTS*sizeof(char)*10); // Bcp au cas ou
  if(res == NULL) {
    perror("malloc");
    exit(0);
  }
  Robot *rR, *rB, *rJ, *rV;
  rR = getRobot(e->robots, Rouge);
  rB = getRobot(e->robots, Bleu);
  rJ = getRobot(e->robots, Jaune);
  rV = getRobot(e->robots, Vert);
  
  sprintf(res, "(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%c)", rR->x, rR->y, rB->x, rB->y, rJ->x, rJ->y, rV->x, rV->y, e->cible.x, e->cible.y, col);
 
  return res;
}

char *mursToString(Mur *murs, int nbMurs) { // A free
  int i;
  char buf[T_BUF];
  char *res = malloc(sizeof(char)*nbMurs*5+nbMurs*sizeof(char)*10); // Bcp au cas ou
  if(res == NULL) {
    perror("malloc");
    exit(0);
  }
  res[0]='\0'; 
  for(i=0 ; i<nbMurs ; i++) {
    sprintf(buf, "(%d,%d,%c)", murs[i].x, murs[i].y, directionToChar(murs[i].d));
    strcat(res, buf);
  }
  return res;
}


Mur *getMurs(char *c, int *nb) { // A free
  int i;
  *nb = getNbMurs(c);
  Mur *tabMur = (Mur *)malloc(*nb * sizeof(Mur));
  if(tabMur == NULL) {
    perror("malloc");
    exit(0);
  }
  for(i=0 ; i<*nb ; i++) { // Faux
    tabMur[i].x = atoi(c);
    c = nextVirgule(c);
    tabMur[i].y = atoi(c);
    c = nextVirgule(c);
    tabMur[i].d = getDir(*c);
    c = nextVirgule(c);
  }
  return tabMur;
}



Cible getCible(char *c) {// Faux
  Cible cible;
  cible.x = atoi(c); 
  c = nextVirgule(c);
  cible.y = atoi(c);
  return cible;
}
void setCible(char *c, Cible *cible) {// Faux
  cible->x = atoi(c); 
  c = nextVirgule(c);
  cible->y = atoi(c);
}

Robot *getRobots(char *c) { // Faux
  int i;
  Robot *robots = (Robot *)malloc(NB_ROBOTS*sizeof(Robot));
  if(robots == NULL) {perror("malloc");exit(1);}
  for(i=0 ; i<NB_ROBOTS ; i++) {
    robots[i].x = atoi(c);
    c = nextVirgule(c);
    robots[i].y = atoi(c);
    robots[i].col = numToCol(i);
    c = nextVirgule(c);
  }
  return robots;
}

Couleur numToCol(int i) {
  if(i == 0)
    return Rouge;
  if(i == 1)
    return Bleu;
  if(i == 2)
    return Jaune;
  return Vert;
}

char *nextVirgule(char *c) {
  while(*c != ',' && *c != '\0')
    c++;
  c++;
  return c;
}


int getNbMurs(char *murs) {
  int cpt=0;
  while(*murs != '\0') 
    if(*murs++ == ',')
      cpt++;
  return (cpt+1)/3;
}


// Verifier que modifie pas etat initial
int solutionAccepte(char *sol, Session *s, Joueur *myJoueur) {
  Plateau *plateau = s->p;
  Deplacement d;
  Deplacements *dep = getDeplacements(sol);
  Deplacement *tabDep = dep->tabDep;
  int nbDeplacement = dep->nbDeplacement;
  int i;
  Robot *rR, *rB, *rJ, *rV; 
  Robot *rTmp;

  rR = getRobot(plateau->enigme.robots, Rouge);
  rB = getRobot(plateau->enigme.robots, Bleu);
  rJ = getRobot(plateau->enigme.robots, Jaune);
  rV = getRobot(plateau->enigme.robots, Vert);
      
  ////////////////////////////////////////////// COM
  if(myJoueur != NULL) ///////////////////// SUPPRIMER CE IF !!!!!!!!!!!!!!!!!!!! JUSTE POUR LE TEST
    if(nbDeplacement > myJoueur->enchere) // Supperieur a l'enchere A DECOM !!!!!!!!
      return 0;
  
  for(i=0 ; i<nbDeplacement ; i++) {
    d = tabDep[i];
    rTmp = (d.col == Rouge) ? rR : ( (d.col == Bleu) ? rB : ((d.col == Jaune) ? rJ : rV));
    deplacement(plateau, &d, rTmp);
    if(rTmp->x == plateau->enigme.cible.x && rTmp->y == plateau->enigme.cible.y)
      return 1;
  }  
  
  /* int xCible = plateau->enigme.cible.x; */
  /* int yCible = plateau->enigme.cible.y; */
  /* if( (rR->x == xCible && rR->y == yCible) || */
  /*     (rB->x == xCible && rB->y == yCible) || */
  /*     (rJ->x == xCible && rJ->y == yCible) || */
  /*     (rV->x == xCible && rV->y == yCible) ) */
  /*   return 1; */
  return 0;
}


void deplacement(Plateau *p, Deplacement *d, Robot *r) {
  Direction dir = d->dir;
  int *champ = (dir == H || dir == B) ? &r->y : &r->x;
  int incr = (dir == H || dir == G) ? -1 : 1;
  while(!isObstacle(p, r->x, r->y, dir)) {
    /////////////////////////////////////////
    /* sleep(1); */
    /* system("clear"); */
    /* printf("---------------------------\n"); */
    /* printf("---------------------------\n"); */
    /* affPlateau(p); */
    /////////////////////////////////////:::

    // If sur la cible break !!!!!!!!!!!!!!!!!!!!!!!!!
    if(r->x == p->enigme.cible.x && r->y == p->enigme.cible.y)
      break;
    *champ += incr;
  }    
  //////////////////////
  /* sleep(1); */
  /* system("clear"); */
  /* printf("---------------------------\n"); */
  /* printf("---------------------------\n"); */
  /* affPlateau(p); */
  /////////////////////
}


// Faire le symetrique aussi
int isObstacle(Plateau *p, int x, int y, Direction d) { // OK ?
  Mur *murs;
  int nbMurs;
  int i;
  int xSym, ySym;
  Direction dSym;
  dSym = (d == G) ? D : ((d == D) ? G : ((d == H) ? B : H)); 
  xSym = (d == D) ? x+1 : ((d == G) ? x-1 : x);
  ySym = (d == H) ? y-1 : ((d == B) ? y+1 : y);

  // Les bords du plateau
  if( (x == 0 && d == G) || (x == X_PLATEAU-1 && d == D) ||
      (y == 0 && d == H) || (y == Y_PLATEAU-1 && d == B) ) {
    return 1;
  }
  
  nbMurs = p->nbMurs;
  murs = p->murs;
  for(i=0 ; i<nbMurs ; i++) {
    if(murs[i].x == x && murs[i].y == y && murs[i].d == d) {
      return 1;
    }
    if(murs[i].x == xSym && murs[i].y == ySym && murs[i].d == dSym) {// Verifier 
      return 1;
    }
  }

  // Collision avec autre robot
  Robot *r = p->enigme.robots;
  int newX = (d == D) ? x+1 : ((d == G) ? x-1 : x);
  int newY = (d == H) ? y-1 : ((d == B) ? y+1 : y);
  for(i=0 ; i<NB_ROBOTS ; i++) {
    if( (newX == r[i].x) &&  (newY == r[i].y) ) {
      return 1;
    }
  }

  return 0;
}

// Verifier retourne bien copie
Robot *getRobot(Robot *robots, Couleur col) { // Faire en set
  int i;
  Robot *res;
  for(i=0 ; i<NB_ROBOTS ; i++) {
    if(robots[i].col == col) {
      res = &(robots[i]);
      break;
    }
  }
  return res;
}


Deplacements *getDeplacements(char *sol) { // OK ?
  char col, dir;
  int i;
  int cpt = 0;
  int nbCar = strlen(sol);
  Deplacement *d = (Deplacement *)malloc((nbCar/2)*sizeof(Deplacement));
  Deplacements *res = (Deplacements *)malloc(sizeof(Deplacements));
  if(res == NULL) { perror("malloc");exit(0); }

  for(i=0 ; i<nbCar-1 ; i+=2) {
    col = sol[i];
    dir = sol[i+1];
    if(d == NULL) { perror("malloc");exit(0); }
    d[cpt].col = getCol(col);
    d[cpt].dir = getDir(dir);
    cpt++;
  }

  res->tabDep = d;
  res->nbDeplacement = (nbCar/2);
  return res;
}


// Mettre dans session.(c|h)
char *getBilanSession(Session *s) { // OK ?
  // Prendre mutex session, pas besoin je crois
  int nbTour = s->nbTour;
  ListeJoueurs *l = s->liste;
  char res[T_ENI];
  char tmp[T_ENI];
  Joueur *j;
  
  // Mutex liste
  pthread_mutex_lock(&(l->mutex));
  j = l->j;
  sprintf(res, "%d", nbTour);
  while(j != NULL) {
    sprintf(tmp, "(%s,%d)", j->pseudo, j->score);
    strcat(res, tmp);
    j = j->next;
  }
  pthread_mutex_unlock(&(l->mutex)); // Lacher mutex
  return strdup(res);
}


Couleur getCol(char c) { // OK ?
  if(c == 'R')
    return Rouge;
  else if(c == 'V')
    return Vert;
  else if(c == 'B')
    return Bleu;
  else //(c == 'J')
    return Jaune;
}

Direction getDir(char d) { // OK ?
  if(d == 'H')
    return H;
  else if(d == 'B')
    return B;
  else if(d == 'D')
    return D;
  else //(d == 'G')
    return G;
}

////////////////////////////////////////////////////
char directionToChar(Direction d) { // Faut free apres
  switch(d) {
  case H:
    return 'H';
    break;
  case B:
    return 'B';
    break;
  case D:
    return 'D';
    break;
  case G:
    return 'G';
    break;
  default:
    return -1;
    break;
  }
}

char colToChar(Couleur c) {
  switch(c) {
  case Rouge:
    return 'R';
    break;
  case Jaune:
    return 'J';
    break;
  case Bleu:
    return 'B';
    break;
  case Vert:
    return 'V';
    break;
  default:
    return -1;
    break;
  }

}
