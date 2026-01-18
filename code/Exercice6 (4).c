#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Exercice1.h"
#include "Exercice2.h"
#include "Exercice4.h"
#include "Exercice5.h"
#include "Exercice6.h"
#include "Exercice7.h"
#include "Exercice8.h"

// Fonction trim() : enlève les espaces au début et à la fin d'une chaîne
char* trim(char* str) {
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;
    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
    return str;
    
}
// Fonction search_and_replace : remplace toutes les occurrences de clés du HashMap dans une chaîne par leur valeur numérique
int search_and_replace(char **str, HashMap *values) {
    // Vérifie que les pointeurs ne sont pas NULL
    if (!str || !*str || !values) return 0;

    int replaced = 0; // Variable pour savoir si on a remplacé quelque chose
    char *input = *str; // Chaîne d'entrée

    // Parcourt toutes les entrées de la table de hachage
    for (int i = 0; i < values->size; i++) {
        // Vérifie si une clé existe à cet emplacement
        if (values->table[i].key && values->table[i].key != (void *)-1) {
            char *key = values->table[i].key;
            int value = (int)(long)values->table[i].value;

            // Cherche si la clé apparaît dans la chaîne
            char *substr = strstr(input, key);
            if (substr) {
                // Prépare le remplacement
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", value);

                // Calcule les longueurs
                int key_len = strlen(key);
                int repl_len = strlen(replacement);
                int remain_len = strlen(substr + key_len);

                // Alloue une nouvelle chaîne assez grande
                char *new_str = (char *)malloc((substr - input) + repl_len + remain_len + 1);
                if (!new_str) {
                    return 0; // Échec d'allocation mémoire
                }

                // Copie le début avant la clé
                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';

                // Ajoute le remplacement
                strcat(new_str, replacement);

                // Ajoute la suite après la clé
                strcat(new_str, substr + key_len);

                // Libère l'ancienne chaîne et met à jour
                free(input);
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    // Nettoie la chaîne finale (trim)
    if (replaced) {
        char *trimmed = trim(input);
        if (trimmed != input) {
            *str = strdup(trimmed);
            free(input);
        }
    }

    return replaced;
}
//Exercie 6.1
int resolve_constants(ParserResult *result) {
    if (!result) return 0;

    // Applique aux instructions .DATA
    for (int i = 0; i < result->data_count; i++) {
        Instruction *inst = result->data_instructions[i];
        search_and_replace(&inst->mnemonic, result->memory_locations);
        search_and_replace(&inst->operand1, result->memory_locations);
        search_and_replace(&inst->operand2, result->memory_locations);
    }

    // Applique aux instructions .CODE
    for (int i = 0; i < result->code_count; i++) {
        Instruction *inst = result->code_instructions[i];
        search_and_replace(&inst->mnemonic, result->labels);
        search_and_replace(&inst->operand1, result->labels);
        search_and_replace(&inst->operand2, result->labels);
    }

    return 1;
}


// 6.2
CPU* cpu_init_ex6(int memory_size) {
    return cpu_init(memory_size);
}

// 6.3 : Fonction allocate_code_segment() : donne un tableau d'instructions au CPU
void allocate_code_segment(CPU* cpu, Instruction** program, int count) {
    cpu->code_segment = program;
    cpu->code_size = count;
}

// 6.4 + 7.3 + 8.
void handle_instruction(CPU* cpu, Instruction* instr, void* src, void* dest) {
    if (strcmp(instr->mnemonic, "MOV") == 0) {
        handle_mov(cpu, src, dest); // copie une valeur
    }
    else if (strcmp(instr->mnemonic, "ADD") == 0) {
        if (src != NULL && dest != NULL) {
            *((int*)dest) += *((int*)src); // additionne src à dest
        }
    }
    else if (strcmp(instr->mnemonic, "CMP") == 0) {
        if (src != NULL && dest != NULL) {
            int result = *((int*)dest) - *((int*)src); // compare dest et src
            cpu->ZF = (result == 0); // ZF si égalité
            cpu->SF = (result < 0);  // SF si résultat négatif
        }
    }
    else if (strcmp(instr->mnemonic, "JMP") == 0) {
        if (src != NULL) {
            cpu->IP = *((int*)src); 
        }
    }
    else if (strcmp(instr->mnemonic, "JZ") == 0) {
        if (src != NULL && cpu->ZF == 1) {
            cpu->IP = *((int*)src); // saut si ZF actif
        }
    }
    else if (strcmp(instr->mnemonic, "JNZ") == 0) {
        if (src != NULL && cpu->ZF == 0) {
            cpu->IP = *((int*)src); // saut si ZF désactivé
        }
    }
    else if (strcmp(instr->mnemonic, "HALT") == 0) {
        cpu->IP = -1; // arrêt du programme
    }
    else if (strcmp(instr->mnemonic, "PUSH") == 0) {
        if (src != NULL) {
            int val = *((int*)src);
            push_value(cpu, val); // empile la valeur
        }
    }
    else if (strcmp(instr->mnemonic, "POP") == 0) {
        if (dest != NULL) {
            int val = 0;
            if (pop_value(cpu, &val) == 0) {
                *((int*)dest) = val; // dépile une valeur
            }
        }
    }
    else if (strcmp(instr->mnemonic, "ALLOC") == 0) {
        alloc_es_segment(cpu); // alloue un segment ES
    }
    else if (strcmp(instr->mnemonic, "FREE") == 0) {
        free_es_segment(cpu); // libère le segment ES
    }
}

// 6.6 
Instruction* fetch_next_instruction(CPU* cpu) {
    if (cpu->IP < 0 || cpu->IP >= cpu->code_size) return NULL; // fin du programme
    return cpu->code_segment[cpu->IP++]; // passe à l'instruction suivante
}

// 6.5 :
int execute_instruction(CPU* cpu, Instruction* instr) {
    // Cherche à résoudre l'opérande source (operand2) en mode immédiat
    void *src = immediate_addressing(cpu, instr->operand2);

    // Si pas trouvé en immédiat, essaie l'adressage par registre
    if (!src) src = register_addressing(cpu, instr->operand2);

    
    if (!src) src = direct_addressing(cpu, instr->operand2);

    
    if (!src) src = indirect_addressing(cpu, instr->operand2);

    // Cherche à résoudre l'opérande destination (operand1) en mode registre
    void *dest = register_addressing(cpu, instr->operand1);

    // Si pas trouvé, essaie adressage direct
    if (!dest) dest = direct_addressing(cpu, instr->operand1);

    if (!dest) dest = indirect_addressing(cpu, instr->operand1);

    // Applique l'instruction sur les adresses trouvées
    handle_instruction(cpu, instr, src, dest);

    return 0;
}

// 6.7 : 

int run_program(CPU* cpu) {
    printf("État initial :\n");
    print_data_segment(cpu);

    Instruction* instr;
    while ((instr = fetch_next_instruction(cpu)) != NULL) {
        // Affiche l'instruction en cours
        printf("Instruction : %s %s %s\n", instr->mnemonic, instr->operand1, instr->operand2);

        // Affiche la demande d'appuyer sur Entrée ou quitter
        printf("Appuyez sur Entrée pour continuer (q pour quitter)...\n");

        // Lire ce que l'utilisateur tape
        int c = getchar();
        if (c == 'q' || c == 'Q') {
            printf("Arrêt du programme.\n");
            break; // sortir de la boucle si q est tapé
        }

        execute_instruction(cpu, instr);
    }

    printf("État final :\n");
    print_data_segment(cpu);
    return 0;
}
