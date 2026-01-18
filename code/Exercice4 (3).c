// Exercice4.c 

#include "Exercice2.h"
#include "Exercice1.h"
#include "Exercice3.h"
#include "Exercice4.h"
#include "Exercice8.h"




// EXERCICE 4.1 + EXERCICE 7.1 
CPU* cpu_init(int memory_size) {
    // Création du CPU
    CPU* cpu = malloc(sizeof(CPU));
    if (!cpu) return NULL;

    // Initialisation du gestionnaire mémoire
    cpu->memory_handler = malloc(sizeof(MemoryHandler));
    if (!cpu->memory_handler) {
        free(cpu);
        return NULL;
    }
    cpu->memory_handler->memory = calloc(memory_size, sizeof(void*));
    cpu->memory_handler->total_size = memory_size; // Correction ici : size -> total_size
    cpu->memory_handler->free_list = malloc(sizeof(Segment));
    if (!cpu->memory_handler->free_list) {
        free(cpu->memory_handler->memory);
        free(cpu->memory_handler);
        free(cpu);
        return NULL;
    }
    cpu->memory_handler->free_list->start = 0;
    cpu->memory_handler->free_list->size = memory_size;
    cpu->memory_handler->free_list->next = NULL;
    cpu->memory_handler->allocated = hashmap_create(); // Correction ici : allocations -> allocated

    // Création des tables de registres
    cpu->context = hashmap_create();
    cpu->constant_pool = hashmap_create();

    // Registres AX, BX, CX, DX initialisés à 0
    int* ax = malloc(sizeof(int)); *ax = 0;
    int* bx = malloc(sizeof(int)); *bx = 0;
    int* cx = malloc(sizeof(int)); *cx = 0;
    int* dx = malloc(sizeof(int)); *dx = 0;
    hashmap_insert(cpu->context, "AX", ax);
    hashmap_insert(cpu->context, "BX", bx);
    hashmap_insert(cpu->context, "CX", cx);
    hashmap_insert(cpu->context, "DX", dx);

    // EXERCICE 8.4 
    // Initialisation du registre ES à -1 (extra segment)
    int* es = malloc(sizeof(int));
    *es = -1;
    hashmap_insert(cpu->context, "ES", es);

    // Flags et pointeur d’instruction
    cpu->ZF = 0;
    cpu->SF = 0;
    cpu->IP = 0;

    // EXERCICE 7.1
    // Initialisation du registre SP (stack pointer)
    int* sp = malloc(sizeof(int));
    *sp = memory_size - 1;
    hashmap_insert(cpu->context, "SP", sp);

    // Initialisation du registre BP (base pointer)
    int* bp = malloc(sizeof(int));
    *bp = memory_size - 1;
    hashmap_insert(cpu->context, "BP", bp);

    // On garde aussi la valeur dans la structure
    cpu->stack_pointer = memory_size - 1;

    return cpu;
}

// 4.2 – Libérer toute la mémoire

void cpu_destroy(CPU* cpu) {
    if (!cpu) return;

    // Libération des registres (context)
    if (cpu->context) {
        for (int i = 0; i < cpu->context->size; i++) {
            if (cpu->context->table[i].key != NULL) {
                free(cpu->context->table[i].key);
            }
        }
        free(cpu->context->table);
        free(cpu->context);
    }

    // Libération du pool de constantes
    if (cpu->constant_pool) {
        for (int i = 0; i < cpu->constant_pool->size; i++) {
            if (cpu->constant_pool->table[i].key != NULL) {
                free(cpu->constant_pool->table[i].key);
                free(cpu->constant_pool->table[i].value);
            }
        }
        free(cpu->constant_pool->table);
        free(cpu->constant_pool);
    }

    // Libération de la mémoire principale
    if (cpu->memory_handler) {
        if (cpu->memory_handler->memory) {
            for (int i = 0; i < cpu->memory_handler->total_size; i++) {
                if (((void**)cpu->memory_handler->memory)[i] != NULL) {
                    free(((void**)cpu->memory_handler->memory)[i]);
                }
            }
            free(cpu->memory_handler->memory);
        }

        // Libération des segments libres
        Segment* current = cpu->memory_handler->free_list;
        while (current != NULL) {
            Segment* next = current->next;
            free(current);
            current = next;
        }

        // Libération des segments alloués
        hashmap_destroy(cpu->memory_handler->allocated);

        // Libération du gestionnaire lui-même
        free(cpu->memory_handler);
    }

    // Libération finale du CPU
    free(cpu);
}


// 4.3 

// Stocke une donnée dans un segment mémoire à une position donnée
void* store(MemoryHandler* handler, const char* segment_name, int pos, void* data) {
    Segment* seg = hashmap_get(handler->allocated, segment_name); // récupère le segment

    if (!seg || pos < 0 || pos >= seg->size) return NULL; // vérifie si segment ou position invalide

    int position_mem = seg->start + pos; // calcule la position réelle en mémoire

    ((void**)handler->memory)[position_mem] = data; // stocke la donnée

    return data; // retourne ce qu'on a stocké
}

// 4.4 

// Lit une donnée stockée dans un segment mémoire à une position donnée
void* load(MemoryHandler* handler, const char* segment_name, int pos) {
    Segment* seg = hashmap_get(handler->allocated, segment_name); // récupère le segment

    if (!seg || pos < 0 || pos >= seg->size) return NULL; // vérifie si segment ou position invalide

    int position_mem = seg->start + pos; // calcule la position mémoire

    return ((void**)handler->memory)[position_mem]; // retourne la valeur à cette position
}

// 4.5

void allocate_variables(CPU* cpu, Instruction** data, int count) {
    int total_size = 0;

    // Calcule combien de variables il faut allouer
    for (int i = 0; i < count; i++) {
        Instruction* instr = data[i];
        for (int j = 0; instr->operand2[j] != '\0'; j++) {
            if (instr->operand2[j] == ',') total_size++;
        }
        total_size++; // compte aussi la dernière valeur sans virgule
    }

    Segment* ds = malloc(sizeof(Segment)); // alloue un segment pour DS
    ds->start = 0;
    ds->size = total_size;
    ds->next = NULL;

    hashmap_insert(cpu->memory_handler->allocated, "DS", ds); // insère DS dans la mémoire

    int index = 0;

    // Remplit le segment DS avec les valeurs extraites
    for (int i = 0; i < count; i++) {
        Instruction* instr = data[i];
        char nombre[16];
        int k = 0;

        for (int j = 0;; j++) {
            char c = instr->operand2[j];
            if (c == ',' || c == '\0') {
                nombre[k] = '\0'; // fin de la chaîne nombre
                int val = 0;
                for (int m = 0; nombre[m] != '\0'; m++) val = val * 10 + (nombre[m] - '0'); // conversion en entier

                unsigned char* octet = malloc(sizeof(unsigned char)); // alloue 1 octet
                *octet = (unsigned char) val;
                store(cpu->memory_handler, "DS", index, octet); // stocke l'octet dans DS

                index++; // passe à la case suivante
                k = 0; // réinitialise

                if (c == '\0') break; // si fin de chaîne, on arrête
            } else if (c != ' ') {
                nombre[k++] = c; // construit le nombre caractère par caractère
            }
        }
    }
}

// 4.6

// Affiche le contenu du segment de données "DS"
void print_data_segment(CPU* cpu) {
    Segment* seg = hashmap_get(cpu->memory_handler->allocated, "DS"); // récupère DS

    if (!seg) {
        printf("Segment DS introuvable.\n"); // si pas trouvé
        return;
    }

    // Parcourt toutes les cases du segment DS
    for (int i = 0; i < seg->size; i++) {
        int pos = seg->start + i; // calcule la position
        unsigned char* val = (unsigned char*) ((void**)cpu->memory_handler->memory)[pos]; // récupère la valeur
        printf("[%d] = %d\n", pos, *val); // affiche la position et la valeur
    }
}
