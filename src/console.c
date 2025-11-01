#include "platform.h"
#include "console.h"
#include "font.h"
#include <string.h>

#define LONGUEUR_CAR 8
#define HAUTEUR_CAR 8
#define COULEUR_BASE 0xFF
#define NB_COLONNE_CAR (DISPLAY_WIDTH/LONGUEUR_CAR)
#define NB_LIGNE_CAR (DISPLAY_HEIGHT/HAUTEUR_CAR)
static uint32_t col_curseur =0;
static uint32_t lig_curseur =1;

void init_uart(void){
    uint16_t rapport =(UART_CLOCK_FREQ/(16*UART_BAUD_RATE)) ;
    *(volatile uint8_t*)(UART_BASE + UART_FCR)|= (1<<7); 
    *(volatile uint8_t*)(UART_BASE +UART_DLL) = rapport & 0xff;
    *(volatile uint8_t*)(UART_BASE + UART_DLH)  = (rapport >> 8) & 0xff;
    *(volatile uint8_t*)(UART_BASE + UART_FCR )|= 1;
    *(volatile uint8_t*)(UART_BASE + UART_LCR) = (1<<0)|(1<<1)|(1<<3); 
    *(volatile uint8_t*)(UART_BASE + UART_LCR) &=~(1<<7);
    
}

void traite_car_uart(char car){
    *(volatile uint8_t*)(UART_BASE + UART_THR) = car;

}
uint64_t init_ecran(void){
    uint16_t type_ecran = *(volatile uint16_t*)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 +VBE_DISPI_INDEX_ID);
    if (((type_ecran)>>4) != 0xb0c) {
        return 1;
    }
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_ENABLE ) = 0x0000; 
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_XRES) = 0x0400;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_YRES) = 0x0300;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_BPP  ) = 0x0020;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_X_OFFSET   ) = 0x0000;

    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_Y_OFFSET  ) = 0x0000;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_BANK ) = 0x0000;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_ENABLE ) = 0x41; 
    return 0;


}
void pixel(uint32_t x, uint32_t y, uint32_t couleur){
    *(volatile uint32_t*)(BOCHS_DISPLAY_BASE_ADDRESS + (y*1024 + x)*4) = couleur;

}

void efface_ecran(void){
    uint8_t *origine = (uint8_t *) BOCHS_DISPLAY_BASE_ADDRESS;
    memset(origine,0,DISPLAY_SIZE*(DISPLAY_BPP/8));
    lig_curseur=1;
    col_curseur=0;
    place_curseur(lig_curseur,col_curseur,COULEUR_BASE);


}
void place_curseur(uint32_t lig, uint32_t col, uint32_t couleur){ 
    if(couleur !=0){

    lig_curseur= lig;
    col_curseur=col; 
    }
    for(int i = 0; i<8; i++){
        pixel(col*8 + i,lig*8 + 7, couleur);

        }

}
void defilement(void){
    uint8_t *origine = (uint8_t *) BOCHS_DISPLAY_BASE_ADDRESS;
    size_t taille_ligne_ecran = DISPLAY_WIDTH*(DISPLAY_BPP/8);
    size_t taille_ligne_texte = 8*taille_ligne_ecran;

    memmove(origine, origine + taille_ligne_texte, (DISPLAY_HEIGHT*taille_ligne_ecran) - taille_ligne_texte );//pour la source, on se place au début de la 2eme ligne
    lig_curseur = lig_curseur -1;
    memset(origine+(DISPLAY_HEIGHT-HAUTEUR_CAR)*taille_ligne_ecran,0,taille_ligne_texte );



}
void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t couleur){
        unsigned char *pixels_car = font8x8_basic[(int)c];
        place_curseur(lig,col,0);
        for( int ligne_car = 0  ; ligne_car<LONGUEUR_CAR; ligne_car++){
            
            for(int bit = 0; bit<8; bit++){
                if((pixels_car[ligne_car] & (1<<bit)) !=0){
                    pixel(col*8 + bit, lig*8 + ligne_car, couleur);
                }
            }
        }
        place_curseur(lig+1, col+1, COULEUR_BASE);
        
    



}
void traite_car(char c){
    if (c=='\b'){
        if(col_curseur > 0){
            place_curseur(lig_curseur,col_curseur,0);
            col_curseur--;
            place_curseur(lig_curseur, col_curseur, COULEUR_BASE);
        }
       

    }
    else if (c=='\t'){
        
        place_curseur(lig_curseur,col_curseur,0);
        col_curseur+=8;
        if(col_curseur>=NB_COLONNE_CAR){
            lig_curseur++;
            place_curseur(lig_curseur,0, COULEUR_BASE);
        }
        else if (lig_curseur >= NB_LIGNE_CAR){
            defilement();
            lig_curseur++;
            place_curseur(lig_curseur, 0,COULEUR_BASE);
        }
        else{
            
            
    
            place_curseur(lig_curseur+1,0,COULEUR_BASE);

        }

    }
    else if  (c=='\n'){
       place_curseur(lig_curseur,col_curseur,0);
       if(lig_curseur>=NB_LIGNE_CAR){
        defilement();
        lig_curseur++;
        place_curseur(lig_curseur,0,COULEUR_BASE);

       }
       else{
        lig_curseur++;
        place_curseur(lig_curseur,0,COULEUR_BASE);

       }
       

    }
    else if (c=='\f'){
        efface_ecran();


    }
    else if (c=='\r'){
        place_curseur(lig_curseur,col_curseur,0);
        place_curseur(lig_curseur,0,COULEUR_BASE);


    }
    else{
        ecrit_car(lig_curseur,col_curseur,c,COULEUR_BASE);
    }
}
extern void console_putbytes(const char *s, int len){
    init_uart();
    init_ecran();
    for(int i =0; i<len; i++){
        traite_car_uart(s[i]);
        traite_car(s[i]);
    }
    
}

