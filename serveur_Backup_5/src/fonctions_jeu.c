#include <tools.h>
#include <fonctions_jeu.h>

#define T_ENI 2048 // Faire petit calcul pour borner la taille
#define T_BUF 2048

char *nextVirgule(char *c);
Couleur numToCol(int i);

Plateau *getPlateau(int nb) {
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
  fgets(buf, T_BUF, f);
  robotsC = strdup(buf);
  robotsC[strlen(robotsC)-1] = '\0';
  fgets(buf, T_BUF, f);
  cibleC = strdup(buf);  
  cibleC[strlen(cibleC)-1] = '\0';
  fclose(f);

  res->murs = getMurs(mursC, &(res->nbMurs));
  setCible(cibleC, &(res->cible));
  robots = getRobots(robotsC);
  for(i=0 ; i<NB_ROBOTS ; i++) {
    res->robots[i].x = robots[i].x;
    res->robots[i].y = robots[i].y;
    res->robots[i].col = robots[i].col;
  }


  free(robots);
  return res;
}

Mur *getMurs(char *c, int *nb) { // FAUX !!!
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
    return Jaune;
  if(i == 2)
    return Vert;
  return Bleu;
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
  Robot rR, rB, rJ, rV; 
  Robot *rTmp;

  rR = getRobot(plateau, Rouge);
  rB = getRobot(plateau, Bleu);
  rJ = getRobot(plateau, Jaune);
  rV = getRobot(plateau, Vert);
      

  if(nbDeplacement != myJoueur->enchere) // Different de l'enchere
    return 0;
  
  for(i=0 ; i<nbDeplacement ; i++) {
    d = tabDep[i];
    rTmp = (d.col == Rouge) ? &rR : ( (d.col == Bleu) ? &rB : ((d.col == Jaune) ? &rJ : &rV));
    deplacement(plateau, &d, rTmp);
  }  
  

  // Faire le symetrique aussi
  int xCible = plateau->cible.x;
  int yCible = plateau->cible.y;
  if( (rR.x == xCible && rR.y == yCible) ||
      (rB.x == xCible && rB.y == yCible) ||
      (rJ.x == xCible && rJ.y == yCible) ||
      (rV.x == xCible && rV.y == yCible) )
    return 1;
  return 0;
}


void deplacement(Plateau *p, Deplacement *d, Robot *r) {
  Direction dir = d->dir;
  int *champ = (dir == H || dir == B) ? &r->y : &r->x;
  int incr = (dir == H || dir == G) ? -1 : 1;
  while(!isObstacle(p, r->x, r->y, dir))
    champ += incr;
}

// Verifier retourne bien copie
Robot getRobot(Plateau *plateau, Couleur col) { // Faire en set
  int i;
  Robot res;
  for(i=0 ; i<NB_ROBOTS ; i++) {
    if(plateau->robots[i].col == col) {
      res = plateau->robots[i];
      break;
    }
  }
  return res;
}

Deplacements *getDeplacements(char *sol) { // OK ?
  char col, dir;
  int i;
  int cpt = 0;
  int nbDep = strlen(sol);
  Deplacement *d = (Deplacement *)malloc(nbDep*sizeof(Deplacement));
  Deplacements *res = (Deplacements *)malloc(sizeof(Deplacements));
  if(res == NULL) { perror("malloc");exit(0); }

  for(i=0 ; i<nbDep-1 ; i+=2) {
    col = sol[i];
    dir = sol[i+1];
    if(d == NULL) { perror("malloc");exit(0); }
    d[cpt].col = getCol(col);
    d[cpt].dir = getDir(dir);
    cpt++;
  }

  res->tabDep = d;
  res->nbDeplacement = nbDep;
  return res;
}




int isObstacle(Plateau *p, int x, int y, Direction d) { // OK ?
  Mur *murs;
  int nbMurs;
  int i;

  if( (x == 0 && d == G) || (x == X_PLATEAU-1 && d == D) ||
      (y == 0 && d == H) || (y == Y_PLATEAU-1 && d == B) )
    return 1;
  
  nbMurs = p->nbMurs;
  murs = p->murs;
  for(i=0 ; i<nbMurs ; i++) 
    if(murs[i].x == x && murs[i].y == y && murs[i].d == d)
      return 1;

  Robot *r = p->robots;
  int newX = (d == D) ? x+1 : ((d == G) ? x-1 : x);
  int newY = (d == H) ? y-1 : ((d == B) ? y+1 : y);
  for(i=0 ; i<NB_ROBOTS ; i++) {
    if( (newX == r[i].x) &&  (newY == r[i].y) )
      return 1;
  }

  return 0;
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
