/********
Fichier: gestion_images.c
Auteurs: Domingo Palao Munoz
         Charles-Antoine Brunet
Date: 28 octobre 2023
Description: Fichier de distribution pour GEN145.
********/

#include <stdio.h>
#include "bibliotheque_images.h"

int image1[MAX_HAUTEUR][MAX_LARGEUR];
int image2[MAX_HAUTEUR][MAX_LARGEUR];
struct RGB imageRGB1[MAX_HAUTEUR][MAX_LARGEUR];
struct RGB imageRGB2[MAX_HAUTEUR][MAX_LARGEUR];

void printHistogram(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int barLength) {
    int histogram[MAX_VALEUR + 1];
    pgm_creer_histogramme(matrice, lignes, colonnes, histogram);

    int preponderant_color = pgm_couleur_preponderante(matrice, lignes, colonnes);
    int max_occurences = histogram[preponderant_color];

    for (int color = 0; color <= MAX_VALEUR; color++) {
        printf("%d : (%d) : ", color, histogram[color]);
        for (int i = 0; i < (histogram[color] * barLength) / max_occurences; i++) printf("#");
        printf("\n");
    }
}

char* error_code_to_string(int error_code) {
    switch(error_code) {
        case -1:
            return "ERREUR/ERREUR_FICHIER";
        case -2:
            return "ERREUR_TAILLE";
        case -3:
            return "ERREUR_FORMAT";
        case 0:
            return "OK";
        default:
            return "Unknown error code";
    }
}

int main()
{
    int lignes1, colonnes1;
    int lignes2, colonnes2;
    int maxval;
    int histogramme[MAX_VALEUR+1];
    char nom[MAX_CHAINE] = "/Users/mathieudurand/CLionProjects/S2-A4/Sherbrooke_Frontenac_nuit.pgm";
    char nomOut[MAX_CHAINE] = "/Users/mathieudurand/CLionProjects/S2-A4/Sherbrooke_Frontenac_nuit2.pgm";
    struct MetaData metadonnees;

	int retour;

    printf("-> Debut!\n");

	// exemple d'appel de fonction
    retour = pgm_lire(nom, image1,
                      &lignes1, &colonnes1, 
                      &maxval, &metadonnees);

    printHistogram(image1, lignes1, colonnes1, 50);

	// exemple detraitement d'un code de retour (erreur ou reussite)
	printf("-> Retour: ");
	if (retour == OK)
		printf("-> OK");
	else
		printf("-> %s", error_code_to_string(retour));
	printf("\n");

	// autre exemple d'appel de fonction
    pgm_ecrire(nomOut, image1,
               lignes1, colonnes1, 
               maxval, metadonnees);

    printf("-> Fin!\n");

    return 0;
}
