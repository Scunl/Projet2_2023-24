#include <MLV/MLV_all.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Dimensions de la grille en nombre de cases (origine en haut a gauche) :
#define COLONNES 12
#define LIGNES 18

// Les deux camps :
#define ABEILLE 'A'
#define FRELON 'F'

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
#define TREINEA 8
#define TREINEF 8
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
char **cree_table() {
    char **tab = malloc(LIGNES * sizeof(char *));
    for (int i = 0; i < LIGNES; i++) {
        tab[i] = malloc(COLONNES * sizeof(char));
        for (int j = 0; j < COLONNES; j++) {
            tab[i][j] = 'N';
        }
    }
    return tab;
}

int genStart(void) {
    srand((unsigned int)time(NULL));
    return rand() % 2;
}

int afficheChoix(int n, int *nbPollenA, int *nbPollenF) {
    int choix;
    if (n) {
        printf("C'est le tour des Abeilles\n");
        printf("Pollen : %d\n", *nbPollenA);
        printf("1 - Produire Reines (7 Pollen, 8 tours)\n");
        printf("2 - Produire Ouvrière (3 Pollen, 2 tours)\n");
        printf("3 - Produire Guerrière (5 Pollen, 4 tours)\n");
        printf("4 - Produire Escadron (6 Pollen, 6 tours)\n");
        printf("5 - Detruire une unite\n");
    } else {
        printf("C'est le tour des Frelons\n");
        printf("Pollen : %d\n", *nbPollenF);
        printf("1 - Produire Reines (8 Pollen, 8 tours)\n");
        printf("2 - Produire Frelon (3 Pollen, 5 tours)\n");
        printf("3 - Detruire une unité\n");
    }
    scanf("%d", &choix);

    switch (choix) {
    case 1:
        if (n) {
            if (*nbPollenA >= 7) {
                printf("Vous produisez une Reine (7 Pollen, 8 tours)\n");
                *nbPollenA -= 7;
                break;
            } else {
                printf("Pas assez de pollen pour produire une Reine.\n");
            }
        } else {
            if (*nbPollenF >= 8) {
                printf("Vous produisez une Reine (8 Pollen, 8 tours)\n");
                *nbPollenF -= 8;
                break;
            } else {
                printf("Pas assez de pollen pour produire une Reine.\n");
            }
        }
        break;
    case 2:
        if (n) {
            if (*nbPollenA >= 3) {
                printf("Vous produisez une Ouvrière\n");
                *nbPollenA -= 3;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
            }
        } else {
            if (*nbPollenF >= 3) {
                printf("Vous produisez un Frelon\n");
                *nbPollenF -= 3;
                break;
            } else {
                printf("Vous n'avez pas assez de ressource\n");
            }
        }
        break;
    case 3:
        if (n) {
            if (*nbPollenA >= 5) {
                printf("Vous produisez une Guerrière\n");
                *nbPollenA -= 5;
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
            if (*nbPollenA >= 6) {
                printf("Vous produisez un Escadron\n");
                *nbPollenA -= 6;
                break;
            } else {
                printf("Vous n'avez pas assez de Pollen\n");
            }
        } else {
            printf("Mauvais choix\n");
            afficheChoix(n, nbPollenA, nbPollenF);
            break;
        }
        break;
    case 5:
        if (n) {
            printf("Vous detruisez X et recuperez X pollen\n");
            break;
        } else {
            printf("Mauvais choix\n");
            afficheChoix(n, nbPollenA, nbPollenF);
            break;
        }
    default:
        printf("Mauvais choix\n");
        afficheChoix(n, nbPollenA, nbPollenF);
        break;
    }

    return choix;
}

void afficher_table(char **plateau) {
    for (int i = 0; i < LIGNES; i++) {
        printf("\n");
        for (int j = 0; j < COLONNES; j++) {
            printf("| %c ", plateau[i][j]);
        }
        printf("|");
    }
    printf("\n");
}

int main(void) {
    char **tab = cree_table();
    afficher_table(tab);
    int generer = genStart();
    printf("%d\n", generer);
    int nbPollenA = 10;
    int nbPollenF = 10;
    int *ptrPollenA = &nbPollenA;
    int *ptrPollenF = &nbPollenF;
    afficheChoix(generer, ptrPollenA, ptrPollenF);
    printf("Il vous reste %d Pollen et %d Pollen\n", *ptrPollenA, *ptrPollenF);
    return 0;
}