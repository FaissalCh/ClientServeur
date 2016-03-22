#include <tools.h>
#include <fonctions_jeu.h>

#define T_ENI 2048 // Faire petit calcul pour borner la taille
#define T_BUF 2048

Plateau *getPlateau(int nb) {
  Mur *murs;
  Cible cible;
  Robot robots[4];

  char buf[T_BUF];
  int nbMurs;
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
  robotsC = strdup(buf);
  robotsC[strlen(robotsC)-1] = '\0';

  fgets(buf, T_BUF, f);
  cibleC = strdup(buf);  
  cibleC[strlen(cibleC)-1] = '\0';
  

  printf("[murs] = %s\n", mursC);
  printf("[robots] = %s\n", robotsC);
  printf("[cible] = %s\n", cibleC);
  


  murs = getMurs(mursC, &nbMurs);
  setCible(cibleC, &cible);
  setRobots(robotsC, robots);
  

  fclose(f);
  return NULL;
}

Mur *getMurs(char *c, int *nb) { // return tableau
  return NULL;
}

void setCible(char *c, Cible *cible) {

}

void setRobots(char *c, Robot *robots) {

}




int getNbMurs(char *murs) {
  int cpt=0;
  while(*murs != '\0') 
    if(*murs++ == '(')
      cpt++;
  return cpt;
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
Robot getRobot(Plateau *plateau, Couleur col) {
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

Couleur getDir(char d) { // OK ?
  if(d == 'H')
    return H;
  else if(d == 'B')
    return B;
  else if(d == 'D')
    return D;
  else //(d == 'G')
    return G;
}
