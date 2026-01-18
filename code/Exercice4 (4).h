#ifndef EXERCICE4_H
#define EXERCICE4_H

#include <stdlib.h>   // Pour malloc, free
#include <stdio.h>    // Pour printf
#include <string.h>   // Pour strdup


#include "Exercice2.h"
#include "Exercice1.h"
#include "Exercice3.h"
#include "Exercice4.h"


// Structure de l'objet CPU


typedef struct {
    MemoryHandler* memory_handler;  // Gestionnaire de mémoire (depuis l'exo 2)
    HashMap* context;               // Registres du CPU (ex: AX, BX, etc.)
    HashMap* constant_pool;         // Pool de constantes pour l’adressage immédiat
    Instruction** code_segment;  // Segment de code (tableau d'instructions)
    int code_size;               // Taille du segment de code
    int stack_pointer;              // Pointeur de pile
    int IP;                         // Pointeur d'instruction
    int ZF;                         // Zero Flag (utilisé par les comparaisons)
    int SF;                         // Sign Flag (utilisé par les comparaisons)
} CPU;


// Fonctions à implémenter dans Exercice4.c


// Fonction pour initialiser le CPU avec une mémoire de taille donnée
CPU* cpu_init(int memory_size);

// Fonction pour libérer toute la mémoire utilisée par le CPU
void cpu_destroy(CPU* cpu);

// Fonction pour stocker une donnée dans un segment nommé à une position donnée
void* store(MemoryHandler* handler, const char* segment_name, int pos, void* data);

// Fonction pour lire une donnée depuis un segment nommé à une position donnée
void* load(MemoryHandler* handler, const char* segment_name, int pos);

// Fonction pour allouer les variables définies dans la section .DATA
void allocate_variables(CPU* cpu, Instruction** data_instructions, int data_count);

// Fonction pour afficher les valeurs présentes dans le segment de données (DS)
void print_data_segment(CPU* cpu);

#endif
