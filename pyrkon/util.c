#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

state_t stan=beginPyrkon;

pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clock_lMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t finishedMutex = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { 
    { "pakiet aplikacyjny", APP_PKT }, 
    { "start pyrkonu", PYRKON_START }, 
    { "chcę bilet", WANT_TICKET }, 
    { "potwierdzenie chęci biletu na warsztat", WANT_TICKET_ACK }, 
    { "koniec warsztatu", WORKSHOP_FINISH }, 
    { "koniec pyrkonu", PYRKON_FINISH }
};


const char const *tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	if ( tagNames[i].tag == tag )  return tagNames[i].name;
    }
    return "<unknown>";
}
/* tworzy typ MPI_PAKIET_T
*/
void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int       blocklengths[NITEMS] = {1,1,1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);
    offsets[3] = offsetof(packet_t, id_workshopu);
    
    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}

/* opis patrz util.h */
void sendPacket(packet_t *pkt, int destination, int tag, int workshop_id_request) {
    int freepkt = 0;
    if (pkt == 0) {
        pkt = (packet_t *)malloc(sizeof(packet_t));
        freepkt = 1;
    }
    pkt->src = rank;
    pkt->id_workshopu = workshop_id_request;

    pthread_mutex_lock(&clock_lMutex);
    clock_l++;
    pkt->ts = clock_l;
    if(tag == WANT_TICKET){
        local_request_ts[rank][id_workshopu][destination] = clock_l;
    }
    pthread_mutex_unlock(&clock_lMutex);

    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string(tag), destination);
    if (freepkt) free(pkt);
}



void changeState( state_t newState )
{
    pthread_mutex_lock( &stateMutex );
    if (stan==InFinish) { 
	pthread_mutex_unlock( &stateMutex );
        return;
    }
    stan = newState;
    pthread_mutex_unlock( &stateMutex );
}

