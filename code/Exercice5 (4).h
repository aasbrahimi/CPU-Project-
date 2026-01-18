#ifndef EXERCICE5_H
#define EXERCICE5_H



// Fonction pour tester une regex
int matches(const char *pattern, const char *string);

// EXERCICE 5.2 
void* immediate_addressing(CPU* cpu, const char* operand);

// EXERCICE 5.3 
void* register_addressing(CPU* cpu, const char* operand);

// EXERCICE 5.4 :-
void* direct_addressing(CPU* cpu, const char* operand);

// EXERCICE 5.5
void* indirect_addressing(CPU* cpu, const char* operand);

// EXERCICE 5.6
void handle_mov(CPU* cpu, void* src, void* dest);

// EXERCICE 5.7 
CPU* setup_test_environment();

// EXERCICE 5.8 
void* resolve_addressing(CPU* cpu, const char* operand);

#endif
