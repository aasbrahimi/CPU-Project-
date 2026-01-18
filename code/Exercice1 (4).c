// Exercice1.c 

#include "Exercice1.h"



#include <stdio.h>      // Pour printf
#include <stdlib.h>     // Pour malloc, free
#include <string.h>     // Pour strcmp, strdup


#define TABLE_SIZE 128           
#define TOMBSTONE ((void*) -1)              



// EXERCICE 1.1

unsigned long hash(const char *str) {
    unsigned long hash = 0;  // Résultat
    int c;
    while ((c = *str++)) {
        hash = hash * 31 + c;  // On multiplie et on ajoute le caractère
    }
    return hash % TABLE_SIZE;  // On ramène à la taille de la table
}


// EXERCICE 1.2

HashMap* hashmap_create() {
    HashMap* map = (HashMap*) malloc(sizeof(HashMap));     // Allocation de la structure principale
    if (!map) return NULL;                                  // Vérification de l'allocation
    map->size = TABLE_SIZE;                                 // On fixe la taille
    map->table = (HashEntry*) calloc(TABLE_SIZE, sizeof(HashEntry));  // On initialise toutes les entrées à NULL
    if (!map->table) {
        free(map);
        return NULL;
    }
    return map;
}


// EXERCICE 1.3 

int hashmap_insert(HashMap* map, const char* key, void* value) {
    unsigned long idx = hash(key);      // On calcule l'index de départ
    for (int i = 0; i < map->size; i++) {
        int current = (idx + i) % map->size;   // Sondage linéaire
        HashEntry* entry = &map->table[current];

        // Si la case est vide ou marquée TOMBSTONE
        if (entry->key == NULL || entry->value == TOMBSTONE) {
            entry->key = strdup(key);    // On copie la clé
            entry->value = value;        // On stocke la valeur
            return 1;                    // Succès
        }

        // Si la clé existe déjà, on met à jour
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 1;
        }
    }
    return 0; // Échec si la table est pleine
}


// EXERCICE 1.4 

void* hashmap_get(HashMap* map, const char* key) {
    unsigned long idx = hash(key);    // Index de départ
    for (int i = 0; i < map->size; i++) {
        int current = (idx + i) % map->size;
        HashEntry* entry = &map->table[current];

        // Case vide => fin de chaîne
        if (entry->key == NULL && entry->value != TOMBSTONE) {
            return NULL;
        }

        // Si la clé est trouvée
        if (entry->key != NULL && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }
    return NULL;  // Pas trouvé
}


// EXERCICE 1.5 

int hashmap_remove(HashMap* map, const char* key) {
    unsigned long idx = hash(key);   // Index de départ
    for (int i = 0; i < map->size; i++) {
        int current = (idx + i) % map->size;
        HashEntry* entry = &map->table[current];

        // Si la clé correspond
        if (entry->key != NULL && strcmp(entry->key, key) == 0) {
            free(entry->key);              // Libérer la clé
            entry->key = NULL;
            entry->value = TOMBSTONE;      // Marquer la case
            return 1;                      // Succès
        }
    }
    return 0;  // Échec si la clé n'a pas été trouvée
}


// EXERCICE 1.6

void hashmap_destroy(HashMap* map) {
    if (!map) return;
    for (int i = 0; i < map->size; i++) {
        HashEntry* entry = &map->table[i];
        if (entry->key != NULL) {
            free(entry->key);    // On libère les clés
        }
        // Les valeurs génériques ne sont pas libérées ici,
        // car on ne sait pas ce qu'elles pointent.
    }
    free(map->table);   // Libération du tableau
    free(map);          // Libération de la structure principale
}
