// Exercice6.h 

#ifndef EXERCICE6_H
#define EXERCICE6_H

#include "Exercice1.h"
#include "Exercice2.h"
#include "Exercice3.h"
#include "Exercice4.h"
#include "Exercice5.h"

// Fonction 6.1 : trim
char* trim(char* str);
int resolve_constants(ParserResult *result) ;
int search_and_replace(char **str, HashMap *values);

// Fonction 6.2 : search_and_replace
int search_and_replace(char** str, HashMap* values);
//Exercie 6.3
void allocate_code_segment(CPU* cpu, Instruction** program, int count);

// Fonction 6.4 : handle_instruction
void handle_instruction(CPU* cpu, Instruction* instr, void* src, void* dest);
int execute_instruction(CPU *cpu, Instruction *instr);

// Fonction 6.5 : execute_instruction
int execute_instruction(CPU* cpu, Instruction* instr);

// Fonction 6.6 : fetch_next_instruction
Instruction* fetch_next_instruction(CPU* cpu);


// Fonction 6.7 : run_program
int run_program(CPU* cpu);

#endif
