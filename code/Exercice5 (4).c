#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

#include "Exercice1.h"
#include "Exercice2.h"
#include "Exercice4.h"
#include "Exercice5.h"
#include "Exercice8.h"

// Fonction matches() pour tester une regex
int matches(const char *pattern, const char *string) {
    regex_t regex;
    int result;

    result = regcomp(&regex, pattern, REG_EXTENDED); 
    if (result) {
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }

    result = regexec(&regex, string, 0, NULL, 0); // teste si la chaîne correspond
    regfree(&regex); // libère la regex

    return result == 0; // retourne 1 si ça correspond, sinon 0
}

// EXERCICE 5.2 : Adressage immédiat 
void* immediate_addressing(CPU* cpu, const char* operand) {
    if (!matches("^[0-9]+$", operand)) return NULL; // vérifie que c'est un nombre

    void* exist = hashmap_get(cpu->constant_pool, operand); // cherche si déjà dans le constant_pool
    if (exist != NULL) return exist; 

    int value = 0;
    for (int i = 0; operand[i] != '\0'; i++) {
        value = value * 10 + (operand[i] - '0'); // convertit l'opérande en entier
    }

    int* val_ptr = (int*) malloc(sizeof(int)); // alloue un int
    *val_ptr = value; // stocke la valeur

    hashmap_insert(cpu->constant_pool, strdup(operand), val_ptr); // insère dans constant_pool

    return val_ptr; // retourne pointeur vers la valeur
}

// EXERCICE 5.3 : Adressage par registre 
void* register_addressing(CPU* cpu, const char* operand) {
    if (!matches("^[A-Z]{2}$", operand)) return NULL; // vérifie nom registre
    return hashmap_get(cpu->context, operand); // récupère la valeur du registre
}

// EXERCICE 5.4 : Adressage direct
void* direct_addressing(CPU* cpu, const char* operand) {
    if (!matches("^[A-Z]{2}\\[[0-9]+\\]$", operand)) return NULL; // vérifie format

    char seg[3];
    int pos = 0;

    seg[0] = operand[0]; 
    seg[1] = operand[1]; 
    seg[2] = '\0'; 

    int i = 3; // commence après "XX["
    while (operand[i] != ']' && operand[i] != '\0') {
        pos = pos * 10 + (operand[i] - '0'); // récupère l'adresse
        i++;
    }

    return load(cpu->memory_handler, seg, pos); // charge la valeur
}

// EXERCICE 5.5 : Adressage indirect
void* indirect_addressing(CPU* cpu, const char* operand) {
    if (!matches("^\\*[A-Z]{2}\\[[0-9]+\\]$", operand)) return NULL; // vérifie format indirect
    return direct_addressing(cpu, operand + 1); // saute le '*' et utilise direct_addressing
}

// EXERCICE 5.6 : handle_mov
void handle_mov(CPU* cpu, void* src, void* dest) {
    (void)cpu; // cpu pas utilisé ici

    if (src == NULL || dest == NULL) {
        printf("Erreur handle_mov : src ou dest NULL\n");
        return;
    }

    *((int*)dest) = *((int*)src); // copie la valeur source vers destination
}

// EXERCICE 5.7 : setup_test_environment
CPU* setup_test_environment() {
    CPU* cpu = cpu_init(1024); // initialise le CPU
    if (!cpu) {
        printf("Error: CPU initialization failed\n");
        return NULL;
    }

    // récupère les registres
    int* ax = (int*) hashmap_get(cpu->context, "AX");
    int* bx = (int*) hashmap_get(cpu->context, "BX");
    int* cx = (int*) hashmap_get(cpu->context, "CX");
    int* dx = (int*) hashmap_get(cpu->context, "DX");
    int* sp = (int*) hashmap_get(cpu->context, "SP");
    int* bp = (int*) hashmap_get(cpu->context, "BP");

    if (ax) *ax = 3; // initialise AX
    if (bx) *bx = 6; // initialise BX
    if (cx) *cx = 100; // initialise CX
    if (dx) *dx = 0; // initialise DX

    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        create_segment(cpu->memory_handler, "DS", 0, 20); // crée segment DS
        for (int i = 0; i < 10; i++) {
            int* value = (int*) malloc(sizeof(int));
            *value = i * 10 + 5; // met des valeurs de test
            store(cpu->memory_handler, "DS", i, value);
        }
    }

    if (!hashmap_get(cpu->memory_handler->allocated, "SS")) {
        create_segment(cpu->memory_handler, "SS", 200, 128); // crée segment SS
    }

    if (sp) *sp = 200 + 128 - 1; // initialise SP
    if (bp) *bp = 200 + 128 - 1; // initialise BP

    printf("Test environment initialized.\n");
    return cpu;
}

// EXERCICE 5.8 : resolve_addressing
void* resolve_addressing(CPU* cpu, const char* operand) {
    void* res = immediate_addressing(cpu, operand); // teste immédiat
    if (res != NULL) return res;

    res = register_addressing(cpu, operand); // teste registre
    if (res != NULL) return res;

    res = direct_addressing(cpu, operand); // teste direct
    if (res != NULL) return res;

    res = indirect_addressing(cpu, operand); // teste indirect
    return res;
}
