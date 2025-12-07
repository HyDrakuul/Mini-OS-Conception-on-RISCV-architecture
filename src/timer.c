#include <stdio.h>
#include <string.h>
#include <cpu.h>
#include "platform.h"
#include "console.h"
#include "timer.h"
#include "processus.h"

static int nb_secondes =0;
static int nb_interruptions =0;


extern void mon_traitant(void);
void affichage_haut(const char* s){
    uint32_t len = strlen(s);
    uint32_t colonne = NB_COLONNE_CAR - len;
    for(int i=0; i<len; i++){
        ecrit_car(0,colonne+i,s[i],COULEUR_BASE);
    }


}




void trap_handler(uint64_t mcause, uint64_t mie, uint64_t mip){
    //on recupere la cause de l'interruption
    uint64_t cause = mcause & 0x7FFFFFFFFFFFFFFF;
    //on reprogramme la prochaine interruption timer
    uint64_t timer = *(volatile uint64_t*)(CLINT_TIMER);
    *(volatile uint64_t*)(CLINT_TIMER_CMP) = timer + (TIMER_FREQ / IT_FREQ);



    //on traite la cause de l'interruption (ici timer )
    
    if(cause ==7){
        nb_interruptions++;
        //on ordonne automatiquement à chaque interruption timer
        ordonnance(); 
        //on change l'etat de l'horloge chque seconde
        if(nb_interruptions%IT_FREQ ==0){
            nb_secondes++;
            char time[16];
            int heure=nb_secondes/3600;
            int minute=(nb_secondes%3600)/60;
            int seconde=nb_secondes%60;
            sprintf(time,"%02d:%02d:%02d",heure,minute,seconde);
            
            //affiche_etats();
            affichage_haut(time);
        } 
        

    }
    
    
}

    
uint64_t nbr_secondes(void){
    return nb_secondes;
}   
void init_traitant_timer(void (*traitant)(void)){
    uint64_t address_traitant = (uint64_t) traitant;
    //on place l'adresse du traitant dans mtvec
    __asm__ __volatile__("csrw mtvec, %0"::"r"(address_traitant));   


}
void enable_timer(){
    uint64_t timer = *(volatile uint64_t*)(CLINT_TIMER);
    *(volatile uint64_t*)(CLINT_TIMER_CMP) = timer + (TIMER_FREQ / IT_FREQ);
    __asm__ __volatile__("csrw mie, %0"::"r"(0x80)); //on active les interruptions timer en mettant le bit 7 de mie à 1 (ce que ne fait pas enable_it)
    enable_it();//on active les interruptions globalement via mstatus




}