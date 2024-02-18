/********
Fichier: bibliotheque_images.c
Auteurs: Domingo Palao Munoz
         Charles-Antoine Brunet
Date: 28 octobre 2023
Description: Fichier de distribution pour GEN145.
********/

#include <string.h>
#include "bibliotheque_images.h"

int meta_data_lire(char* comment, struct MetaData *p_metadonnees) {
    char data[3][STR_MAX_LENGTH];

    if (split(comment, ";", 3, data) != 3) {
        return ERREUR;
    }

    if (length(data[0]) >= MAX_CHAINE || length(data[1]) >= MAX_CHAINE || length(data[2]) >= MAX_CHAINE) {
        return ERREUR;
    }

    strcpy(p_metadonnees->auteur, data[0]);
    strcpy(p_metadonnees->dateCreation, data[1]);
    strcpy(p_metadonnees->lieuCreation, data[2]);

    return OK;
}

int image_info_lire(FILE *file, struct ImageInfo *p_info) {
    int i = 0;
    char line[MAX_CHAINE];

    if (fgets(line, MAX_CHAINE, file) == NULL) {
        return ERREUR;
    }

    if (line[0] != '#') {
        rewind(file);
    } else {
        // Remove newline
        line[length(line) - 1] = '\0';

        int codeMetaData = meta_data_lire(line + 1, &p_info->metadata);
        if (codeMetaData != OK) return codeMetaData;
        printf("Author: %s\nDate: %s\nLocation: %s\n", p_info->metadata.auteur, p_info->metadata.dateCreation, p_info->metadata.lieuCreation);
    }

    if (fscanf(file, "%*c%d", &p_info->type) != 1) return ERREUR_FORMAT;
    if (p_info->type != P2 && p_info->type != P3) return ERREUR_FORMAT;

    if (fscanf(file, "%d%*[ \t]%d", &p_info->width, &p_info->height) != 2) return ERREUR_TAILLE;
    if (p_info->width == 0 || p_info->height == 0 || p_info->width > MAX_LARGEUR || p_info->height > MAX_HAUTEUR) return ERREUR_TAILLE;

    if (fscanf(file, "%d", &p_info->maxval) != 1) return ERREUR_TAILLE;
    if (p_info->maxval == 0 || p_info->maxval > MAX_VALEUR) return ERREUR_TAILLE;

    return OK;
}

int color_data_read(FILE *file, int data[MAX_HAUTEUR * MAX_LARGEUR * MAX_PIXEL_BYTES]) {
    int i = 0;

    while (i < MAX_HAUTEUR * MAX_LARGEUR * MAX_PIXEL_BYTES && fscanf(file, "%d", &data[i++]) == 1);

    return i < MAX_HAUTEUR * MAX_LARGEUR * MAX_PIXEL_BYTES ? OK : ERREUR_TAILLE;
}

int meta_data_ecrire(FILE *file, struct MetaData *p_metadonnees) {
    fprintf(file, "#%s;%s;%s\n", p_metadonnees->auteur, p_metadonnees->dateCreation, p_metadonnees->lieuCreation);
    printf("#%s;%s;%s\n", p_metadonnees->auteur, p_metadonnees->dateCreation, p_metadonnees->lieuCreation);

    return OK;
}

int image_info_ecrire(FILE *file, struct ImageInfo *p_info) {
    int codeMetaData = meta_data_ecrire(file, &p_info->metadata);
    if (codeMetaData != OK) return codeMetaData;

    fprintf(file, "P%d\n%d %d\n%d\n", p_info->type, p_info->width, p_info->height, p_info->maxval);

    return OK;
}

int pgm_lire(char nom_fichier[], int matrice[MAX_HAUTEUR][MAX_LARGEUR],
             int *p_lignes, int *p_colonnes,
             int *p_maxval, struct MetaData *p_metadonnees) {

    FILE *file = fopen(nom_fichier, "r");

    struct ImageInfo info;
    int codeInfo = image_info_lire(file, &info);
    if (codeInfo != OK) return codeInfo;

    if (info.type != P2) return ERREUR_FORMAT;

    int data[info.width * info.height];
    int codeData = color_data_read(file, data);
    if (codeData != OK) return codeData;

    //memcpy(matrice, data, MAX_HAUTEUR * MAX_LARGEUR * sizeof(int));


    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {
            matrice[i][j] = data[i * info.width + j];
        }
    }
    *p_lignes = info.height;
    *p_colonnes = info.width;
    *p_maxval = info.maxval;
    *p_metadonnees = info.metadata;

    fclose(file);

    return OK;
}

int pgm_ecrire(char nom_fichier[], int matrice[MAX_HAUTEUR][MAX_LARGEUR],
               int lignes, int colonnes,
               int maxval, struct MetaData metadonnees) {

    FILE *file = fopen(nom_fichier, "w");

    struct ImageInfo info = {metadonnees, P2, colonnes, lignes, maxval};

    image_info_ecrire(file, &info);

    int count = 0;

    char number[5];

    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {

            fprintf(file, "%d\t", matrice[i][j]);

            if (count++ > 25) {
                fprintf(file, "\n");
                count = 0;
            }

            fprintf(file, "%s", number);
        }
    }

    fclose(file);

    return OK;
}

int pgm_copier(int matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int matrice2[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes2, int *p_colonnes2) {
    memcpy(matrice2, matrice1, sizeof(int) * MAX_HAUTEUR * MAX_LARGEUR);
    *p_lignes2 = lignes1;
    *p_colonnes2 = colonnes1;
}

int pgm_sont_identiques(int matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int matrice2[MAX_HAUTEUR][MAX_LARGEUR], int lignes2, int colonnes2) {
    if (lignes1 != lignes2 || colonnes2 != lignes2) {
        return 0;
    }

    for (int i = 0; i < lignes1; i++)
        for (int j = 0; j < colonnes1; j++)
            if (matrice1[i][j] != matrice2[i][j]) return 0;

    return 1;
}

int ppm_lire(char nom_fichier[], struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR],
             int *p_lignes, int *p_colonnes,
             int *p_maxval, struct MetaData *p_metadonnees) {

    FILE *file = fopen(nom_fichier, "r");

    struct ImageInfo info;
    int codeInfo = image_info_lire(file, &info);
    if (codeInfo != OK) return codeInfo;

    if (info.type != P3) return ERREUR_FORMAT;

    int data[info.width * info.height * 3];
    int codeData = color_data_read(file, data);

    if (codeData != OK) return codeData;

    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {
            int index = 3 * (i * info.width + j);
            matrice[i][j] = (struct RGB) {data[index], data[index + 1], data[index + 2]};
        }
    }

    *p_lignes = info.height;
    *p_colonnes = info.width;
    *p_maxval = info.maxval;
    *p_metadonnees = info.metadata;

    fclose(file);

    return OK;
}

int ppm_ecrire(char nom_fichier[], struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR],
               int lignes, int colonnes,
               int maxval, struct MetaData metadonnees) {

    FILE *file = fopen(nom_fichier, "w");

    struct ImageInfo info = {metadonnees, P3, colonnes, lignes, maxval};

    image_info_ecrire(file, &info);

    int count = 0;

    char number[5];

    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {

            fprintf(file, "%d\t%d\t%d\t", matrice[i][j].valeurR, matrice[i][j].valeurG, matrice[i][j].valeurB);

            if (count++ > 8) {
                fprintf(file, "\n");
                count = 0;
            }

            fprintf(file, "%s", number);
        }
    }

    fclose(file);

    return OK;
}

int ppm_copier(struct RGB matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, struct RGB matrice2[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes2, int *p_colonnes2) {
    memcpy(matrice2, matrice1, sizeof(struct RGB) * MAX_HAUTEUR * MAX_LARGEUR);
    *p_lignes2 = lignes1;
    *p_colonnes2 = colonnes1;
}

int ppm_sont_identiques(struct RGB matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, struct RGB matrice2[MAX_HAUTEUR][MAX_LARGEUR], int lignes2, int colonnes2) {
    if (lignes1 != lignes2 || colonnes2 != lignes2) {
        return 0;
    }

    for (int i = 0; i < lignes1; i++)
        for (int j = 0; j < colonnes1; j++) {
            if (matrice1[i][j].valeurR != matrice2[i][j].valeurR) return 0;
            if (matrice1[i][j].valeurG != matrice2[i][j].valeurG) return 0;
            if (matrice1[i][j].valeurB != matrice2[i][j].valeurB) return 0;
        }

    return 1;
}