
/* Codigo escrito por Lluis Garrido */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ficheros-csv.h"

/**
 *
 * Esta funcion crea el arbol a partir de los datos de los aeropuertos y de los ficheros de retardo
 *
 */

rb_tree *create_tree(char *str_airports, char *str_dades)
{
    FILE *fp;
    rb_tree *tree;

    /* 	TODO  
	 * 	Para crear el árbol debemos:
	 * 		Reservar memoria
	 * 		Abrir el fichero con la lista de aeropuertos 
     * 		Leer los datos del fichero de aeropuertos
	 * 		Abrir el fichero con los datos de los vuelos
	 * 		Leer los datos e introducirlos en el árbol 
	 */

    //Mi parte Reservar Memoria, Abbrir fichero con la lista de aeropuertos
    //Leer esos datos del fichero de aeropuertos, i lo otro lluc
    tree = (rb_tree *) malloc(sizeof(rb_tree));
    if(!tree){
        printf("Error: no memory for tree\n");
        exit(EXIT_FAILURE);
    }
    init_tree(tree);
    fp = fopen(str_airports, "r");
    if(!fp){
        printf("Could not open file '%s'\n", str_airports); 
        exit(EXIT_FAILURE);
    }
    read_airports(tree, fp);

    return tree;
}

/**
 *
 * Esta funcion lee la lista de los aeropuertos y crea el arbol 
 *
 */

void read_airports(rb_tree *tree, FILE *fp) 
{
    int i, num_airports;
    char line[MAXCHAR];
    node_data *n_data;

    /*
     * eow es el caracter de fin de palabra
     */
    char eow = '\0';


    fgets(line, 100, fp);
    num_airports = atoi(line);

    i = 0;
    while (i < num_airports)
    {
        fgets(line, 100, fp);
        line[3] = eow; 

        /*	TODO  
		 *  Reservamos memoria para el nodo 
		 *  Copiamos los datos al nodo
		 *  Incializamos la lista
		 *  Insertamos el nodo suponiendo que los nodos son únicos, no hace falta
         *  comprobar si ya existen previamente.
         */
        n_data = malloc(sizeof(node_data));
        if(!n_data){
            printf("Error: no memory for node_data\n");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        n_data->key = strdup(line);
        if(!n_data->key){
            printf("Error: no memory for key\n");
            free(n_data);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        n_data->destinations = NULL;
        insert_node(tree, n_data);

        i++;
    }
}

/**
 * Función que permite leer todos los campos de la línea de vuelo: origen,
 * destino, retardo.
 * 		
 */

static int extract_fields_airport(char *line, flight_information *fi) {

    /*Recorre la linea por caracteres*/
    char caracter;
    /* i sirve para recorrer la linea
     * iterator es para copiar el substring de la linea a char
     * coma_count es el contador de comas
     */
    int i, iterator, coma_count;
    /* start indica donde empieza el substring a copiar
     * end indica donde termina el substring a copiar
     * len indica la longitud del substring
     */
    int start, end, len;
    /* invalid nos permite saber si todos los campos son correctos
     * 1 hay error, 0 no hay error pero no hemos terminado
     */
    int invalid = 0;
    /*
     * eow es el caracter de fin de palabra
     */
    char eow = '\0';
    /*
     * contenedor del substring a copiar
     */
    char *word;
    /*
     * Inicializamos los valores de las variables
     */
    start = 0;
    end = -1;
    i = 0;
    coma_count = 0;
    /*
     * Empezamos a contar comas
     */
    do {
        caracter = line[i++];
        if (caracter == ',') {
            coma_count ++;
            /*
             * Cogemos el valor de end
             */
            end = i;
            /*
             * Si es uno de los campos que queremos procedemos a copiar el substring
             */
            if(coma_count == ATRASO_LLEGADA_AEROPUERTO || 
                    coma_count == AEROPUERTO_ORIGEN || 
                    coma_count == AEROPUERTO_DESTINO){
                /*
                 * Calculamos la longitud, si es mayor que 1 es que tenemos 
                 * algo que copiar
                 */
                len = end - start;
                if (len > 1) {
                    /*
                     * Alojamos memoria y copiamos el substring
                     */
                    word =(char*)malloc(sizeof(char)*(len));
                    for(iterator = start; iterator < end-1; iterator ++){
                        word[iterator-start] = line[iterator];
                    }
                    /*
                     * Introducimos el caracter de fin de palabra
                     */
                    word[iterator-start] = eow;
                    /*
                     * Comprobamos que el campo no sea NA (Not Available) 
                     */
                    if (strcmp("NA", word) == 0)
                        invalid = 1;
                    else {
                        switch (coma_count) {
                            case ATRASO_LLEGADA_AEROPUERTO:
                                fi->delay = atoi(word);
                                break;
                            case AEROPUERTO_ORIGEN:
                                strcpy(fi->origin, word);
                                break;
                            case AEROPUERTO_DESTINO:
                                strcpy(fi->destination, word);
                                break;
                            default:
                                printf("ERROR in coma_count\n");
                                exit(1);
                        }
                    }

                    free(word);

                } else {
                    /*
                     * Si el campo esta vacio invalidamos la linea entera 
                     */

                    invalid = 1;
                }
            }
            start = end;
        }
    } while (caracter && invalid==0);

    return invalid;
}

/**
 *
 * Esta funcion lee los datos de los vuelos y los inserta en el 
 * arbol (previamente creado)
 *
 */

void read_airports_data(rb_tree *tree, FILE *fp) {
    char line[MAXCHAR];
    int invalid;

    flight_information fi;


    /* Leemos la cabecera del fichero */
    fgets(line, MAXCHAR, fp);
    
    while (fgets(line, MAXCHAR, fp) != NULL)
    {
        invalid = extract_fields_airport(line, &fi);

        if (!invalid) {
            
			/* TODO  
			 * Inserción de datos en el arbol 
			 */

            } else {
                printf("ERROR: aeropuerto %s no encontrado en el arbol.\n", fi.origin);
                exit(1);
            }
        }
    }



