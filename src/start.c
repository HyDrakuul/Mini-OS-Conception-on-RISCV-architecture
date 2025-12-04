#include <inttypes.h>
#include <stdio.h>
#include <cpu.h>
#include "console.h"
#include "processus.h"
#include "timer.h"
extern void proc1(void);
extern void idle(void);

extern void mon_traitant(void);
/* on peut s'entrainer a utiliser GDB avec ce code de base */
/* par exemple afficher les valeurs de n et res avec la commande display */
/* une fonction bien connue */
/*uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}*/

void kernel_start(void)
{

   
    /* quand on saura gerer l'ecran, on pourra afficher x */
    init_uart();
    
    printf("> Test [printf UART] (1/1)\n");
    init_ecran();
    if(init_ecran()==1){printf("Pas de device trouvé\n");}
    else if (init_ecran()==2){  printf("Mauvaise config\n");}
    else if (init_ecran()==0){printf("Bonne config\n");}
    init_traitant_timer(mon_traitant);
    enable_timer();
    printf("Truc\nSalut\n1\t2\t3\nAB\bC\nBEEF\rRABBIT");
    for (int i=0;i<121;i++) {printf("%d\n",i);}
    printf("temps écoulé %u\n", nbr_secondes());
    printf("Hello\n");
    printf("FIN\n");
    
    init_proc();
    cree_processus(proc1, "proc1");
    cree_processus(proc1, "proc2"); 
    cree_processus(proc1, "proc3");
    cree_processus(proc1, "proc4");

    idle();
  
    
    
    
    
    
   

    //printf("\f");
    
    /* on ne doit jamais sortir de kernel_start */
    while (1) {
        /* cette fonction arrete le processeur */
        hlt();
    }
}
