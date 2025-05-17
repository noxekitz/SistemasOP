#include <stdio.h>
#include <stdlib.h>
#include "red-black-tree.h"
#include "estadisticas.h"
#include "ficheros-csv.h"

/**
 *
 * Main function.
 *
 */

int main(int argc, char **argv) {
    FILE *fp;

    if (argc != 4) {
        printf("Use: %s <filename.csv> <llista_aeroports.csv> <aeroport>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Abrimos el fichero con la lista de aeropuertos */
    fp = fopen(argv[2], "r");
    if (!fp) {
        printf("Could not open file '%s'\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    /* TODO
	 * Inicializar árbol y leer los datos del fichero de aeropuertos */
    rb_tree *tree = create_tree(argv[2], argv[1]);

    fclose(fp);

    /* Abrimos el fichero con los datos de los vuelos */
    fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Could not open file '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* TODO 
	 * Leer los datos e introducirlos en el árbol */
    fclose(fp);

    /* TODO
	Realizar e imprimir estadísticas según los métodos de estadisticas.c */
	
    printf("Media de retardos para %s\n", argv[3]);

    printf("\nAeropuerto con mas destinos\n");

    return EXIT_SUCCESS;