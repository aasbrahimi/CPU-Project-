#include <stdio.h>     // Pour printf
#include <stdlib.h>    // Pour malloc, free
#include "Exercice7.h" 

/// EXERCICE 7.2 
int push_value(CPU* cpu, int value) {
    // Vérifie que le CPU existe
    if (cpu == NULL) {
        return -1;
    }

    // Récupère le registre SP (pointeur de pile)
    int* sp = hashmap_get(cpu->context, "SP");
    if (sp == NULL) {
        return -1;
    }

    // Récupère le segment de pile "SS"
    Segment* stack_segment = hashmap_get(cpu->memory_handler->allocated, "SS");
    if (stack_segment == NULL) {
        return -1;
    }

    // Vérifie si la pile est pleine
    if (*sp < stack_segment->start) {
        printf("Erreur : pile pleine\n");
        return -1;
    }

    // Calcule l'adresse mémoire où mettre la valeur
    int adresse = stack_segment->start + *sp;

    // Alloue de la mémoire pour stocker la valeur
    int* case_mem = malloc(sizeof(int));
    if (case_mem == NULL) {
        return -1;
    }

    // Stocke la valeur à empiler
    *case_mem = value;

    // Place la valeur dans la mémoire du CPU
    cpu->memory_handler->memory[adresse] = case_mem;

    // Décrémente SP car la pile est descendante
    (*sp)--;


    return 0;
}

// EXERCICE 7.2 
int pop_value(CPU* cpu, int* dest) {
    // Vérifie que le CPU et dest existent
    if (cpu == NULL || dest == NULL) {
        return -1;
    }

    // Récupère le registre SP
    int* sp = hashmap_get(cpu->context, "SP");
    if (sp == NULL) {
        return -1;
    }

    // Récupère le segment de pile "SS"
    Segment* stack_segment = hashmap_get(cpu->memory_handler->allocated, "SS");
    if (stack_segment == NULL) {
        return -1;
    }

    // Vérifie si la pile est vide
    if (*sp >= stack_segment->start + stack_segment->size - 1) {
        printf("Erreur : pile vide\n");
        return -1;
    }

    // Incrémente SP pour pointer sur la valeur à dépiler
    (*sp)++;

    // Calcule l'adresse mémoire où lire la valeur
    int adresse = stack_segment->start + *sp;

    // Récupère la valeur depuis la mémoire
    int* case_mem = (int*)cpu->memory_handler->memory[adresse];
    if (case_mem == NULL) {
        printf("Erreur : pas de valeur à cette adresse\n");
        return -1;
    }

    // Copie la valeur dépilée dans dest
    *dest = *case_mem;

    // Libère la case mémoire
    free(case_mem);
    cpu->memory_handler->memory[adresse] = NULL;

   
    return 0;
}
