// Exercice8.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "Exercice1.h"
#include "Exercice2.h"
#include "Exercice4.h"
#include "Exercice5.h"
#include "Exercice6.h"
#include "Exercice7.h"
#include "Exercice8.h"

// EXERCICE 8.1 
void* segment_override_addressing(CPU* cpu, const char* operand) {
    if (operand == NULL) return NULL;

    // Vérifie que l'opérande est du type [XX:YY]
    if (!matches("^\[[A-Z]{2}:[A-Z]{2}\\]$", operand)) return NULL;

    // Copie de l'opérande pour ne pas modifier l'original
    char* temp = strdup(operand);
    if (!temp) return NULL;

    // Retire les crochets [ ]
    temp[strlen(temp) - 1] = '\0'; // enlève le dernier ]
    char* inside = temp + 1;         // saute le premier [

    // Sépare sur ':'
    char* segment_name = inside;
    char* register_name = strchr(inside, ':');
    if (!register_name) {
        free(temp);
        return NULL;
    }

    *register_name = '\0'; // coupe la chaîne pour obtenir le segment
    register_name++;        // avance sur le registre

    // Cherche le segment
    Segment* seg = hashmap_get(cpu->memory_handler->allocated, segment_name);
    if (!seg) {
        free(temp);
        return NULL;
    }

    // Cherche le registre
    int* reg = hashmap_get(cpu->context, register_name);
    if (!reg) {
        free(temp);
        return NULL;
    }

    // Vérifie que la position est dans le segment
    int offset = *reg;
    if (offset < 0 || offset >= seg->size) {
        free(temp);
        return NULL;
    }

    // Récupère la valeur dans le segment
    void* value = cpu->memory_handler->memory[seg->start + offset];

    free(temp);
    return value;
}

// EXERCICE 8.3 

int find_free_address_strategy(MemoryHandler* handler, int size, int strategy) {
    if (!handler || size <= 0) return -1; // Vérifie si les paramètres sont valides

    Segment* current = handler->free_list; // Pointeur pour parcourir la liste de segments libres
    Segment* best = NULL; // Sert pour Best Fit et Worst Fit

    if (strategy == 0) { // First Fit : premier segment assez grand
        while (current) {
            if (current->size >= size) {
                return current->start; // Retourne immédiatement l'adresse trouvée
            }
            current = current->next; // Passe au segment suivant
        }
    }
    else if (strategy == 1) { // Best Fit : segment avec l'espace le plus proche
        int min_diff = -1;
        while (current) {
            if (current->size >= size) {
                int diff = current->size - size; // Différence entre espace disponible et demandé
                if (min_diff == -1 || diff < min_diff) {
                    min_diff = diff;
                    best = current; // Garde le meilleur candidat
                }
            }
            current = current->next;
        }
        if (best) return best->start; // Retourne le meilleur segment trouvé
    }
    else if (strategy == 2) { // Worst Fit : plus grand espace possible
        int max_diff = -1;
        while (current) {
            if (current->size >= size) {
                int diff = current->size - size;
                if (diff > max_diff) {
                    max_diff = diff;
                    best = current; // Garde le plus grand espace libre
                }
            }
            current = current->next;
        }
        if (best) return best->start;
    }

    return -1; // Aucun segment disponible
}

// EXERCICE 8.5

// Cette fonction alloue dynamiquement un segment ES
int alloc_es_segment(CPU* cpu) {
    if (!cpu) return -1; // Vérifie si le CPU existe

    // Récupère les registres AX (taille), BX (stratégie) et ES (adresse résultat)
    int* ax = hashmap_get(cpu->context, "AX");
    int* bx = hashmap_get(cpu->context, "BX");
    int* es = hashmap_get(cpu->context, "ES");
    if (!ax || !bx || !es) return -1; // Vérifie que tout est trouvé

    int size = *ax; // Taille du segment à allouer
    int strategy = *bx; // Stratégie d'allocation

    // Trouve une adresse libre 
    int address = find_free_address_strategy(cpu->memory_handler, size, strategy);
    if (address == -1) {
        cpu->ZF = 1; // Si aucun espace, on met ZF=1 (échec)
        return -1;
    }

    // Crée un segment nommé "ES" à l'adresse trouvée
    if (create_segment(cpu->memory_handler, "ES", address, size) != 0) {
        cpu->ZF = 1;
        return -1;
    }

    // Initialise tout le segment avec des 0
    for (int i = 0; i < size; i++) {
        int* value = (int*) malloc(sizeof(int));
        *value = 0;
        store(cpu->memory_handler, "ES", i, value);
    }

    *es = address; // Stocke l'adresse du segment dans ES
    cpu->ZF = 0; // Allocation réussie : ZF=0

    return 0;
}

// EXERCICE 8.6 

// Cette fonction libère complètement le segment ES (mémoire dynamique).
int free_es_segment(CPU* cpu) {
    if (!cpu) return -1; // Vérifie que le CPU existe

    int* es = hashmap_get(cpu->context, "ES"); // Récupère le registre ES
    if (!es || *es == -1) return -1; // Si pas de segment ES alloué, rien à faire

    Segment* seg = hashmap_get(cpu->memory_handler->allocated, "ES"); // Récupère le segment ES
    if (!seg) return -1; // Si pas trouvé, erreur

    // Libère chaque case mémoire du segment
    for (int i = 0; i < seg->size; i++) {
        int index = seg->start + i;
        if (cpu->memory_handler->memory[index]) {
            free(cpu->memory_handler->memory[index]); // Libère la valeur
            cpu->memory_handler->memory[index] = NULL; // Remet à NULL
        }
    }

    remove_segment(cpu->memory_handler, "ES"); // Supprime ES de la table des segments
    *es = -1; // Réinitialise le registre ES à -1

    return 0;
}
