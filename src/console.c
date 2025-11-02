#include "platform.h"
#include "console.h"
#include "font.h"
#include <string.h>

#define LONGUEUR_CAR 8
#define HAUTEUR_CAR 8
#define COULEUR_BASE 0xFFFFFFFF
#define NB_COLONNE_CAR (DISPLAY_WIDTH/LONGUEUR_CAR)
#define NB_LIGNE_CAR (DISPLAY_HEIGHT/HAUTEUR_CAR)
static uint32_t col_curseur =0;
static uint32_t lig_curseur =1;

void init_uart(void){
    uint16_t rapport =(UART_CLOCK_FREQ/(16*UART_BAUD_RATE)) ;
    *(volatile uint8_t*)(UART_BASE + UART_LCR)|= (1<<7); 
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
    //config PCIe
    int good_device =-1;
    uint16_t vendor_id, device_id;
    for(int device = 0; device<32; device++){
        uint32_t adresse = PCI_ECAM_BASE_ADDRESS +(device<<11);
        vendor_id = *((volatile uint32_t *)adresse)& 0xffff;
        device_id = (*((volatile uint32_t *)adresse)>>16)&0xffff;
        if(vendor_id ==0x1234  && device_id ==0x1111){
            good_device =device;
            break;
      
        }

    }
    if(good_device==-1){
        return 1;
    }
    
    *(volatile uint32_t*)(PCI_ECAM_BASE_ADDRESS + (good_device<<11) + 0x04)|=0b111;
    *(volatile uint32_t*)(PCI_ECAM_BASE_ADDRESS + (good_device<<11) + 0x10)=BOCHS_DISPLAY_BASE_ADDRESS;
    *(volatile uint32_t*)(PCI_ECAM_BASE_ADDRESS + (good_device<<11) + 0x18)=BOCHS_CONFIG_BASE_ADDRESS;
    
    //config écran
    uint16_t type_ecran = *(volatile uint16_t*)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 +VBE_DISPI_INDEX_ID*2);
    if (((type_ecran)>>4) != 0xb0c) {
        return 2;
    }
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_ENABLE*2 ) = 0x0000; 
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_XRES*2) = 0x0400;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_YRES*2) = 0x0300;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_BPP*2  ) = 0x0020;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_X_OFFSET*2   ) = 0x0000;

    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_Y_OFFSET*2  ) = 0x0000;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_BANK*2 ) = 0x0000;
    *(volatile uint16_t *)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 + VBE_DISPI_INDEX_ENABLE*2 ) = 0x41; 
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
    size_t taille_ligne_texte = HAUTEUR_CAR*taille_ligne_ecran;

    memmove(origine, origine + taille_ligne_texte, (DISPLAY_HEIGHT*taille_ligne_ecran) - taille_ligne_texte );//pour la source, on se place au début de la 2eme ligne
    
    //memset(origine + (NB_LIGNE_CAR - 1) * HAUTEUR_CAR * taille_ligne_ecran,0,taille_ligne_texte );
    lig_curseur = NB_LIGNE_CAR-1;



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
        place_curseur(lig, col+1, COULEUR_BASE);
        
    



}
void traite_car(char c){
    if (c=='\b'){
        if(col_curseur > 0){
            //place_curseur(lig_curseur,col_curseur,0);
            col_curseur--;
            ecrit_car(lig_curseur,col_curseur,' ',COULEUR_BASE);
            place_curseur(lig_curseur, col_curseur, COULEUR_BASE);
        }
       

    }
    else if (c=='\t'){
        
        place_curseur(lig_curseur,col_curseur,0);
        col_curseur+=8;
        if (lig_curseur >= NB_LIGNE_CAR){
            defilement();
            lig_curseur=NB_LIGNE_CAR-1;
            place_curseur(lig_curseur, 0,COULEUR_BASE);
        }
        else if(col_curseur>=NB_COLONNE_CAR){
            lig_curseur++;
            place_curseur(lig_curseur,0, COULEUR_BASE);
        }
        
        else{
            
            
    
            place_curseur(lig_curseur,col_curseur,COULEUR_BASE);

        }

    }
    else if  (c=='\n'){
       place_curseur(lig_curseur,col_curseur,0);
       if(lig_curseur>=NB_LIGNE_CAR){
        defilement();
        lig_curseur=NB_LIGNE_CAR-1;
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

