// Exercice1.h

// Contient les définitions des structures et fonctions


#ifndef EXERCICE1_H
#define EXERCICE1_H

#include <stdlib.h>

#define TABLE_SIZE 128              // Taille fixe de la table
#define TOMBSTONE ((void*) -1)      // Marqueur pour les suppressions


// Structure d'une entrée de hachage

typedef struct hashentry {
    char* key;      // Clé (nom de la variable)
    void* value;    // Valeur associée
} HashEntry;


// Structure de la table de hachage

typedef struct hashmap {
    int size;           // Taille totale de la table
    HashEntry* table;   // Tableau des entrées
} HashMap;


// Déclarations des fonctions de l'exercice 1



unsigned long hash(const char* str);

// Exercice 1.2
HashMap* hashmap_create();

// Exercice 1.3
int hashmap_insert(HashMap* map, const char* key, void* value);

// Exercice 1.4
void* hashmap_get(HashMap* map, const char* key);

// Exercice 1.5 
int hashmap_remove(HashMap* map, const char* key);

// Exercice 1.6 
void hashmap_destroy(HashMap* map);

#endif
