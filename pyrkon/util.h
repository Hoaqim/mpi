#ifndef UTILH
#define UTILH
#include "main.h"

/* typ pakietu */
typedef struct {
    int ts;       /* timestamp (clock_l lamporta */
    int src;  
    int id_workshopu;
    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
} packet_t;
/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 4

/* Typy wiadomości */
#define ACCEPT_TICKET  1
#define REQUEST_TICKET 2
#define RELEASE_TICKET 3
#define APP_PACKAGE 4
#define FINISH  5

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag, int workshop_id_request);

typedef enum {InFinish, beginPyrkon,duringPyrkon,wantPyrkon,wantWorkshop,duringWorkshop,finishedWorkshops} state_t;
extern state_t stan;
extern pthread_mutex_t stateMut;
void changeState(state_t);

#endif
