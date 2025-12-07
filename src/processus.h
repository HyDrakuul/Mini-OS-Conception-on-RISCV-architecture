#ifndef __PROCESSUS_H__
#define __PROCESSUS_H__
#include <inttypes.h>
#define TAILLE_NOM 32
#define NB_REGISTRES_CTX 17
#define TAILLE_PILE 4096
#define NB_PROCESSUS_MAX  50
enum etat_processus {ELU, ACTIVABLE, ENDORMI,MORT};
typedef struct processus{
    uint64_t pid;
    char nom[TAILLE_NOM];
    enum etat_processus etat;
    uint64_t ctx[NB_REGISTRES_CTX];
    uint64_t pile[TAILLE_PILE];
   
    uint64_t reveil_prevu;

} processus_t;

void init_proc(void);
void ordonnance(void);
int64_t mon_pid(void);
char *mon_nom(void);
void dors(uint64_t nbr_secs);
void ctx_sw(uint64_t *ctxt_de_sauvegarde, uint64_t *ctxt_a_restaurer);
int64_t cree_processus(void (*code)(void), char *nom);
void fin_processus(void);
void affiche_etats(void);
void proc_launcher(void (*proc)(void));
#endif
