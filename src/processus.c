#include "processus.h"
#include <string.h>
#include <stdio.h>
#include "console.h"
#include "cpu.h"
#include "timer.h"
static processus_t table_processus[NB_PROCESSUS_MAX]; 

static processus_t * actif;
static processus_t * ancien;
static int processus_crees= 0; //le processus idle est deja cree
//indices des processus elu et suivant
static int elu = 0;
static int next_elu =-1;
//fonctions des processus
/*----------------------------------------------------------------------*/
void idle()
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n",
               nbr_secondes(),
               mon_nom(),
               mon_pid());  
        enable_it();
        hlt();
        disable_it();
    }
}
//creation d'un tableau de pointeurs de fonctions pour les adresses des fonctions processus
//static void (*code_processus[NB_PROCESSUS_MAX])(void) = {idle, proc1, proc2, proc3};
/*----------------------------------------------------------------------*/
//fonctions du gestionnaire de processus
void init_proc(void){ 
    for(int i=0; i<processus_crees; i++){
        memset(&table_processus[i], 0, sizeof(processus_t));
        table_processus[i].etat=0; //initialisation des etats des processus a 0 (non utilises)
    }
    elu=0;
    next_elu=-1;
   
   
   ancien = NULL;
   //initialisation du processus idle
   processus_t *p = &table_processus[0];
    p->etat = ELU;
    p->ctx[0] = (uint64_t)idle; //initialisation ra
    p->ctx[1] = (uint64_t)(p->pile + TAILLE_PILE); //initialisation sp
    p->pid = 0;
    p->reveil_prevu = 0;
    strcpy(p->nom, "idle");
    actif = p;
    processus_crees = 1;


}


void ordonnance(void){
    
    
  
    //politique du tourniquet 
//    for(int i = 0; i<processus_crees; i++){//on cherche le processus elu
//         processus_t *p = &table_processus[i];

//         if(p->etat == ELU ) {
//          elu = i;
//             break;
//         }
//    }
 //on cherche le prochain processus activable, on effectue un cycle grace au modulo (tourniquet)
   for(int reveil =1; reveil < processus_crees; reveil++){
        if(table_processus[reveil].etat ==ENDORMI){
            if(table_processus[reveil].reveil_prevu <= nbr_secondes()){
                table_processus[reveil].etat = ACTIVABLE; 
                
            }
        }
    }
   //on change l'etat du processus elu si il n'est pas endormi

  

   int start = elu;
   next_elu=(elu+1)%processus_crees;
  //pour eviter une boucle infinie si aucun processus n'est activable
   while(next_elu !=start && table_processus[next_elu].etat != ACTIVABLE){
        next_elu=(next_elu+1)%processus_crees;

   }    
   if (table_processus[next_elu].etat != ACTIVABLE){
    //aucun processus activable, on reste sur l'elu actuel
    return;
   }    
   if (table_processus[elu].etat == ELU){
    table_processus[start].etat = ACTIVABLE;}
    
   //on traite le cas du nouvel elu
   processus_t *p = &table_processus[next_elu];
   elu =next_elu;
   
   p->etat = ELU;
   //changement de contexte

   ancien=actif;
   actif=p;
   
   ctx_sw(ancien->ctx, actif->ctx);
   //printf("Changement de contexte vers %s (pid=%d)\n", mon_nom(), mon_pid());
   

   
  

  
}


void  dors(uint64_t nbr_secs){
    if(actif->pid !=0){
        actif->etat = ENDORMI;
        actif->reveil_prevu = nbr_secondes() + nbr_secs;
    }
    

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
            p->reveil_prevu =0; 
            strcpy(p->nom, nom);
            processus_crees++;
            return p->pid;

        }
    
    return -1;

}