#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"

int rank, size, id_workshopu;
state_t stan=InRun;
pthread_t threadKom;
pthread_mutex_t clockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t finishedMutex = PTHREAD_MUTEX_INITIALIZER;


int clock=0;
int number_of_tickets=10; // liczba biletów na pyrkon
int number_of_workshops=14; 
int number_of_people_per_workshop = 3; 
int number_of_participants = 14;
int number_of_workshops_per_participant=2; 
int zaakceptowani[MAX_PARTICIPANTS];
int waiting_queue[MAX_WORKSHOPS + 1][MAX_PARTICIPANTS]; //waiting for ticket for warsztat or pyrkon
int indexes_for_waiting_queue[MAX_WORKSHOPS + 1]; //ich indeksy
int workshop_count[MAX_PARTICIPANTS];
int my_workshops[MAX_PARTICIPANTS][MAX_WORKSHOPS + 1];
int on_pyrkon[MAX_PARTICIPANTS];
int local_request_ts[MAX_PARTICIPANTS][MAX_WORKSHOPS + 1][MAX_PARTICIPANTS];
int finished[MAX_PARTICIPANTS];


void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(threadKom,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();

    memset(number_of_acks, 0, sizeof(number_of_acks));
    memset(waiting_queue, 0, sizeof(waiting_queue));
    memset(indexes_for_waiting_queue, 0, sizeof(indexes_for_waiting_queue));
    memset(workshop_count, 0, sizeof(workshop_count));
    memset(my_workshops, 0, sizeof(my_workshops));
    memset(on_pyrkon, 0, sizeof(on_pyrkon));
    memset(local_request_ts, 0, sizeof(local_request_ts));
    memset(finished, 0, sizeof(finished));
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}


int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);
    inicjuj_typ_pakietu(); // tworzy typ pakietu
    packet_t pkt;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    pthread_create( &threadKom, NULL, startKomWatek , 0);

    mainLoop();
    
    finalizuj();
    return 0;
}

