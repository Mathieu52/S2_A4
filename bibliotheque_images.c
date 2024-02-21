/********
Fichier: bibliotheque_images.c
Auteurs: Domingo Palao Munoz
         Charles-Antoine Brunet
Date: 28 octobre 2023
Description: Fichier de distribution pour GEN145.
********/

#include <string.h>
#include "bibliotheque_images.h"

//
// Created by Mathieu Durand on 2024-02-16.
//

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * This function iterates over the characters of the input string until it
 * encounters the null terminator ('\0') or reaches the maximum allowed length
 * of the string, whichever comes first. It then returns the number of characters
 * encountered before the null terminator.
 *
 * @param str Pointer to the null-terminated string whose length is to be calculated.
 *            If NULL, the function returns 0.
 * @return The length of the string, excluding the null terminator.
 */
int length(const char* str) {
    if (str == NULL) return 0;  // Handle NULL pointer gracefully

    int index = 0;
    while (index < STR_MAX_LENGTH && str[index] != '\0') {
        index++;
    }

    return index;
}

/**
 * @brief Checks if two null-terminated strings are equal.
 *
 * This function iterates over the characters of both input strings simultaneously
 * until it encounters a mismatch or reaches the maximum allowed length of the strings.
 * If a mismatch is found, it returns 0. If both strings are equal up to the null
 * terminator of either string, it returns 1. If one of the strings is NULL and the
 * other is not, it returns 0. If both strings are NULL, it returns 1.
 *
 * @param str1 Pointer to the first null-terminated string to compare.
 * @param str2 Pointer to the second null-terminated string to compare.
 * @return 1 if the strings are equal up to the null terminator of either string,
 *         0 otherwise. If one of the strings is NULL and the other is not, it returns 0.
 *         If both strings are NULL, it returns 1.
 */
int equals(const char* str1, const char* str2) {
    // Handle NULL pointer gracefully
    if (str1 == NULL && str2 == NULL) return 1;
    if (str1 == NULL || str2 == NULL) return 0;

    for (int index = 0; index < STR_MAX_LENGTH; index++) {
        if (str1[index] != str2[index]) {
            return 0; // Mismatch found
        }

        if (str1[index] == '\0' || str2[index] == '\0') {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Splits a string into tokens based on a delimiter.
 *
 * This function takes a source string and a delimiter string and splits the source
 * string into tokens based on the delimiter. The tokens are stored in an array of
 * strings, provided as the 'dest' parameter. The maximum number of tokens to be
 * stored in the 'dest' array is specified by the 'size' parameter.
 *
 * @param source Pointer to the source string to be split.
 * @param delim Pointer to the delimiter string used to split the source string.
 * @param size Maximum number of tokens to be stored in the 'dest' array.
 * @param dest Array of strings where the tokens will be stored.
 *
 * @return The number of tokens found and stored in the 'dest' array.
 */
int split(char* source, char* delim, int size, char dest[size][STR_MAX_LENGTH]) {
    int length = strlen(source);
    char temp[length + 1];
    strcpy(temp, source);

    char *ptr = temp; // Initialize ptr with temp
    int index = 0;

    while (index < size - 1) {
        char *token = strsep(&ptr, delim);
        if (token == NULL)
            break;
        strcpy(dest[index++], token);
    }

    strcpy(dest[index++], ptr);

    return index;
}

int constrain_color(int color, int maxval) {
    if (color < 0) return 0;
    if (color > maxval) return maxval;

    return color;
}

int meta_data_lire(char* comment, struct MetaData *p_metadonnees) {
    char data[3][STR_MAX_LENGTH];

    if (split(comment, ";", 3, data) != 3) {
        printf("%d\n", split(comment, ";", 3, data));
        return ERREUR_FORMAT;
    }

    if (length(data[0]) >= MAX_CHAINE || length(data[1]) >= MAX_CHAINE || length(data[2]) >= MAX_CHAINE) {
        return ERREUR_FORMAT;
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

    if (fscanf(file, "P%d", &p_info->type) != 1) return ERREUR_FORMAT;
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
    if (maxval > MAX_VALEUR) return ERREUR_FORMAT;
    if (lignes < 0 || lignes > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes < 0 || colonnes > MAX_LARGEUR) return ERREUR_TAILLE;

    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            matrice[i][j] = constrain_color(matrice[i][j] + valeur, maxval);
        }
    }

    return OK;
}

int pgm_creer_negatif(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes, int colonnes, int maxval) {
    if (maxval > MAX_VALEUR) return ERREUR_FORMAT;
    if (lignes < 0 || lignes > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes < 0 || colonnes > MAX_LARGEUR) return ERREUR_TAILLE;

    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            matrice[i][j] = constrain_color(maxval - matrice[i][j], maxval);
        }
    }

    return OK;
}

int pgm_extraire(int matrice[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int lignes2, int colonnes2, int *p_lignes, int *p_colonnes) {
	int width = colonnes2 - colonnes1 + 1;
	int height = lignes2 - lignes1 + 1;
	
	if (*p_lignes < 0 || *p_lignes > MAX_HAUTEUR) return ERREUR;
    if (*p_colonnes < 0 || *p_colonnes > MAX_LARGEUR) return ERREUR;
    
    if (height < 0 || height > MAX_HAUTEUR) return ERREUR;
    if (width < 0 || width > MAX_LARGEUR) return ERREUR;
    
    if (lignes1 < 0 || lignes1 > MAX_HAUTEUR) return ERREUR;
    if (colonnes1 < 0 || colonnes1 > MAX_LARGEUR) return ERREUR;
    
    if (lignes2 < 0 || lignes2 > MAX_HAUTEUR) return ERREUR;
    if (colonnes2 < 0 || colonnes2 > MAX_LARGEUR) return ERREUR;
    
    if (height >= *p_lignes) return ERREUR;
    if (width >= *p_colonnes) return ERREUR;
   
    
    int matriceCopy[MAX_HAUTEUR][MAX_LARGEUR];
    memcpy(matriceCopy, matrice, MAX_HAUTEUR * MAX_LARGEUR * sizeof(int));

    for (int i = lignes1; i <= lignes2; i++) {
        for (int j = colonnes1; j <= colonnes2; j++) {
            matrice[i - lignes1][j - colonnes1] = matriceCopy[i][j]; 
        }
    }
    
    *p_lignes = height;
    *p_colonnes = width;

    return OK;
}

int pgm_sont_identiques(int matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, int matrice2[MAX_HAUTEUR][MAX_LARGEUR], int lignes2, int colonnes2) {
    if (lignes2 < 0 || lignes2 > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes2 < 0 || colonnes2 > MAX_LARGEUR) return ERREUR_TAILLE;
    
    if (lignes1 != lignes2 || colonnes1 != colonnes2) return 1;

    for (int i = 0; i < lignes1; i++)
        for (int j = 0; j < colonnes1; j++)
            if (matrice1[i][j] != matrice2[i][j]) return 1;

    return 0;
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
    if (lignes1 < 0 || lignes1 > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes1 < 0 || colonnes1 > MAX_LARGEUR) return ERREUR_TAILLE;

    memcpy(matrice2, matrice1, sizeof(struct RGB) * MAX_HAUTEUR * MAX_LARGEUR);
    *p_lignes2 = lignes1;
    *p_colonnes2 = colonnes1;

    return OK;
}

int ppm_sont_identiques(struct RGB matrice1[MAX_HAUTEUR][MAX_LARGEUR], int lignes1, int colonnes1, struct RGB matrice2[MAX_HAUTEUR][MAX_LARGEUR], int lignes2, int colonnes2) {
    if (lignes1 < 0 || lignes1 > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes1 < 0 || colonnes1 > MAX_LARGEUR) return ERREUR_TAILLE;
    if (lignes2 < 0 || lignes2 > MAX_HAUTEUR) return ERREUR_TAILLE;
    if (colonnes2 < 0 || colonnes2 > MAX_LARGEUR) return ERREUR_TAILLE;
    
    if (lignes1 != lignes2 || colonnes1 != colonnes2) return 1;

    for (int i = 0; i < lignes1; i++)
        for (int j = 0; j < colonnes1; j++) {
            if (matrice1[i][j].valeurR != matrice2[i][j].valeurR) return 1;
            if (matrice1[i][j].valeurG != matrice2[i][j].valeurG) return 1;
            if (matrice1[i][j].valeurB != matrice2[i][j].valeurB) return 1;
        }

    return 0;
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

    return OK;
}
