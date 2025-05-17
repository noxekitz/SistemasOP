#pragma once

#include "linked-list.h"
#include "red-black-tree.h"

#define MAXCHAR 500

#define ATRASO_LLEGADA_AEROPUERTO 15
#define AEROPUERTO_ORIGEN 17
#define AEROPUERTO_DESTINO 18

typedef struct flight_information {
    char origin[4];
    char destination[4];
    int delay;
} flight_information;

void read_airports(rb_tree *tree, FILE *fp);
void read_airports_data(rb_tree *tree, FILE *fp);
