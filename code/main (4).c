#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Exercice1.h"
#include "Exercice2.h"
#include "Exercice3.h"
#include "Exercice4.h"
#include "Exercice5.h"
#include "Exercice6.h"
#include "Exercice7.h"
#include "Exercice8.h"

int main() {


//  TEST EXERCICE 1 : HashMap 
    printf("=== TEST EXERCICE 1 : HashMap ===\n");
    HashMap* map = hashmap_create();
    int val1 = 42;
    hashmap_insert(map, "cle", &val1);
    int* recup = hashmap_get(map, "cle");
    if (recup) printf("Valeur récupérée : %d\n", *recup);
    hashmap_remove(map, "cle");
    hashmap_destroy(map);

//  TEST EXERCICE 2 : MemoryHandler 
    MemoryHandler* mem = memory_init(64);

// Allocation correcte : créer les segments
    create_segment(mem, "A", 0, 10); // Allouer à partir de l'adresse 0 une taille de 10
    create_segment(mem, "B", 10, 20); // Allouer à partir de l'adresse 10 une taille de 20

// Libération correcte : supprimer le segment "B"
    remove_segment(mem, "B");

// Libération de la mémoire : (pas besoin de memory_destroy, tu feras un free manuel si besoin)
    free(mem->memory);
    hashmap_destroy(mem->allocated);
    free(mem);

 
 //  TEST EXERCICE 3 : Parser 
    printf("\n TEST EXERCICE 3 : Parser \n");
    ParserResult* result = parse("programme.txt");
    if (result) {
        printf("DATA : %d, CODE : %d\n", result->data_count, result->code_count);
        free_parser_result(result);
    }

 //  TEST EXERCICE 4 : Segment DS 
    printf("\nTEST EXERCICE 4 : Segment DS \n");
    CPU* cpu = cpu_init(100);
    Instruction* i1 = malloc(sizeof(Instruction));
    i1->mnemonic = strdup("x");
    i1->operand1 = strdup("DW");  // DW = Data Word 
    i1->operand2 = strdup("10,20");


    Instruction* i2 = malloc(sizeof(Instruction));
    i2->mnemonic = strdup("y");
    i2->operand1 = strdup("DW");
    i2->operand2 = strdup("30");

    Instruction* data[] = {i1, i2};
    allocate_variables(cpu, data, 2);
    print_data_segment(cpu);

    //  Libération manuelle des instructions
    free(i1->mnemonic); free(i1->operand1); free(i1->operand2); free(i1);
    free(i2->mnemonic); free(i2->operand1); free(i2->operand2); free(i2);
    cpu_destroy(cpu);

 // TEST EXERCICE 5 : ADRESSAGES 
  printf("=== TEST EXERCICE 5 : ADRESSAGES + handle_mov ===\n");

    // 1. Initialiser l'environnement de test
    CPU* cpu5 = setup_test_environment();
    if (!cpu5) {
        printf("Erreur : impossible d'initialiser l'environnement de test\n");
        return 1;
    }

    // Tester chaque mode d'adressage avec handle_mov()

    // Adressage immédiat
    void* src_imm = immediate_addressing(cpu5, "42");
    int* dest_imm = malloc(sizeof(int));  // allouer un int pour tester la copie
    if (src_imm && dest_imm) {
        handle_mov(cpu5, src_imm, dest_imm);
        printf("Résultat MOV immédiat : %d (attendu : 42)\n", *dest_imm);
    }

    // Adressage par registre
    void* src_reg = register_addressing(cpu5, "AX");
    int* dest_reg = malloc(sizeof(int));
    if (src_reg && dest_reg) {
        handle_mov(cpu5, src_reg, dest_reg);
        printf("Résultat MOV registre : %d (attendu : 3)\n", *dest_reg);
    }

    // Adressage direct
    void* src_dir = direct_addressing(cpu5, "DS[2]");
    int* dest_dir = malloc(sizeof(int));
    if (src_dir && dest_dir) {
        handle_mov(cpu5, src_dir, dest_dir);
        printf("Résultat MOV direct : %d (attendu : 25)\n", *dest_dir);
    }

    // Adressage indirect
    // Remplir DS[5] avec l'adresse d'un emplacement existant
    int* fake_address = malloc(sizeof(int));
    *fake_address = 2; // Suppose que DS[2] est une adresse valide
    store(cpu5->memory_handler, "DS", 5, fake_address);

    void* src_indir = indirect_addressing(cpu5, "DS[5]");
    int* dest_indir = malloc(sizeof(int));
    if (src_indir && dest_indir) {
        handle_mov(cpu5, src_indir, dest_indir);
        printf("Résultat MOV indirect : %d (attendu : 25)\n", *dest_indir);
    }

    // Nettoyage
    free(dest_imm);
    free(dest_reg);
    free(dest_dir);
    free(dest_indir);
    cpu_destroy(cpu5);

//// TEST EXERCICE 6 : INSTRUCTIONS
    printf("TEST EXERCICE 6 : INSTRUCTIONS\n");

  // Initialiser un CPU avec une mémoire de taille 100
    CPU* cpu6 = cpu_init(100);

//  Créer instruction : MOV AX, 15
    Instruction* instr1 = malloc(sizeof(Instruction));
    instr1->mnemonic = strdup("MOV");
    instr1->operand1 = strdup("AX");
    instr1->operand2 = strdup("15");

// . Créer instruction : MOV BX, AX
    Instruction* instr2 = malloc(sizeof(Instruction));
    instr2->mnemonic = strdup("MOV");
    instr2->operand1 = strdup("BX");
    instr2->operand2 = strdup("AX");

//  Créer instruction : ADD BX, 5
    Instruction* instr3 = malloc(sizeof(Instruction));
    instr3->mnemonic = strdup("ADD");
    instr3->operand1 = strdup("BX");
    instr3->operand2 = strdup("5");

//  Placer toutes les instructions dans un tableau (le programme)
    Instruction* program[] = {instr1, instr2, instr3};

//  Charger les instructions dans le segment de code du CPU
    allocate_code_segment(cpu6, program, 3);

//. Exécuter le programme instruction par instruction
    run_program(cpu6);

//  Après l'exécution, récupérer la valeur du registre BX
    int* bx_final = hashmap_get(cpu6->context, "BX");
    if (bx_final) {
        printf("BX final : %d (attendu : 20)\n", *bx_final); // Affiche la valeur finale de BX
    }

//  Libérer la mémoire allouée pour chaque instruction
    free(instr1->mnemonic); free(instr1->operand1); free(instr1->operand2); free(instr1);
    free(instr2->mnemonic); free(instr2->operand1); free(instr2->operand2); free(instr2);
    free(instr3->mnemonic); free(instr3->operand1); free(instr3->operand2); free(instr3);

//  Détruire le CPU pour libérer toute la mémoire
    cpu_destroy(cpu6);

// TEST EXERCICE 7 : PILE (STACK)


    printf("\n TEST EXERCICE 7 : PILE \n");

// Création du CPU pour les tests
    CPU* cpu7 = cpu_init(1024);
    // Allouer le segment SS pour la pile
    create_segment(cpu7->memory_handler, "SS", 200, 128);  // Par exemple à partir de l'adresse 200

// Initialiser SP et BP au sommet de la pile
    int* sp = hashmap_get(cpu7->context, "SP");
    int* bp = hashmap_get(cpu7->context, "BP");
    if (sp) *sp = 200 + 128 - 1;  // Pile descendante
    if (bp) *bp = 200 + 128 - 1;

// Empile trois valeurs manuellement
    printf("Empile 3 valeurs (10, 20, 30)...\n");
    push_value(cpu7, 10);
    push_value(cpu7, 20);
    push_value(cpu7, 30);

// Dépile les valeurs une par une
    int valeur1 = 0;
    int valeur2 = 0;
    int valeur3 = 0;

    pop_value(cpu7, &valeur1);
    pop_value(cpu7, &valeur2);
    pop_value(cpu7, &valeur3);

// Affiche les valeurs dépilées
    printf("Valeurs dépilées (attendu 30, 20, 10) : %d, %d, %d\n", valeur1, valeur2, valeur3);

// Test avec instructions PUSH AX et POP BX
    printf("\nTest avec instructions PUSH AX et POP BX\n");

// On met AX = 42
    int* registre_ax = hashmap_get(cpu7->context, "AX");
    *registre_ax = 42;

// Création de l'instruction PUSH AX
    Instruction instruction_push;
    instruction_push.mnemonic = strdup("PUSH");
    instruction_push.operand1 = strdup("");       // vide car PUSH utilise operand2
    instruction_push.operand2 = strdup("AX");     // c'est ici que doit être "AX"

// Création de l'instruction POP BX
    Instruction instruction_pop;
    instruction_pop.mnemonic = strdup("POP");
    instruction_pop.operand1 = strdup("BX");      // destination
    instruction_pop.operand2 = strdup("");        // vide car POP utilise operand1

// Exécution des deux instructions
    execute_instruction(cpu7, &instruction_push);
    execute_instruction(cpu7, &instruction_pop);

// Vérification : BX doit contenir 42
    int* registre_bx = hashmap_get(cpu7->context, "BX");
    if (registre_bx != NULL) {
        printf("BX après POP : %d (attendu : 42)\n", *registre_bx);
    }

// Libération mémoire
    free(instruction_push.mnemonic);
    free(instruction_push.operand1);
    free(instruction_push.operand2);
    free(instruction_pop.mnemonic);
    free(instruction_pop.operand1);
    free(instruction_pop.operand2);

    cpu_destroy(cpu7);
    
    
    
     printf("\nTEST EXERCICE 8 : ALLOC et FREE \n");

// Création CPU pour l'exercice 8
    CPU* cpu_8 = setup_test_environment();

// AX = 1 (index dans le segment)
    int* ax_8 = hashmap_get(cpu_8->context, "AX");
    *ax_8 = 1;

// BX = 0 (stratégie First Fit)
    int* bx_8 = hashmap_get(cpu_8->context, "BX");
    *bx_8 = 0;

// Appelle ALLOC
    Instruction alloc;
    alloc.mnemonic = strdup("ALLOC");
    alloc.operand1 = strdup("");  // vide car non utilisé
    alloc.operand2 = strdup("");  // idem
    execute_instruction(cpu_8, &alloc);
    printf("Adresse ES après ALLOC : %d\n", *((int*) hashmap_get(cpu_8->context, "ES")));
    

// MOV [ES:AX], 10
Instruction mov_es_ax;
    mov_es_ax.mnemonic = strdup("MOV");
    mov_es_ax.operand1 = strdup("[ES:AX]");
    mov_es_ax.operand2 = strdup("10");
    execute_instruction(cpu_8, &mov_es_ax);
    

// MOV BX, [ES:AX]
    Instruction mov_bx_es;
    mov_bx_es.mnemonic = strdup("MOV");
    mov_bx_es.operand1 = strdup("BX");
    mov_bx_es.operand2 = strdup("[ES:AX]");
    execute_instruction(cpu_8, &mov_bx_es);

// Vérifie contenu de BX
    int* bx_val = hashmap_get(cpu_8->context, "BX");
    printf("Valeur à [ES:AX] : %d (attendu : 10)\n", *bx_val);

// Appelle FREE
    Instruction free_es;
    free_es.mnemonic = strdup("FREE");
    free_es.operand1 = strdup("");
    free_es.operand2 = strdup("");
    execute_instruction(cpu_8, &free_es);

// Vérifie ES
    int* es_val = hashmap_get(cpu_8->context, "ES");
    printf("ES après FREE : %d (attendu : -1)\n", *es_val);

// Libère mémoire instructions
    free(alloc.mnemonic); free(alloc.operand1); free(alloc.operand2);
    free(mov_es_ax.mnemonic); free(mov_es_ax.operand1); free(mov_es_ax.operand2);
    free(mov_bx_es.mnemonic); free(mov_bx_es.operand1); free(mov_bx_es.operand2);
    free(free_es.mnemonic); free(free_es.operand1); free(free_es.operand2);

// Libération finale du CPU
    cpu_destroy(cpu_8);
    return 0;
}


