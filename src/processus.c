#include "processus.h"
#include <string.h>
#include <stdio.h>
#include "console.h"
#include "cpu.h"

static processus_t table_processus[NB_PROCESSUS_MAX]; 

static processus_t * actif;
static processus_t * ancien;
static int processus_crees =1; //le processus idle est deja cree
//indices des processus elu et suivant
int elu = 0;
int next_elu =-1;
//fonctions des processus
/*----------------------------------------------------------------------*/
void idle(void)
{
    for (int i = 0;i<10;i++) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
    printf("[%s] idle termine\n", mon_nom());
    hlt();
}

void proc1(void) {
    for (int i = 0; i < 10; i++) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
     printf("[%s] proc1 termine\n", mon_nom());
    hlt();
}

void proc2(void) {
    for (int i = 0; i < 10; i++) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
     printf("[%s] proc2 termine\n", mon_nom());
    hlt();
}
void proc3(void) {
    for (int i = 0; i < 10; i++) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
     printf("[%s] proc3 termine\n", mon_nom());
    hlt();
}
//creation d'un tableau de pointeurs de fonctions pour les adresses des fonctions processus
//static void (*code_processus[NB_PROCESSUS_MAX])(void) = {idle, proc1, proc2, proc3};
/*----------------------------------------------------------------------*/
//fonctions du gestionnaire de processus
void init_proc(void){ 
   ancien = NULL;
   //initialisation du processus idle
   processus_t *p = &table_processus[0];
    p->etat = ELU;
    p->ctx[0] = (uint64_t)idle; //initialisation ra
    p->ctx[1] = (uint64_t)(p->pile + TAILLE_PILE); //initialisation sp
    p->pid = 0;
    strcpy(p->nom, "idle");
    actif = p;
//    for(int i =1; i<NB_PROCESSUS_MAX; i++){
//         //creation des autres processus
//         void (*code_prc)(void) = code_processus[i];
//         char nom[TAILLE_NOM];
//         sprintf(nom,"proc%d", i);
//         cree_processus(code_prc, nom);
        
       
//    }

   

}


void ordonnance(void){
    
  
    //politique du tourniquet 
   for(int i = 0; i<processus_crees; i++){//on cherche le processus elu
        processus_t *p = &table_processus[i];

        if(p->etat == ELU ) {
         elu = i;
            break;
        }
   }
   //on change l'etat du processus elu
   table_processus[elu].etat = ACTIVABLE;
   //on cherche le prochain processus activable, on effectue un cycle grace au modulo (tourniquet)
   
   next_elu=(elu+1)%processus_crees;
   while(table_processus[next_elu].etat != ACTIVABLE){
        next_elu=(next_elu+1)%processus_crees;
   }    
   //on traite le cas du nouvel elu
   processus_t *p = &table_processus[next_elu];
   
   p->etat = ELU;
   //changement de contexte
   ancien=actif;
   actif=p;
   ctx_sw(ancien->ctx, actif->ctx);
   //printf("Changement de contexte vers %s (pid=%d)\n", mon_nom(), mon_pid());
   
   }
    
int64_t mon_pid(void){
    return actif->pid;
}
char *mon_nom(void){
    return actif->nom;
}
int64_t cree_processus(void (*code)(void), char *nom){
    
        processus_t *p =&table_processus[processus_crees];
        if(p->etat== 0){
            p->etat= ACTIVABLE;
            p->ctx[0]=(uint64_t)code; //initialisation ra
            p->ctx[1]=(uint64_t)(p->pile + TAILLE_PILE); //initialisation sp
            p->pid=processus_crees;
            strcpy(p->nom, nom);
            processus_crees++;
            return p->pid;

        }
    
    return -1;

}