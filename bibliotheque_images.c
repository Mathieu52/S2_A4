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
    for (int i = 0; i < MAX_HAUTEUR * MAX_LARGEUR * MAX_PIXEL_BYTES; i++) {
        if (fscanf(file, "%d", &data[i]) != 1) return OK;
        if (data[i] < 0 || data[i] > MAX_VALEUR) return ERREUR_FORMAT;
    }

    return OK;
}

int meta_data_ecrire(FILE *file, struct MetaData *p_metadonnees) {
    fprintf(file, "#%s;%s;%s\n", p_metadonnees->auteur, p_metadonnees->dateCreation, p_metadonnees->lieuCreation);

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
    if (file == NULL) return ERREUR_FICHIER;

    struct ImageInfo info;
    int codeInfo = image_info_lire(file, &info);
    if (codeInfo != OK) return codeInfo;

    if (info.type != P2) return ERREUR_FORMAT;

    int data[info.width * info.height];
    int codeData = color_data_read(file, data);
    if (codeData != OK) return codeData;

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
    if (file == NULL) return ERREUR_FICHIER;

    struct ImageInfo info = {metadonnees, P2, colonnes, lignes, maxval};

    image_info_ecrire(file, &info);

    int count = 0;

    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {

            fprintf(file, "%d\t", matrice[i][j]);

            if (count++ > 25) {
                fprintf(file, "\n");
                count = 0;
            }
        }
    }

    fclose(file);

    return OK;
}

int pgm_copier(int matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int matrice2[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes2, int *p_colonnes2) {
    if (lignes1 != *p_lignes2) return ERREUR;
    if (colonnes1 != *p_colonnes2) return ERREUR;
    if (lignes1 < 0 || lignes1 > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes1 < 0 || colonnes1 > MAX_LARGEUR) return ERREUR_TAILLE;

    memcpy(matrice2, matrice1, sizeof(int) * MAX_HAUTEUR * MAX_LARGEUR);
    *p_lignes2 = lignes1;
    *p_colonnes2 = colonnes1;

    return OK;
}

int pgm_creer_histogramme(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int histogramme[MAX_VALEUR+1]) {
    if (lignes < 0 || lignes > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes < 0 || colonnes > MAX_LARGEUR) return ERREUR_TAILLE;

    for (int i = 0; i <= MAX_VALEUR; i++) {
        histogramme[i] = 0;
    }

    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            int brightness = matrice[i][j];
            if (brightness < 0 || brightness > MAX_VALEUR) return ERREUR_FORMAT;

            histogramme[brightness]++;
        }
    }

    return OK;
}

int pgm_couleur_preponderante(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes) {
    // A zero-sized image is counted as an error as this operation is impossible if there is no color
    if (lignes <= 0 || lignes > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes <= 0 || colonnes > MAX_LARGEUR) return ERREUR_TAILLE;

    int histogram[MAX_VALEUR+1];
    int error_code = pgm_creer_histogramme(matrice, lignes, colonnes, histogram);
    if (error_code != OK) return error_code;

    int preponderant_color = 0;
    int occurrences = histogram[0];

    for (int color = 1; color <= MAX_VALEUR; color++)
        if (histogram[color] > occurrences) {
            occurrences = histogram[color];
            preponderant_color = color;
        }

    return preponderant_color;
}

int pgm_eclaircir_noircir(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval, int valeur) {
    // TODO Implement pgm_eclaircir_noircir
}

int pgm_creer_negatif(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval) {
    // TODO Implement pgm_creer_negatif
}

int pgm_extraire(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int lignes2, int colonnes2, int *p_lignes, int *p_colonnes) {
    // TODO Implement pgm_extraire
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

int pgm_pivoter90(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes, int *p_colonnes, int sens) {
    if (sens != 0 && sens != 1) return ERREUR;
    if (*p_lignes < 0 || *p_lignes > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (*p_colonnes < 0 || *p_colonnes > MAX_LARGEUR) return ERREUR_TAILLE;

    int matriceCopy[MAX_HAUTEUR][MAX_LARGEUR];
    memcpy(matriceCopy, matrice, MAX_HAUTEUR * MAX_LARGEUR * sizeof(int));

    for (int i = 0; i < *p_lignes; i++) {
        for (int j = 0; j < *p_colonnes; j++) {
            int iDest = sens == 1 ? j : ((*p_colonnes - 1) - j);
            int jDest = sens == 0 ? i : ((*p_lignes - 1) - i);
            matrice[iDest][jDest] = matriceCopy[i][j];
        }
    }

    // Invert the sides
    int temp = *p_lignes;
    *p_lignes = *p_colonnes;
    *p_colonnes = temp;

    return OK;
}

int ppm_lire(char nom_fichier[], struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR],
             int *p_lignes, int *p_colonnes,
             int *p_maxval, struct MetaData *p_metadonnees) {

    FILE *file = fopen(nom_fichier, "r");
    if (file == NULL) return ERREUR_FICHIER;

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
    if (file == NULL) return ERREUR_FICHIER;

    struct ImageInfo info = {metadonnees, P3, colonnes, lignes, maxval};

    image_info_ecrire(file, &info);

    int count = 0;

    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {

            fprintf(file, "%d\t%d\t%d\t", matrice[i][j].valeurR, matrice[i][j].valeurG, matrice[i][j].valeurB);

            if (count++ > 8) {
                fprintf(file, "\n");
                count = 0;
            }
        }
    }

    fclose(file);

    return OK;
}

int ppm_copier(struct RGB matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, struct RGB matrice2[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes2, int *p_colonnes2) {
    if (lignes1 != *p_lignes2) return ERREUR;
    if (colonnes1 != *p_colonnes2) return ERREUR;
    if (lignes1 < 0 || lignes1 > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes1 < 0 || colonnes1 > MAX_LARGEUR) return ERREUR_TAILLE;

    memcpy(matrice2, matrice1, sizeof(struct RGB) * MAX_HAUTEUR * MAX_LARGEUR);
    *p_lignes2 = lignes1;
    *p_colonnes2 = colonnes1;

    return OK;
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

int ppm_pivoter90(struct RGB matrice[MAX_HAUTEUR][MAX_LARGEUR], int *p_lignes, int *p_colonnes, int sens) {
    if (sens != 0 && sens != 1) return ERREUR;
    if (*p_lignes < 0 || *p_lignes > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (*p_colonnes < 0 || *p_colonnes > MAX_LARGEUR) return ERREUR_TAILLE;

    struct RGB matriceCopy[MAX_HAUTEUR][MAX_LARGEUR];
    memcpy(matriceCopy, matrice, MAX_HAUTEUR * MAX_LARGEUR * sizeof(struct RGB));

    for (int i = 0; i < *p_lignes; i++) {
        for (int j = 0; j < *p_colonnes; j++) {
            int iDest = sens == 1 ? j : ((*p_colonnes - 1) - j);
            int jDest = sens == 0 ? i : ((*p_lignes - 1) - i);
            matrice[iDest][jDest] = matriceCopy[i][j];
        }
    }

    // Invert the sides
    int temp = *p_lignes;
    *p_lignes = *p_colonnes;
    *p_colonnes = temp;
}