#ifndef EXERCICE3_H
#define EXERCICE3_H

#include "Exercice1.h"  // Pour la HashMap
#include <stdio.h>      // Pour FILE, printf
#include <stdlib.h>     // Pour malloc, free
#include <string.h>     // Pour strdup, strncmp


// STRUCTURE DE DONNÉES POUR UNE INSTRUCTION


typedef struct {
    char* mnemonic;   // Le nom de l'instruction (ex : MOV, ADD)
    char* operand1;   // Le premier opérande (ex : AX)
    char* operand2;   // Le second opérande (ex : 42 ou BX)
} Instruction;


// STRUCTURE POUR STOCKER LE RÉSULTAT DU PARSING


typedef struct {
    Instruction** data_instructions;  // Tableau d'instructions .DATA
    int data_count;                   // Nombre d'instructions .DATA

    Instruction** code_instructions;  // Tableau d'instructions .CODE
    int code_count;                   // Nombre d'instructions .CODE

    HashMap* memory_locations;        // Table des variables et leur adresse
    HashMap* labels;                  // Table des labels de code
} ParserResult;



// (EXERCICE 3.1)

Instruction* parse_data_instruction(const char* line, HashMap* memory_locations);


// (EXERCICE 3.2)

Instruction* parse_code_instruction(const char* line, HashMap* labels, int code_count);



// QUI LIT UN FICHIER ET STOCKE LES INSTRUCTIONS
// (EXERCICE 3.3)

ParserResult* parse(const char* filename);



// (EXERCICE 3.5)

void free_parser_result(ParserResult* result);

#endif
