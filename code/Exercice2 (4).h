#ifndef EXERCICE2_H
#define EXERCICE2_H

#include "Exercice1.h"  // Nécessaire pour utiliser la structure HashMap dans MemoryHandler


typedef struct segment {
     int start;              // Position de début du segment dans la mémoire (indice dans le tableau)
     int size;               // Taille du segment en unités (par exemple : 1 unité = 1 case)
     struct segment *next;   // Pointeur vers le segment libre suivant (liste chaînée)
} Segment;



typedef struct memoryHandler {
    void **memory;           // C’est le tableau de mémoire simulée (chaque case = 1 unité)
    int total_size;          // Taille totale de la mémoire simulée (en nombre de cases)
    Segment *free_list;      // Liste chaînée des segments libres → permet de savoir où on peut allouer
    HashMap *allocated;   
    HashMap *constant_pool;   
} MemoryHandler;




// Question 2.1
MemoryHandler *memory_init(int size);

// Question 2.2
Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment** prev);

// Question 2.3
int create_segment(MemoryHandler *handler, const char *name, int start, int size);

// Question 2.4 
int remove_segment(MemoryHandler *handler, const char *name);


#endif
