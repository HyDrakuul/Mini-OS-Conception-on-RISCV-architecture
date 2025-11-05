#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include <inttypes.h>
#define LONGUEUR_CAR 8
#define HAUTEUR_CAR 8
#define COULEUR_BASE 0xFFFFFFFF
//definition du nombre de lignes et de colonnes de caractères à l'écran (et non en pixels)
#define NB_COLONNE_CAR (DISPLAY_WIDTH/LONGUEUR_CAR)
#define NB_LIGNE_CAR (DISPLAY_HEIGHT/HAUTEUR_CAR)
/*
 * This is the function called by printf to send its output to the screen. You
 * have to implement it in the kernel and in the user program.
 */
extern void console_putbytes(const char *s, int len);
void init_uart(void);                       
void traite_car_uart(char c);   
uint64_t init_ecran(void);
void pixel(uint32_t x, uint32_t y, uint32_t couleur);
void ecrit_car(uint32_t lig, uint32_t col, char c,  uint32_t couleur);             
void efface_ecran(void);
void place_curseur(uint32_t lig, uint32_t col, uint32_t couleur);
void defilement(void);
void traite_car(char c);

void init_uart(void);
#endif
