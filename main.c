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
/*
Case alloueCellule(int x) {
    Cellule *cell = (Cellule *)malloc(sizeof(Cellule));
    if (cell != NULL) {
        cell->valeur = x;
        cell->suivant = NULL;
    }
    return cell;
}

Case extraitTete(Liste *al) {
    if (*al == NULL)
        return NULL;
    Cellule *tmp = *al;
    *al = tmp->suivant;
    tmp->suivant = NULL;
    return tmp;
}

int insererTete(Liste *al, int x) {
    Case *cell = alloueCellule(x);
    if (cell == NULL)
        return 0;
    cell->suivant = *al;
    *al = cell *;
    return 1;
}
void commencejeu(**plateau, int n) {
    if (n == 1) {
        Case->occupant->posx = 0;
        Case->occupant->posy = 0;
    }
}
*/

int genStart(void) {
    srand((unsigned int)time(NULL));
    return rand() % 2;
}

int afficheChoix(int n, Grille *g) {
    int choix;
    if (n) {
        printf("C'est le tour des Abeilles\n");
        printf("Pollen : %d\n", g->ressourcesAbeille);
        printf("1 - Produire Reines (7 Pollen, 8 tours)\n");
        printf("2 - Produire Ouvrière (3 Pollen, 2 tours)\n");
        printf("3 - Produire Guerrière (5 Pollen, 4 tours)\n");
        printf("4 - Produire Escadron (6 Pollen, 6 tours)\n");
        printf("5 - Detruire une unite\n");
        printf("6 - Passer son tour\n");
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
        if (n) {
            if (g->ressourcesAbeille >= 7) {
                printf("Vous produisez une Reine (7 Pollen, 8 tours)\n");
                g->ressourcesAbeille -= 7;
                break;
            } else {
                printf("Pas assez de pollen pour produire une Reine.\n");
            }
        } else {
            if (g->ressourcesFrelon >= 8) {
                printf("Vous produisez une Reine (8 Pollen, 8 tours)\n");
                g->ressourcesFrelon -= 8;
                break;
            } else {
                printf("Pas assez de pollen pour produire une Reine.\n");
            }
        }
        break;
    case 2:
        if (n) {
            if (g->ressourcesAbeille >= 3) {
                printf("Vous produisez une Ouvrière\n");
                g->ressourcesAbeille -= 3;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
            }
        } else {
            if (g->ressourcesFrelon >= 3) {
                printf("Vous produisez un Frelon\n");
                g->ressourcesFrelon -= 3;
                break;
            } else {
                printf("Vous n'avez pas assez de ressource\n");
            }
        }
        break;
    case 3:
        if (n) {
            if (g->ressourcesAbeille >= 5) {
                printf("Vous produisez une Guerrière\n");
                g->ressourcesAbeille -= 5;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
            }
        } else {
            printf("Vous detruisez X et recuperez X pollen\n");
            break;
        }
        break;
    case 4:
        if (n) {
            if (g->ressourcesAbeille >= 6) {
                printf("Vous produisez un Escadron\n");
                g->ressourcesAbeille -= 6;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
            }
        } else {
            printf("Vous passez votre tour\n");
            break;
        }
        break;
    case 5:
        if (n) {
            printf("Vous detruisez X et recuperez X pollen\n");
            break;
        } else {
            printf("Mauvais choix\n");
            afficheChoix(n, g);
            break;
        }
    case 6:
        if (n) {
            printf("Vous passez votre tour\n");
            break;
        } else {
            printf("Mauvais choix\n");
            afficheChoix(n, g);
            break;
        }
    default:
        printf("Mauvais choix\n");
        afficheChoix(n, g);
        break;
    }
    return choix;
}


Unite *initUnite(char camp, char type, int posx, int posy){
    Unite *u = (Unite*)malloc(sizeof(Unite));
    if(u != NULL){
        u->camp = camp;
        u->type = type;
        switch(type){
            case 'r' : u->force = FREINE; u->temps = TREINE; break;
            case 'o' : u->force = FOUVRIERE; u->temps = TOUVRIERE; break;
            case 'e' : u->force = FESCADRON; u->temps = TESCADRON; break;
            case 'g' : u->force = FGUERRIERE; u->temps = TGUERRIERE; break;
            case 'f' : u->force = FFRELON; u->temps = TFRELON; break;
            case 'R' : u->force = 0; u->temps = 0; break;
            case 'N' : u->force = 0; u->temps = 0; break;
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

Case * initCase(){
    Case *c = (Case*)malloc(sizeof(Case));
    if(c != NULL){
        c->colonie = NULL;
        c->occupant = NULL;
    }
    return c;
}

Grille * initGrille(){
    Grille *g = (Grille*)malloc(sizeof(Grille));
    if(g != NULL){
        g->abeille = NULL;
        g->frelon = NULL;
        g->tour = 0;
        g->ressourcesAbeille = 0;
        g->ressourcesFrelon = 0;
    }
    return g;
}


int main(void) {
    Grille g;
    bool jeu = true;
    int generer = genStart();
    printf("%d\n", generer);

    g.ressourcesAbeille = 10;
    g.ressourcesFrelon = 10;
    while (jeu) {
        afficheChoix(generer, &g);
        if (generer == 1)
            generer = 0;
        else
            generer = 1;
        printf("Il vous reste %d Pollen Abeilles et %d Pollen Frelons\n",
               g.ressourcesAbeille, g.ressourcesFrelon);
        if (g.ressourcesAbeille < 3 && g.ressourcesFrelon < 3) {
            printf("Vous devez attendre de récuperer des ressources");
            jeu = false;
        }
    }

    return 0;
}