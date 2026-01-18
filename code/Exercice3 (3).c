#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <math.h>       
#include <stdint.h>    
#include <assert.h>     
#include "Exercice3.h"  
#include "Exercice1.h"  // Pour la table de hachage
#include "Exercice2.h"  // Pour le gestionnaire mémoire


// Fonction : parse_data_instruction

Instruction *parse_data_instruction(const char *line, HashMap *memory_locations) {
    Instruction *instruc = (Instruction *)malloc(sizeof(Instruction)); // alloue une nouvelle structure Instruction
    if (!instruc) return NULL; // vérifie si l'allocation a échoué

    char *copy = strdup(line); // on fait une copie de la ligne pour pouvoir la modifier avec strtok
    if (!copy) {
        free(instruc); // si l'allocation de copy échoue, on libère ce qu'on avait déjà fait
        return NULL;
    }

    // On découpe la ligne avec des espaces : exemple "X DW 5"
    char *mnemonic = strtok(copy, " ");  // nom de la variable (X)
    char *oper1 = strtok(NULL, " ");     // type de donnée (DW)
    char *oper2 = strtok(NULL, " ");     // valeur (5)

    // On copie chaque élément dans la structure Instruction
    if (mnemonic != NULL) {
        instruc->mnemonic = strdup(mnemonic);
        if (!instruc->mnemonic) {
            free(copy); free(instruc); return NULL;
        }
    }

    if (oper1 != NULL) {
        instruc->operand1 = strdup(oper1);
        if (!instruc->operand1) {
            free(instruc->mnemonic); free(copy); free(instruc); return NULL;
        }
    }

    if (oper2 != NULL) {
        instruc->operand2 = strdup(oper2);
        if (!instruc->operand2) {
            free(instruc->mnemonic); free(instruc->operand1); free(copy); free(instruc); return NULL;
        }
    }

    // Vérification finale si une des 3 allocations a échoué
    if (!instruc->mnemonic || !instruc->operand1 || !instruc->operand2) {
        free(instruc->mnemonic); free(instruc->operand1); free(instruc->operand2);
        free(instruc); free(copy); return NULL;
    }

    // On utilise une variable statique pour que la valeur de "adress" soit mémorisée entre les appels
    static int adress = 0;

    // On insère la variable (mnemonic) avec son adresse dans la table de hachage
    hashmap_insert(memory_locations, mnemonic, (void *)(intptr_t)adress);

    // On incrémente l'adresse pour la prochaine variable
    adress += 1;

    free(copy); // on libère la copie de la ligne 
    return instruc; // on retourne l'instruction créée
}


// Fonction : parse_code_instruction


Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count){
    Instruction *instruc = malloc(sizeof(Instruction)); // alloue la mémoire pour une instruction
    if (!instruc) return NULL;

    char *copy = strdup(line); // copie modifiable de la ligne
    if (!copy) {
        free(instruc); return NULL;
    }

    // On initialise à NULL les champs
    instruc->mnemonic = NULL;
    instruc->operand1 = NULL;
    instruc->operand2 = NULL;

    // On découpe : exemple "loop: ADD AX, BX"
    char *label = strtok(copy, ": ");     // soit un label (loop), soit directement la mnémonique
    char *mnemonic = strtok(NULL, " ");   // mnémonique (ADD)
    char *oper1 = strtok(NULL, " ");      // premier opérande (AX)
    char *oper2 = strtok(NULL, " ");      // deuxième opérande (BX)

    if(label != NULL){
        hashmap_insert(labels, label, (void *)(intptr_t)code_count); // on associe le label à son numéro de ligne
    }

    // On copie chaque partie si elle existe
    if (mnemonic != NULL) {
        instruc->mnemonic = strdup(mnemonic);
        if (!instruc->mnemonic) {
            free(copy); free(instruc); return NULL;
        }
    }

    if (oper1 != NULL) {
        instruc->operand1 = strdup(oper1);
        if (!instruc->operand1) {
            free(instruc->mnemonic); free(copy); free(instruc); return NULL;
        }
    }

    if (oper2 != NULL) {
        instruc->operand2 = strdup(oper2);
        if (!instruc->operand2) {
            free(instruc->mnemonic); free(instruc->operand1); free(copy); free(instruc); return NULL;
        }
    }

    free(copy); // on libère la ligne copiée
    return instruc;
}


// Fonction : parse


ParserResult *parse(const char *filename){
    FILE *f = fopen(filename, "r"); // ouverture du fichier en lecture
    if (!f) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return NULL;
    }

    ParserResult *res = (ParserResult*)malloc(sizeof(ParserResult));
    if (!res) {
        printf("Erreur d'allocation mémoire pour ParserResult\n");
        fclose(f); return NULL;
    }

    // Initialisation de la structure
    res->data_instructions = NULL;
    res->data_count = 0;
    res->code_instructions = NULL;
    res->code_count = 0;
    res->labels = hashmap_create();
    res->memory_locations = hashmap_create();

    char line[256]; // pour lire chaque ligne du fichier
    int in_data = 0; // on est dans la section .DATA ?
    int in_code = 0; // ou dans la section .CODE ?

    // Lecture ligne par ligne
    while (fgets(line, sizeof(line), f)) {
        // Si la ligne est ".DATA", on passe en mode DATA
        if (strcmp(line, ".DATA\n") == 0) {
            in_data = 1;
            in_code = 0;
            continue;
        }

        // Si la ligne est ".CODE", on passe en mode CODE
        if (strcmp(line, ".CODE\n") == 0) {
            in_data = 0;
            in_code = 1;
            continue;
        }

        // Si on est dans la section DATA
        if (in_data) {
            res->data_instructions = realloc(res->data_instructions, (res->data_count + 1) * sizeof(Instruction *));
            if (!res->data_instructions) {
                printf("Erreur de réallocation pour data_instructions\n");
                return NULL;
            }

            res->data_instructions[res->data_count] = parse_data_instruction(line, res->memory_locations);
            res->data_count++;
        }

        // Si on est dans la section CODE
        if (in_code) {
            res->code_instructions = realloc(res->code_instructions, (res->code_count + 1) * sizeof(Instruction *));
            if (!res->code_instructions) {
                printf("Erreur de réallocation pour code_instructions\n");
                return NULL;
            }

            res->code_instructions[res->code_count] = parse_code_instruction(line, res->labels, res->code_count);
            res->code_count++;
        }
    }

    fclose(f); // on ferme le fichier
    return res; // on renvoie le résultat du parsing
}


// Fonction : free_parser_result


void free_parser_result(ParserResult *result){
    if (result == NULL) return;

    // Libère toutes les instructions de la section .DATA
    if (result->data_instructions) {
        for (int i = 0; i < result->data_count; i++) {
            Instruction* instr = result->data_instructions[i];
            if (instr) {
                free(instr->mnemonic);
                free(instr->operand1);
                free(instr->operand2);
                free(instr);
            }
        }
        free(result->data_instructions);
    }

    // Libère toutes les instructions de la section .CODE
    if (result->code_instructions) {
        for (int j = 0; j < result->code_count; j++) {
            Instruction* instr = result->code_instructions[j];
            if (instr) {
                free(instr->mnemonic);
                free(instr->operand1);
                free(instr->operand2);
                free(instr);
            }
        }
        free(result->code_instructions);
    }

    // Libère les tables de hachage
    if (result->labels) hashmap_destroy(result->labels);
    if (result->memory_locations) hashmap_destroy(result->memory_locations);

    free(result); // on libère la structure principale
}

