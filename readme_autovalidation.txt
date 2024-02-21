Les détails à propos de l'autovalidation sont dans la documentation.

La commande avec Geany pour construire avec l'autovalidation est la suivante:

gcc gestion_images.c string_x.c bibliotheque_images.c -L. -l AutoValidation -o gestion_images

gcc -static gestion_images.c string_x.c bibliotheque_images.c -o gestion_images

gcc -static gestion_images.c string_x.c bibliotheque_images.c -L. -lAutoValidation -o gestion_images