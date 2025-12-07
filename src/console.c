#include "platform.h"
#include <string.h>
#include "console.h"
#include "font.h"



/*position du curseur en (coordonnées de caractères)*/
static uint32_t col_curseur =0;
static uint32_t lig_curseur =1;

void init_uart(void){
    /*on se contente de suivre l'énoncé*/
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
    /*config PCIe*/

    int good_device =-1;
    uint16_t vendor_id, device_id;
    for(uint64_t device = 0; device<32; device++){
        uint32_t adresse = PCI_ECAM_BASE_ADDRESS +(device<<11);
        /*bits de poids faible*/
        vendor_id = *((volatile uint32_t *)adresse)& 0xffff;
        /*bits de poids fort*/
        device_id = (*((volatile uint32_t *)adresse)>>16)&0xffff;
        if(vendor_id ==0x1234  && device_id ==0x1111){
            /*l'écran est trouvé*/
            good_device =device;
            break;
      
        }

    }
    if(good_device==-1){
        /*erreur pas de device trouvé*/
        return 1;
    }
    /*une fois le device trouvé, 
     *on l'active en écrivant dans les registres appropriés (voir énoncé)
     */
    *(volatile uint32_t*)(PCI_ECAM_BASE_ADDRESS + (good_device<<11) + 0x04)|=0b111;
    *(volatile uint32_t*)(PCI_ECAM_BASE_ADDRESS + (good_device<<11) + 0x10)=BOCHS_DISPLAY_BASE_ADDRESS;
    *(volatile uint32_t*)(PCI_ECAM_BASE_ADDRESS + (good_device<<11) + 0x18)=BOCHS_CONFIG_BASE_ADDRESS;
    
    /*config écran*/

    uint16_t type_ecran = *(volatile uint16_t*)(BOCHS_CONFIG_BASE_ADDRESS + 0x500 +VBE_DISPI_INDEX_ID*2);
    if (((type_ecran)>>4) != 0xb0c) {
        /*erreur type d'écran incorrect*/
        return 2;
    }

    /*on configure selon l'énoncé*/
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
    /*on se contente d'appliquer la formule de l'énoncé*/
    *(volatile uint32_t*)(BOCHS_DISPLAY_BASE_ADDRESS + (y*1024 + x)*4) = couleur;

}

void efface_ecran(void){
    /*taille d'une ligne en octets*/
    size_t taille_ligne_ecran = DISPLAY_WIDTH*(DISPLAY_BPP/8);
    /*taille d'une ligne de texte en octets (en prenant en compte la taille des caractères)*/
    size_t taille_ligne_texte = HAUTEUR_CAR*taille_ligne_ecran;
    /*adresse de début de l'écran*/
    uint8_t *origine = (uint8_t *) BOCHS_DISPLAY_BASE_ADDRESS;
    /*on efface tout l'écran (valeur 0 = noir)*/
    memset(origine,
        0,
        DISPLAY_SIZE*(DISPLAY_BPP/8));
    /*on place le curseur au debut de la 2nde ligne*/
    lig_curseur=1;
    col_curseur=0;
    place_curseur(lig_curseur,col_curseur,COULEUR_BASE);


}
void place_curseur(uint32_t lig, uint32_t col, uint32_t couleur){ 
    if(couleur !=0){
    /*si il ne s'agit pas d'effacer le curseur, on met à jour sa position*/
        lig_curseur= lig;
        col_curseur=col; 
    }
    for(uint64_t i = 0; i<8; i++){
        /*on dessine une ligne en bas du caractère pour représenter le curseur*/
        pixel(col*8 + i,
            lig*8 + 7, 
            couleur);

        }

}
void defilement(void){
    
    size_t taille_ligne_ecran = DISPLAY_WIDTH*(DISPLAY_BPP/8);
    size_t taille_ligne_texte = HAUTEUR_CAR*taille_ligne_ecran;
    uint8_t *origine = (uint8_t *) BOCHS_DISPLAY_BASE_ADDRESS  ;
    /*on décale toutes les lignes de texte d'une ligne vers le haut
     *en commencant par la 2eme ligne
     *Pour la source, on se place au début de la 3eme ligne, 
     *la destinantion est la 2eme ligne
     */
    memmove(origine+ taille_ligne_texte,
        origine + 2*taille_ligne_texte, 
        (NB_LIGNE_CAR-2)*taille_ligne_texte);
    /*on efface la dernière ligne*/
    memset(origine+(NB_LIGNE_CAR-1)*taille_ligne_texte,
    0,
    taille_ligne_texte );
    /*on met à jour la position du curseur à la dernière ligne */
    lig_curseur = NB_LIGNE_CAR-1;



}
void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t couleur){
    /*on récupère les pixels du caractère à afficher (tableau défini dans font.h )*/
        unsigned char *pixels_car = font8x8_basic[(int)c];
        /*on nettoie la case*/
        for (int ligne_car =0; ligne_car<LONGUEUR_CAR; ligne_car++){
            for(int bit =0; bit<LONGUEUR_CAR;   bit++){
                pixel(col*8+bit, 
                    lig*8 +ligne_car,
                    0); 
            }
        }
        /*on écrit le caractère (pixel blanc pour chaque bit à 1, sinon noir)*/
        for( uint64_t ligne_car = 0  ; ligne_car<LONGUEUR_CAR; ligne_car++){
            for(uint64_t bit = 0; bit<LONGUEUR_CAR; bit++){
                if((pixels_car[ligne_car] & (1<<bit)) !=0){
                    pixel(col*8 + bit, 
                        lig*8 + ligne_car, 
                        couleur);
                }
            }
        }
        
        
    



}
void traite_car(char c){
    /*on traite les caractères spéciaux*/
    if (c=='\b'){
        /*backspace, on efface le caractère précédent si on n'est pas en début de ligne*/ 
        if(col_curseur > 0){
            place_curseur(lig_curseur,
                col_curseur,
                0);
            col_curseur--;
            
            place_curseur(lig_curseur,
                col_curseur, 
                COULEUR_BASE);
        }
        
       

    }
    else if (c=='\t'){
        /*tabulation, on avance au prochain multiple de 8*/
        place_curseur(lig_curseur,
            col_curseur,
            0);                              
        col_curseur = ((col_curseur/8)+1)*8 ;
       /*si on dépasse la fin de la ligne, on passe à la ligne suivante */                         
        if(col_curseur>=NB_COLONNE_CAR){
            lig_curseur++;
            place_curseur(lig_curseur,
                0,
                COULEUR_BASE);
            /*on defile si on dépasse la fin de l'écran*/
            if (lig_curseur >= NB_LIGNE_CAR){
                defilement();
                place_curseur(lig_curseur,
                    0,
                    COULEUR_BASE);
        }
        }else{
            /*on met à jour la position du curseur*/
            place_curseur(lig_curseur,
                col_curseur,
                COULEUR_BASE);

        }

    }else if (c=='\n'){
        /*nouvelle ligne, on passe à la ligne suivante*/
       place_curseur(lig_curseur,
        col_curseur,
        0);
       lig_curseur++;
       /*on defile si on dépasse la fin de l'écran*/
       if(lig_curseur>=NB_LIGNE_CAR){
            defilement();
            lig_curseur=NB_LIGNE_CAR-1;
            place_curseur(lig_curseur,
                0,
                COULEUR_BASE);
       }else{
            place_curseur(lig_curseur,
                0,
                COULEUR_BASE);
       }
       

    }else if (c=='\f'){
        /*ff, on efface l'écran*/
        efface_ecran();
    }else if (c=='\r'){
        /*retour chariot, on revient au début de la ligne courante*/
        place_curseur(lig_curseur,
            col_curseur,
            0);
        col_curseur=0;
        place_curseur(lig_curseur,
            col_curseur,
            COULEUR_BASE);


    }else{
        /*caractère normal, on l'affiche à la position courante du curseur*/
        ecrit_car(lig_curseur,
            col_curseur,
            c,
            COULEUR_BASE);
        col_curseur++;
        /*si on dépasse la fin de la ligne, on passe à la ligne suivante*/
        if(col_curseur >=NB_COLONNE_CAR){
            /*si on dépasse la fin de l'écran, on defile*/
            if(lig_curseur>=NB_LIGNE_CAR){
                defilement();
                lig_curseur=NB_LIGNE_CAR-1;

            }else{
                lig_curseur++;
                col_curseur=0;  
            }
        }
        place_curseur(lig_curseur, 
            col_curseur, 
            COULEUR_BASE);

    }
}
extern void console_putbytes(const char *s, int len){
    /*initialisation de l'UART et de l'écran*/
    init_uart();
    init_ecran();
    /*on traite chaque caractère de la chaîne dans l'UART et sur l'écran*/
    for(uint64_t i =0; i<len; i++){
        traite_car_uart(s[i]);
        traite_car(s[i]);
    }
    
}

