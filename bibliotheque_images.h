/********
Fichier: gestion_images.c
Auteurs: Charles-Antoine Brunet (structure)
		 Domingo Palao (code original en C++)
		 Alexandre Huppe (passage de C++ vers C)
Date: 22 octobre 2015 (creation)
	  8 novembre 2015 (adaptation de la solution C++)
	  8 decembre 2017 (faire refereces a l'AP et non l'APP)
	  28 octobre 2023 (mise à jour)
Description: Solution de la problematique pour GEN145.
********/
#ifndef BIBLIOTHEQUE_IMAGES_H
#define BIBLIOTHEQUE_IMAGES_H

#include <stdio.h>
#include "string_x.h"

// Valeurs maximales pour les images
#define MAX_LARGEUR 256
#define MAX_HAUTEUR 256
#define MAX_VALEUR 65535

// Valeurs de retour des operations
#define OK 0
#define ERREUR -1
#define ERREUR_FICHIER -1
#define ERREUR_TAILLE -2
#define ERREUR_FORMAT -3
#define IDENTIQUES 0
#define DIFFERENTES 1

// Sens des rotations pour l'operation pivoter90
#define SENS_ANTIHORAIRE 0
#define SENS_HORAIRE 1

// Longueur maximale d'une chaine de caracteres
#define MAX_CHAINE 1024

#define P2 2
#define P3 3

#define MAX_PIXEL_BYTES 3

struct MetaData {
    char auteur[MAX_CHAINE];
    char dateCreation[MAX_CHAINE]; // yyyy-mm-dd
    char lieuCreation[MAX_CHAINE];
};

struct ImageInfo {
    struct MetaData metadata;
    int type;
    int width;
    int height;
    int maxval;
};

struct RGB
{
    int valeurR;
    int valeurG;
    int valeurB;
};

int meta_data_lire(char* comment, struct MetaData *p_metadonnees);
int image_info_lire(FILE *file, struct ImageInfo *p_info);
int color_data_read(FILE *file, int data[MAX_HAUTEUR * MAX_LARGEUR * MAX_PIXEL_BYTES]);

int meta_data_ecrire(FILE *file, struct MetaData *p_metadonnees);
int image_info_ecrire(FILE *file, struct ImageInfo *p_info);

// Operations pour les images noir et blanc
int pgm_lire(char nom_fichier[], int matrice[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes, int *p_colonnes, int *p_maxval, struct MetaData *p_metadonnees);
int pgm_ecrire(char nom_fichier[], int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval, struct MetaData metadonnees);
int pgm_copier(int matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int matrice2[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes2, int *p_colonnes2);
int pgm_creer_histogramme(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int histogramme[MAX_VALEUR+1]);
int pgm_couleur_preponderante(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes);
int pgm_eclaircir_noircir(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval, int valeur);
int pgm_creer_negatif(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval);
int pgm_extraire(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int lignes2, int colonnes2, int *p_lignes, int *p_colonnes);
int pgm_sont_identiques(int matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int matrice2[MAX_HAUTEUR][MAX_LARGEUR], int lignes2, int colonnes2);
int pgm_pivoter90(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes, int *p_colonnes, int sens);

// Operations pour les images couleurs
int ppm_lire(char nom_fichier[], struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes, int *p_colonnes, int *p_maxval, struct MetaData *p_metadonnees);
int ppm_ecrire(char nom_fichier[], struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval, struct MetaData metadonnees);
int ppm_copier(struct RGB matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, struct RGB matrice2[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes2, int *p_colonnes2);
int ppm_sont_identiques(struct RGB matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, struct RGB matrice2[MAX_HAUTEUR][MAX_LARGEUR], int lignes2, int colonnes2);
int ppm_pivoter90(struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes, int *p_colonnes, int sens);

#endif
