#ifndef UTILH
#define UTILH
#include "main.h"

/* typ pakietu */
typedef struct {
    int ts;       /* timestamp (clock lamporta */
    int src;  
    int id_workshopu;
    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
} packet_t;
/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 4

/* Typy wiadomości */
#define APP_PKT 1
#define PYRKON_START 2
#define WANT_TICKET 3
#define WANT_TICKET_ACK 4
#define WANT_WORKSHOP_TICKET 5
#define WANT_WORKSHOP_TICKET_ACK 6
#define WORKSHOP_FINISH 7
#define PYRKON_FINISH 8

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag, int workshop_id_request);
#endif
