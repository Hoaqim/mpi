#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "util.h"
/* boolean */
#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10
#define MAX_PARTICIPANTS 14
#define MAX_WORKSHOPS 16

extern int rank;
extern int size;
extern int id_workshopu;
extern int clock_l;
extern int number_of_tickets;
extern int number_of_workshops;
extern int number_of_people_per_workshop;
extern int number_of_participants;
extern int number_of_workshops_per_participant;
extern int zaakceptowani[MAX_PARTICIPANTS];
extern int waiting_queue[MAX_WORKSHOPS + 1][MAX_PARTICIPANTS]; //waiting for ticket for warsztat or pyrkon
extern int indexes_for_waiting_queue[MAX_WORKSHOPS + 1]; //ich indeksy
extern int workshop_count[MAX_PARTICIPANTS];
extern int my_workshops[MAX_PARTICIPANTS][MAX_WORKSHOPS + 1];
extern int on_pyrkon[MAX_PARTICIPANTS];
extern int local_request_ts[MAX_PARTICIPANTS][MAX_WORKSHOPS + 1][MAX_PARTICIPANTS];
extern int finished[MAX_PARTICIPANTS];

extern pthread_t threadKom;

extern pthread_mutex_t clock_lMutex;
extern pthread_mutex_t finishMutex;




/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta 
   
   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl bold/normal;kolor [RANK]
                                           FORMAT:argumenty doklejone z wywołania debug poprzez __VA_ARGS__
					   "%c[%d;%dm"       wyczyszczenie atrybutów    27,0,37
                                            UWAGA:
                                                27 == kod ascii escape. 
                                                Pierwsze %c[%d;%dm ( np 27[1;10m ) definiuje styl i kolor literek
                                                Drugie   %c[%d;%dm czyli 27[0;37m przywraca domyślne kolory i brak pogrubienia (bolda)
                                                ...  w definicji makra oznacza, że ma zmienną liczbę parametrów
                                            
*/
#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

void changeState( state_t );

#endif
