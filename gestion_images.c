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

int main()
{
    int lignes1, colonnes1;
    int lignes2, colonnes2;
    int maxval;
    int histogramme[MAX_VALEUR+1];
    char nom[MAX_CHAINE] = "/Users/mathieudurand/CLionProjects/S2-A4/Sherbrooke_Frontenac_nuit.ppm";
    char nomOut[MAX_CHAINE] = "/Users/mathieudurand/CLionProjects/S2-A4/Sherbrooke_Frontenac_nuit2.ppm";
    struct MetaData metadonnees;

	int retour;

    printf("-> Debut!\n");

	// exemple d'appel de fonction
    retour = ppm_lire(nom, image1,
                      &lignes1, &colonnes1, 
                      &maxval, &metadonnees);

	// exemple detraitement d'un code de retour (erreur ou reussite)
	printf("-> Retour: ");
	if (retour == OK)
		printf("-> OK");
	else
		printf("-> ERREUR");
	printf("\n");

	// autre exemple d'appel de fonction
    ppm_ecrire(nomOut, image1,
               lignes1, colonnes1, 
               maxval, metadonnees);

    printf("-> Fin!\n");

    return 0;
}
