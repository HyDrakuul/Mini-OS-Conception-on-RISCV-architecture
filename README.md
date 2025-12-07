![lastupdate](https://pcs-louising-a1584e.pages.ensimag.fr/lastupdate.svg)
[Log du dernier test](https://pcs-louising-a1584e.pages.ensimag.fr/pcsef.log)

# Affichage

![Affichage_simple](https://pcs-louising-a1584e.pages.ensimag.fr/Affichage_simple.svg)
![LF_new_line](https://pcs-louising-a1584e.pages.ensimag.fr/LF_new_line.svg)
![HT_horizontal_tab](https://pcs-louising-a1584e.pages.ensimag.fr/HT_horizontal_tab.svg)
![BS_backspace](https://pcs-louising-a1584e.pages.ensimag.fr/BS_backspace.svg)
![CR_carriage_ret](https://pcs-louising-a1584e.pages.ensimag.fr/CR_carriage_ret.svg)
![Curseur_simple](https://pcs-louising-a1584e.pages.ensimag.fr/Curseur_simple.svg)
![scroll](https://pcs-louising-a1584e.pages.ensimag.fr/scroll.svg)
![Curseur_après_scroll](https://pcs-louising-a1584e.pages.ensimag.fr/Curseur_après_scroll.svg)
![FF_formfeed](https://pcs-louising-a1584e.pages.ensimag.fr/FF_formfeed.svg)
![Curseur_après_FF](https://pcs-louising-a1584e.pages.ensimag.fr/Curseur_après_FF.svg)

# UART

![UART_config](https://pcs-louising-a1584e.pages.ensimag.fr/UART_config.svg)
![printf_UART](https://pcs-louising-a1584e.pages.ensimag.fr/printf_UART.svg)

# Timer

![init_traitant_timer](https://pcs-louising-a1584e.pages.ensimag.fr/init_traitant_timer.svg)
![enable_timer_mie](https://pcs-louising-a1584e.pages.ensimag.fr/enable_timer_mie.svg)
![enable_timer_re-active](https://pcs-louising-a1584e.pages.ensimag.fr/enable_timer_re-active.svg)
![trap_handler_re-active](https://pcs-louising-a1584e.pages.ensimag.fr/trap_handler_re-active.svg)
![trap_handler_affichage_du_temps](https://pcs-louising-a1584e.pages.ensimag.fr/trap_handler_affichage_du_temps.svg)
![nbr_secondes](https://pcs-louising-a1584e.pages.ensimag.fr/nbr_secondes.svg)

# Processus gestion basique

![init_proc_mon_nom_mon_pid](https://pcs-louising-a1584e.pages.ensimag.fr/init_proc_mon_nom_mon_pid.svg)
![cree_processus_ordonnance_simple](https://pcs-louising-a1584e.pages.ensimag.fr/cree_processus_ordonnance_simple.svg)
![cree_processus_ordonnance_double](https://pcs-louising-a1584e.pages.ensimag.fr/cree_processus_ordonnance_double.svg)
![cree_processus_ordonnance_apres_reset](https://pcs-louising-a1584e.pages.ensimag.fr/cree_processus_ordonnance_apres_reset.svg)
![Processus_par_interruption](https://pcs-louising-a1584e.pages.ensimag.fr/Processus_par_interruption.svg)

# Processus endormissement

![dors_2_processus_endormis](https://pcs-louising-a1584e.pages.ensimag.fr/dors_2_processus_endormis.svg)
![dors_reveil_d'un_processus](https://pcs-louising-a1584e.pages.ensimag.fr/dors_reveil_d'un_processus.svg)
![dors_reveil_de_tous_les_processus](https://pcs-louising-a1584e.pages.ensimag.fr/dors_reveil_de_tous_les_processus.svg)

# Processus terminaison

![terminaison_explicite](https://pcs-louising-a1584e.pages.ensimag.fr/terminaison_explicite.svg)
![terminaison_implicite](https://pcs-louising-a1584e.pages.ensimag.fr/terminaison_implicite.svg)

# Informations supplémentaires
L'ensemble du cahier des charges minimal a été traité et réussi. 

Les différentes parties ont été séparées en différents fichier (processus/console/timer).

Les tests sont effectués dans start.c.

Les fonctions des processus sont dans start.c et sont commentées/décommentées en fonction de la partie testée.

Une fonction affiche_etat(void) a été implémentée et permet de visualiser l'état des processus en cours.

Cependant, celle-ci faisait crasher les tests (car devait gêner le test de l'affichage du timer).

Ainsi, cette fonction a été commentée. 

Il est possible de la décommenter dans processus.c, processus.h et dans son appel à la fin de la fonction ordonnancement().

Aprés cela, il est possible de visualiser en haut à gauche de l'écran les processus crées et leur état en temps réel. 
