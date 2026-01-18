#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>    
#include <math.h>       
#include "Exercice2.h"  // pour MemoryHandler et Segment
#include "Exercice1.h"  // pour la table de hachage (hashmap)


/*
   Question 2.1 — memory_init
   Objectif : créer un gestionnaire mémoire avec :
      - un tableau de mémoire vide (void**)
      - une liste chaînée de segments libres (free_list)
      - une table de hachage (allocated) pour les segments utilisés
*/
MemoryHandler* memory_init(int size){
    // On alloue la structure principale
    MemoryHandler* mh = (MemoryHandler*)malloc(sizeof(MemoryHandler));
    if(mh == NULL)  return NULL;  // si l’allocation échoue, on arrête

    // On alloue le tableau de mémoire de taille "size"
    mh->memory = (void**)malloc(sizeof(void*) * size); 
    if(mh->memory == NULL){
        free(mh);       // on libère ce qu’on a déjà alloué
        return NULL;
    }

    mh->total_size = size;  // on stocke la taille totale

    // On crée un premier segment libre qui représente toute la mémoire
    mh->free_list = (Segment*)malloc(sizeof(Segment));
    if(mh->free_list == NULL){
        free(mh->memory);   // on libère le tableau mémoire
        free(mh);           // on libère la structure principale
        return NULL;
    }

    // Le premier segment libre commence à l'adresse 0 et fait toute la taille
    mh->free_list->start = 0;
    mh->free_list->size = size;
    mh->free_list->next = NULL;

    // On initialise la table de hachage pour suivre les segments alloués
    mh->allocated = hashmap_create();

    return mh;
}


/* 
   Question 2.2 — find_free_segment
   Objectif : trouver un segment libre qui contient la zone [start, start+size[
   → on renvoie ce segment ET un pointeur sur le segment précédent (prev)
 */
Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment** prev){
    Segment* it = handler->free_list;  // on commence depuis le début de la liste
    *prev = NULL;                      // au début, il n’y a pas de segment précédent

    while(it != NULL){
        // On vérifie si le segment courant couvre la zone demandée
        if(it->start <= start && (it->start + it->size) >= (start + size))
            return it;

        *prev = it;     // on avance : le segment actuel devient "précédent"
        it = it->next;  // on passe au suivant
    }

    return NULL;  // aucun segment libre trouvé qui correspond
}


/*
   Question 2.3 — create_segment
   Objectif : créer un nouveau segment mémoire à l’adresse donnée
   Étapes :
      1. Vérifier qu’il y a assez de place dans un segment libre
      2. Créer un nouveau Segment
      3. L’ajouter dans "allocated"
      4. Mettre à jour la liste des segments libres (free_list)
 */
int create_segment(MemoryHandler *handler, const char *name, int start, int size) {
    // Étape 1 : on cherche un segment libre assez grand
    Segment* prev = NULL;
    Segment* segLibre = find_free_segment(handler, start, size, &prev);
    if (segLibre == NULL) return 0;  // pas de place disponible

    // Étape 2 : on crée le nouveau segment alloué
    Segment* new_seg = (Segment*)malloc(sizeof(Segment));
    if (new_seg == NULL) return 0;

    new_seg->start = start;
    new_seg->size = size;
    new_seg->next = NULL;

    // On l’insère dans la hashmap des segments alloués
    hashmap_insert(handler->allocated, name, new_seg);

    // Étape 3 : on adapte le segment libre existant (segLibre)
    int finSegLibre = segLibre->start + segLibre->size;

    // Cas A : on coupe le segment libre en deux (avant et après la zone utilisée)
    if (segLibre->start < start && finSegLibre > start + size) {
        Segment* segBis = (Segment*)malloc(sizeof(Segment));
        if (segBis == NULL) return 0;

        // Le segment après la zone utilisée
        segBis->start = start + size;
        segBis->size = finSegLibre - (start + size); // <- Correction ici
        segBis->next = segLibre->next;

        // Mise à jour du segment "avant"
        segLibre->next = segBis;
        segLibre->size = start - segLibre->start;
    }
    // Cas B : on utilise exactement tout le segment libre → on le supprime
    else if (segLibre->start == start && finSegLibre == start + size) {
        if (segLibre == handler->free_list)
            handler->free_list = segLibre->next;
        else if (prev != NULL)
            prev->next = segLibre->next;
        free(segLibre);
    }
    // Cas C : on utilise le début du segment → on le raccourcit à gauche
    else if (segLibre->start == start) {
        segLibre->start += size;
        segLibre->size -= size;
    }
    // Cas D : on utilise la fin → on raccourcit à droite
    else {
        segLibre->size = start - segLibre->start;
    }

    return 1;  // succès
}


/*
   Question 2.4 — remove_segment
   Objectif : supprimer un segment alloué
   Étapes :
      1. Retirer le segment de "allocated"
      2. Réinsérer la zone libérée dans free_list
      3. Fusionner avec les segments libres voisins si possible
*/
int remove_segment(MemoryHandler *handler, const char *name){
    // Étape 1 : on récupère le segment à libérer
    Segment* segToRemove = (Segment*)hashmap_get(handler->allocated, name);
    if(segToRemove == NULL) return 0;

    int start = segToRemove->start;
    int size = segToRemove->size;

    // On supprime la clé de la hashmap
    int supp = hashmap_remove(handler->allocated, name);
    if(supp == 0) return 0;

    // Étape 2 : on trouve où insérer le segment libéré dans free_list
    Segment* it = handler->free_list;
    Segment* prev = NULL;

    while(it != NULL){
        if(it->start >= start + size){
            break;
        }
        prev = it;
        it = it->next;
    }

    //  Cas 1 : le segment libéré est en tout début 
    if(it == handler->free_list){
        if(start + size < it->start){
            Segment* new_seg = (Segment*)malloc(sizeof(Segment));
            if(new_seg == NULL) return 0;
            new_seg->start = start;
            new_seg->size = size;
            new_seg->next = handler->free_list;
            handler->free_list = new_seg;
        }
        else if(start + size == it->start){
            it->start = start;
            it->size += size;
        }
        return 1;
    }

    //  Cas 2 : on est à la fin de la liste 
    if(it == NULL){
        if(prev->start + prev->size < start){
            Segment* new_seg = (Segment*)malloc(sizeof(Segment));
            if(new_seg == NULL) return 0;
            new_seg->start = start;
            new_seg->size = size;
            new_seg->next = NULL;
            prev->next = new_seg;
        }
        else if(prev->start + prev->size == start){
            prev->size += size;
        }
        return 1;
    }

    //  Cas 3 : au milieu, on vérifie les fusions possibles 
    if(it != NULL && prev != NULL){
        // Fusion avec les deux segments (avant et après)
        if(prev->start + prev->size == start && start + size == it->start){
            prev->size += size + it->size;
            prev->next = it->next;
            free(it);
            return 1;
        }

        // Fusion uniquement avec celui d’après
        if(prev->start + prev->size < start && start + size == it->start){
            it->start = start;
            it->size += size;
            return 1;
        }

        // Fusion uniquement avec celui d’avant
        if(prev->start + prev->size == start){
            prev->size += size;
            return 1;
        }

        // Aucun fusion possible : insertion entre les deux
        if(prev->start + prev->size < start && start + size < it->start){
            Segment* new_seg = (Segment*)malloc(sizeof(Segment));
            if(new_seg == NULL) return 0;
            new_seg->start = start;
            new_seg->size = size;
            new_seg->next = it;
            prev->next = new_seg;
            return 1;
        }
    }

    return 0;  // normalement on ne passe jamais ici
}
