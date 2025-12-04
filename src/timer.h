#ifndef __TIMER_H__
#define __TIMER_H__
#include <inttypes.h>
#define IT_FREQ 20
void affichage_haut(const char* s);
void trap_handler(uint64_t mcause, uint64_t mie, uint64_t mip);
void init_traitant_timer(void (*traitant)(void));
void enable_timer();
uint64_t nbr_secondes(void);








#endif 