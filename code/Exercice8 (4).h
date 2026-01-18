#ifndef EXERCICE8_H
#define EXERCICE8_H

#include "Exercice4.h"
#include "Exercice2.h"

// EXERCICE 8.1 
void* segment_override_addressing(CPU* cpu, const char* operand);

// EXERCICE 8.3  
int find_free_address_strategy(MemoryHandler* handler, int size, int strategy);

// EXERCICE 8.5 : 
int alloc_es_segment(CPU* cpu);

// EXERCICE 8.6 :
int free_es_segment(CPU* cpu);

// EXERCICE 8.7 : 
void handle_instruction(CPU* cpu, Instruction* instr, void* src, void* dest);

#endif

