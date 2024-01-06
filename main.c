#include <MLV/MLV_all.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Dimensions de la grille en nombre de cases (origine en haut a gauche) :
#define COLONNES 12
#define LIGNES 18

// Les deux camps :
#define ABEILLES 'A'
#define FRELONS 'F'

// Les types d'unites :
#define REINE 'r'
#define OUVRIERE 'o'
#define ESCADRON 'e'
#define GUERRIERE 'g'
#define RUCHE 'R'
#define NID 'N'

// Pour la recolte de pollen
#define RECOLTE 'p'

// Les temps necessaires a la production :
#define TREINE 8
#define TOUVRIERE 2
#define TGUERRIERE 4
#define TESCADRON 6
#define TFRELON 5
#define TRECOLTE 4

// Les couts necessaires a la production :
#define CREINEA 7
#define CREINEF 8
#define COUVRIERE 3
#define CGUERRIERE 5
#define CESCADRON 6
#define CFRELON 3
#define CRUCHE 10
#define CNID 10

// La force des unites
#define FREINE 6
#define FOUVRIERE 1
#define FGUERRIERE 5
#define FESCADRON 12
#define FFRELON 8

// La structure Unite :
typedef struct unite {
    char camp; // ABEILLE ou FRELON
    char type; // RUCHE, NID, REINE, OUVRIERE, GUERRIERE, ESCADRON ou FRELON
    int force; // la force de l'unite
    int posx, posy;   // position actuelle sur la grille
    int destx, desty; // destination (negatif si immobile)
    char production;  // production d'une ruche ou d'un nid et RECOLTE pour la
                      // recolte de pollen
    int temps;        // nombres de tours total pour cette production
    int toursrestant; // tours restant pour cette production
    struct unite *usuiv,
        *uprec; // liste des unites affiliees a une ruche ou un nid
    struct unite *colsuiv,
        *colprec; // liste des autres ruches ou nids (colonies) du même camp
    struct unite *vsuiv, *vprec; // liste des autres unites sur la meme case
} Unite, *UListe;

// La structure Case :
typedef struct {
    Unite *colonie;  // S'il y a une ruche ou un nid sur la case
    UListe occupant; // les autres occupants de la case
} Case;

// La structure Grille :
typedef struct {
    Case plateau[LIGNES][COLONNES];
    UListe abeille, frelon;
    int tour; // Numero du tour
    int ressourcesAbeille, ressourcesFrelon;
} Grille;

int genStart(void) {
    srand((unsigned int)time(NULL));
    return (rand() % 2) + 1;
}

Case *initCase();
void ajout_uac(UListe occupant, Unite u);
void ajout_colac(UListe camp, Unite colonie);
void ajout_uacol(UListe colonie, Unite u);
Unite *initUnite(char camp, char type, int posx, int posy);
void detruire_unite(UListe u);

void creer_unite(Grille *g, char camp, char type, int x, int y) {
    if (g->plateau[x][y].colonie == NULL) {
        g->plateau[x][y] = *initCase();
    }

    Unite *u = initUnite(camp, type, x, y);
    if (type == 'R' || type == 'N') {
        g->plateau[x][y].colonie = u;
        g->plateau[x][y].occupant = u;
        switch (type) { // ajout de la colonie au camp qui convient.
        case 'R':
            if (g->abeille == NULL) {
                g->abeille = u;
            } else {
                ajout_colac(g->abeille, *u);
                break;
            }
        case 'N':
            if (g->frelon == NULL) {
                g->frelon = u;
            } else {
                ajout_colac(g->frelon, *u);
                break;
            }
        }
    } else {

        ajout_uacol(g->plateau[x][y].colonie,
                    *u); // ajout de l'unité à sa colonie.
        ajout_uac(g->plateau[x][y].occupant, *u); // ajout de l'unité à sa case.
    }
}
/*
void GagneRessource(int unite, Grille *g) {
    printf("Quelle unité souhaitez vous detruire ?\n"
           "1 - "
           "test");

    switch (unite) {
    case 1:

        break;

    default:
        break;
    }
} En cours..
*/

Unite *initUnite(char camp, char type, int posx, int posy) {
    Unite *u = (Unite *)malloc(sizeof(Unite));
    if (u != NULL) {
        u->camp = camp;
        u->type = type;
        switch (type) {
        case 'r':
            u->force = FREINE;
            u->temps = TREINE;
            break;
        case 'o':
            u->force = FOUVRIERE;
            u->temps = TOUVRIERE;
            break;
        case 'e':
            u->force = FESCADRON;
            u->temps = TESCADRON;
            break;
        case 'g':
            u->force = FGUERRIERE;
            u->temps = TGUERRIERE;
            break;
        case 'f':
            u->force = FFRELON;
            u->temps = TFRELON;
            break;
        case 'R':
            u->force = 0;
            u->temps = 0;
            break;
        case 'N':
            u->force = 0;
            u->temps = 0;
            break;
        }
        u->posx = posx;
        u->posy = posy;
        u->destx = -1;
        u->desty = -1;
        u->production = ' ';
        u->toursrestant = -1;
        u->usuiv = NULL;
        u->uprec = NULL;
        u->colsuiv = NULL;
        u->colprec = NULL;
        u->vsuiv = NULL;
        u->vprec = NULL;
    }
    return u;
}

Case *initCase() {
    Case *c = (Case *)malloc(sizeof(Case));
    if (c != NULL) {
        c->colonie = NULL;
        c->occupant = NULL;
    }
    return c;
}

Grille *initGrille() {
    Grille *g = (Grille *)malloc(sizeof(Grille));
    if (g != NULL) {
        for (int i = 0; i < LIGNES; i++) {
            for (int j = 0; j < COLONNES; j++) {
                g->plateau[i][j] = *initCase();
            }
        }
        g->abeille = initUnite('A', 'R', 0, 0);
        g->frelon = initUnite('F', 'N', LIGNES - 1, COLONNES - 1);
        g->tour = 0;
        g->ressourcesAbeille = 0;
        g->ressourcesFrelon = 0;
    }
    return g;
}

void anihile(UListe u);

// ATTENTION!!! Si c'est la 1re colonie d'un camps qui est detruite, il faut
// repointer abeille/frelon dans Grille vers la prochaine colonie avant
// destruction.

void anihile(UListe u) {
    if (u->usuiv == NULL)
        return;
    anihile(u->usuiv);
    detruire_unite(u->usuiv);
}

// Ajoute une colonie à un camp.
void ajout_colac(UListe camp, Unite colonie) {
    if (camp->colsuiv == NULL) {
        camp->colsuiv = &colonie;
        colonie.colprec = camp;
    } else {
        ajout_colac(camp->colsuiv, colonie);
    }
}
// Ajoute une unité à une colonie.
void ajout_uacol(UListe colonie, Unite u) {
    if (colonie == NULL) {
        colonie = &u;
        u.uprec = colonie;
    } else {
        ajout_uacol(colonie, u);
    }
}

UListe prendre_unite(UListe u) {
    // Enlève l'unité donnée de toutes ses listes, refermes ces listes, puis
    // renvoie l'unité.
    if (u->type == 'R' || u->type == 'N') {
        if (u->colsuiv != NULL)
            u->colsuiv->colprec = u->colprec;
        if (u->colprec != NULL)
            u->colprec->colsuiv = u->colsuiv;
        anihile(u);
    } else {
        if (u->usuiv != NULL)
            u->usuiv->uprec = u->uprec;
        if (u->uprec != NULL)
            u->uprec->usuiv = u->usuiv;
    }
    if (u->vsuiv != NULL)
        u->vsuiv->vprec = u->vprec;
    if (u->vprec != NULL)
        u->vprec->vsuiv = u->vsuiv;
    return u;
}

void deplacer_unite(Unite *u, Grille *g) {
    UListe v = prendre_unite(u);

    // if(g->plateau[v.destx][v.desty] == NULL) g->plateau[v.destx][v.desty] =
    // initCase;

    if (g->plateau[v->destx][v->desty].occupant == NULL) {
        g->plateau[v->destx][v->desty].occupant = v;
    } else {
        ajout_uac(g->plateau[v->destx][v->desty].occupant, *v);
    }

    v->posx = v->destx;
    v->posy = v->desty;
    v->destx = -1;
    v->desty = -1;
}

void detruire_unite(UListe u) {
    // Prend une unité et la détruit.
    free(prendre_unite(u));
}

// Ajoute une unité à une case.
void ajout_uac(UListe occupant, Unite u) {
    if (occupant == NULL || occupant->force == u.force) {
        u.vsuiv = occupant;
        u.vprec = occupant;
        occupant = &u;
        occupant = &u;
    } else {
        ajout_uacol(occupant, u);
    }
}

void choixetaction(int n, Grille *g) {
    int choix;
    int x, y;

    if (n % 2 == 0) {
        printf("C'est le tour des Abeilles\n"
               "Pollen : %d\n"
               "1 - Produire Reines (7 Pollen, 8 tours)\n"
               "2 - Produire Ouvrière (3 Pollen, 2 tours)\n"
               "3 - Produire Guerrière (5 Pollen, 4 tours)\n"
               "4 - Produire Escadron (6 Pollen, 6 tours)\n"
               "5 - Detruire une unite\n"
               "6 - Passer son tour\n",
               g->ressourcesAbeille);
    } else {
        printf("C'est le tour des Frelons\n");
        printf("Pollen : %d\n", g->ressourcesFrelon);
        printf("1 - Produire Reines (8 Pollen, 8 tours)\n");
        printf("2 - Produire Frelon (3 Pollen, 5 tours)\n");
        printf("3 - Detruire une unité\n");
        printf("4 - Passer son tour\n");
    }
    scanf("%d", &choix);
    switch (choix) {
    case 1:
        if (n % 2 == 0) {
            if (g->ressourcesAbeille >= 1) { // RessourcesAbeille à 1 pour jouer comme je veux
                printf("Vous produisez une Reine (7 Pollen, 8 tours)\n");
                g->ressourcesAbeille -= 1;
                printf("Choisissez les coordonées x et y\n");
                if (g->abeille != NULL) {
                    do {
                        scanf(" %d %d", &x, &y);
                    } while (g->abeille->posx == x && g->abeille->posy == y);
                } else {
                    scanf(" %d %d", &x, &y);
                }
                creer_unite(g, ABEILLES, REINE, x, y);

                printf("Vous avez crée une unité en %d, %d\n", x, y);
                break;
            } else {
                printf("Pas assez de pollen pour produire une Reine.\n");
                choixetaction(n, g);
            }
        } else {
            if (g->ressourcesFrelon >= 8) {
                printf("Vous produisez une Reine (8 Pollen, 8 tours)\n");
                g->ressourcesFrelon -= 8;
                break;
            } else {
                printf("Pas assez de pollen pour produire une Reine.\n");
                choixetaction(n, g);
            }
        }
        break;
    case 2:
        if (n % 2 == 0) {
            if (g->ressourcesAbeille >= 3) {
                printf("Vous produisez une Ouvrière\n");
                g->ressourcesAbeille -= 3;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
                choixetaction(n, g);
            }
        } else {
            if (g->ressourcesFrelon >= 3) {
                printf("Vous produisez un Frelon\n");
                g->ressourcesFrelon -= 3;
                break;
            } else {
                printf("Vous n'avez pas assez de ressource\n");
                choixetaction(n, g);
            }
        }
        break;
    case 3:
        if (n % 2 == 0) {
            if (g->ressourcesAbeille >= 5) {
                printf("Vous produisez une Guerrière\n");
                g->ressourcesAbeille -= 5;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
                choixetaction(n, g);
            }
        } else {
            printf("Vous detruisez X et recuperez X ressources\n");
            break;
        }
        break;
    case 4:
        if (n % 2 == 0) {
            if (g->ressourcesAbeille >= 6) {
                printf("Vous produisez un Escadron\n");
                g->ressourcesAbeille -= 6;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
                choixetaction(n, g);
            }
        } else {
            printf("Vous passez votre tour\n");
            break;
        }
        break;
    case 5:
        if (n % 2 == 0) {
            printf("Vous detruisez X et recuperez X pollen\n");
            break;
        } else {
            printf("Mauvais choix\n");
            choixetaction(n, g);
            break;
        }
    case 6:
        if (n % 2 == 0) {
            printf("Vous passez votre tour\n");
            break;
        } else {
            printf("Mauvais choix\n");
            choixetaction(n, g);
            break;
        }
    default:
        printf("Mauvais choix\n");
        choixetaction(n, g);
        break;
    }
}

int main(void) {
    Grille *g = initGrille();
    bool jeu = true;
    g->tour = genStart();
    printf("%d\n", g->tour);
    g->ressourcesAbeille = g->ressourcesFrelon = 10;

    while (jeu) {
        choixetaction(g->tour, g);
        g->tour += 1;
        if (g->tour % 2)
            printf("Il vous reste %d ressources Abeilles\n",
                   g->ressourcesAbeille);
        else
            printf("Il vous reste %d ressources Frelon\n", g->ressourcesFrelon);
        if (g->ressourcesAbeille < 3 && g->ressourcesFrelon < 3) {
            printf("Vous devez attendre de récuperer des ressources");
            jeu = false;
        }
    }

    return 0;
}