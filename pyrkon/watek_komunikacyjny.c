#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
        packet_t pakiet;
        MPI_Status status;
	    debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        //Przychodzi pakiet do wejścia na pyrkon, dostajemy broadcast
        pthread_mutex_lock(&clockMutex);
        if(clock >= pakiet.ts){
            clock += 1 //wtedy dostajemy bilet i operacja zwiększa lamporta? JO
        } else{
            clock = pakiet.ts + 1; 
        }
        //aktualizacja clocka lamporta (max z własnego clocka i timestampu pakietu + 1)
        pthread_mutex_unlock(&clockMutex)

        //Obsługa żądań
        //Tu by trzbea określić jakiś workshop, który jest teraz przetwarzany chyba
        id_workshopu = workshop[rank][workshop_count[rank]] // <- aktualny workshop 
        if(status.MPI_TAG == WANT_TICKET){
            //zakładając, że warsztaty idą od 1, a 0 to bilet na pyrkon
            //i że mamy id workshopu w structcie pakietu
            if(pakiet.id_workshopu == 0){
                //Akcept na bilet na pyrkon
            } else{
                //Akcept na bilet na warsztat
                println("Dostałem ACK od %d na warsztat %d", status.MPI_SOURCE, id_workshopu);
            }
        }
        //Jak zaakceptowany na aktualnie przetwarzany warsztat
        if(status.MPI_TAG == WANT_TICKET_ACK && pakiet.id_workshopu == id_workshopu){
            zaakceptowani[rank] += 1; //chyba git jest to
        }
        else if(status.MPI_TAG == WANT_TICKET){
            if(pakiet.id_workshopu == 0){
                //Request na pyrkon
            }
            else if(id_workshopu == pakiet.id_workshopu){ // jak request na aktualny workshop
                int ts_req_warsztatu = local_request_ts[rank][id_workshopu][status.MPI_SOURCE];
                if(pakiet.ts < ts_req_warsztatu || pakiet.ts == ts_req_warsztatu && status.MPI_SOURCE < rank){
                    //Jak ma albo mniejszy timestamp niż timestamp requesta, albo jak ma taki sam ale mniejszą range
                    sendPacket( 0, status.MPI_SOURCE, WANT_TICKET_ACK, id_workshopu);
                }
                else{
                    waiting_queue[id_workshopu][indexes_for_waiting_queue[id_workshopu]] = status.MPI_SOURCE;
                    indexes_for_waiting_queue[id_workshopu] += 1;
                    zaakceptowani[rank] += 1;
                    //kolejka oczekujących na warsztat.append(MPI_SOURCE)
                }
            }
            else{
                if(pakiet.id_workshopu != 0){
                    sendPacket( 0, status.MPI_SOURCE, WANT_TICKET_ACK, pakiet.id_workshopu);
                    // println("Wysyłam ACK do %d na warsztat %d bo ubiegam sie o inny", status.MPI_SOURCE, pakiet.id_workshopu);
                }
                else if(pakiet.id_workshopu == 0){
                    waiting_queue[0][indexes_for_waiting_queue[0]] = status.MPI_SOURCE;
                    indexes_for_waiting_queue[0] += 1;
                    // println("Dodaję %d do kolejki oczekujących na warsztat %d", status.MPI_SOURCE, pakiet.id_workshopu);
                }
            }
        } else if(status.MPI_TAG == FINISH)
            finished[rank] += 1
        }
    }
