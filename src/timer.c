#include <string.h>
#include "platform.h"
#include "console.h"
#include "timer.h"

void affichage_haut(const char* s){
    uint32_t len = strlen(s);
    uint32_t colonne = NB_COLONNE_CAR - len;
    for(int i=0; i<len; i++){
        ecrit_car(0,colonne+i,s[i],COULEUR_BASE);
    }


}



void trap_handler(uint64_t mcause, uint64_t mie, uint64_t mip){
    uint64_t cause = mcause & 0x7FFFFFFFFFFFFFFF;
    if(cause ==7){

    }}