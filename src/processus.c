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
        for(int i=0; i<NB_PROCESSUS_MAX; i++){
            memset(&table_processus[i], 0, sizeof(processus_t));
            table_processus[i].etat=0; //initialisation des etats des processus a 0 (non utilises)
        }
        elu=0;
        next_elu=-1;
    
    
    ancien = NULL;
    //initialisation du processus idle
    processus_t *p = &table_processus[0];
        p->etat = ELU;
        p->ctx[0] = (uint64_t)proc_launcher; //initialisation ra
        p->ctx[1] = (uint64_t)(p->pile + TAILLE_PILE); //initialisation sp
        p->ctx[16]= (uint64_t)idle;
        p->pid = 0;
        p->reveil_prevu = 0;
        strcpy(p->nom, "idle");
        actif = p;
        processus_crees = 1;


    }


    void ordonnance(void){
        
        
    //politique du tourniquet 

    //reveil des processus endormis 
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

    while(next_elu !=start && table_processus[next_elu].etat != ACTIVABLE){

    //on cherche le prochain processus activable, on effectue un cycle grace au modulo (tourniquet)
            next_elu=(next_elu+1)%processus_crees;

    }    
    if (table_processus[next_elu].etat != ACTIVABLE){
        //aucun processus activable, on reste sur l'elu actuel
        return;
    }    
    // modificatoin de l'etat du processus élu
    if (table_processus[start].etat == ELU){
        table_processus[start].etat = ACTIVABLE;}
        
    //on traite le cas du nouvel elu
    processus_t *p = &table_processus[next_elu];
    elu =next_elu;
    
    p->etat = ELU;
    //changement de contexte

    ancien=actif;
    actif=p;
    
    
    ctx_sw(ancien->ctx, actif->ctx);
    affiche_etats(); 
    //printf("Changement de contexte vers %s (pid=%d)\n", mon_nom(), mon_pid()); 
    }


    void  dors(uint64_t nbr_secs){
        if(actif->pid !=0){
            actif->etat = ENDORMI;
            actif->reveil_prevu = nbr_secondes() + nbr_secs;
            ordonnance();
        }
        

    }   
    int64_t mon_pid(void){
        return actif->pid;
    }
    char *mon_nom(void){
        return actif->nom;
    }
    int64_t cree_processus(void (*code)(void), char *nom){
        //on ne doit jamais depasser le nombre de processus max
            if(processus_crees>=NB_PROCESSUS_MAX){
                return -1;
            }
            //remplissage des cases des processus morts
            for(int i=1;i<processus_crees;i++){
                processus_t *p =&table_processus[i];
                if(p->etat ==MORT){
                p->etat= ACTIVABLE;
                p->ctx[0]=(uint64_t)proc_launcher; //initialisation ra
                p->ctx[1]=(uint64_t)(p->pile + TAILLE_PILE); //initialisation sp
                p->ctx[16]=(uint64_t)code;
                p->pid=i;
                p->reveil_prevu =0; 
                strcpy(p->nom, nom);
                //on n'incrémente pas le nombre de processu crees ici 
                return p->pid;

                }


            }
            // creation d'un nouveau processus
            processus_t *p =&table_processus[processus_crees];
        
                p->etat= ACTIVABLE;
                p->ctx[0]=(uint64_t)proc_launcher; //initialisation ra
                p->ctx[1]=(uint64_t)(p->pile + TAILLE_PILE); //initialisation sp
                p->ctx[16]=(uint64_t)code;//argument passé à proc_launcher
                p->pid=processus_crees;
                p->reveil_prevu =0; 
                strcpy(p->nom, nom);
                processus_crees++;
                return p->pid;
            
            

    }
    void fin_processus(void){
        if(actif->pid !=0){
            actif->etat= MORT;
            ordonnance();
        }

    } 
    void affiche_etats(void){
        //affichage des etats des processus en cours d'exe
        
        char infos_proc[NB_COLONNE_CAR-16];
        uint32_t colonne =0 ;
        //on verifie l'etat de chaque processus
        for(int i=0; i<processus_crees; i++){
            processus_t *p =&table_processus[i];
            char state[16];
            // conversion de enum en char
            if(p->etat == ELU){
                sprintf(state,"ELU");

            }
            else if(p->etat == ACTIVABLE){
                sprintf(state,"ACTIVABLE");
                
            }
            else if(p->etat == ENDORMI){
                sprintf(state,"ENDORMI");
            }
            else{
                sprintf(state,"MORT");

            }
            //nettoyage de la ligne pour eviter les artefacts 
            memset(infos_proc, ' ', NB_COLONNE_CAR-16);
            infos_proc[NB_COLONNE_CAR-16 - 1] = '\0';
            //chaine de caractère standardisée, taille max de state de 8 car pour eviter les artefacts
            snprintf(infos_proc,NB_COLONNE_CAR-16,"%s;pid:%d;etat:%-8s ", p->nom,p->pid, state);
            //on ne doit pas depasser la fin de la ligne 
            int len = strlen(infos_proc);
            if (colonne + len > NB_COLONNE_CAR) {
                break;
            }
            //ecriture de la chaine infos_proc qui vient d'être construite 
        for(int j=0; j<len; j++){
            ecrit_car(0,colonne+j,infos_proc[j],COULEUR_BASE);
        
        }
        //on passe au processus suivant donc on commence l'criture à la colonne len
        colonne +=len;

        }
        
    
    }

    void proc_launcher(void (*proc)(void)){

        
        proc();

        fin_processus();
    

    }