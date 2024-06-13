#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
        packet_t pakiet;
        MPI_Status status;
	    debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        //Przychodzi pakiet do wejścia na pyrkon, dostajemy broadcast
        
        pthread_mutex_lock(&clock_lMutex);
        if(clock_l >= pakiet.ts){ //(max z własnego clock_la i timestampu pakietu + 1)
            clock_l += 1; 
        } else{
            clock_l = pakiet.ts + 1; 
        }
        
        pthread_mutex_unlock(&clock_lMutex);

        //Obsługa żądań
        id_workshopu = my_workshops[rank][workshop_count[rank]]; // <- aktualny workshop 
        if(status.MPI_TAG == ACCEPT_TICKET){
            //warsztaty idą od 1, a 0 to bilet na pyrkon
            if(pakiet.id_workshopu == 0){
                println("Dostalem ACK od %d na pyrkon", status.MPI_SOURCE);
            } else{
                println("Dostałem ACK od %d na warsztat %d", status.MPI_SOURCE, id_workshopu);
            }
        }
        //Jak zaakceptowany na aktualnie przetwarzany warsztat
        if(status.MPI_TAG == ACCEPT_TICKET && pakiet.id_workshopu == id_workshopu){
            zaakceptowani[rank] += 1;
        }
        else if(status.MPI_TAG == REQUEST_TICKET){
            if(pakiet.id_workshopu == 0){
                //Request na pyrkon
            }
            else if(id_workshopu == pakiet.id_workshopu){ // jak request na aktualny workshop
                int ts_req_warsztatu = local_request_ts[rank][id_workshopu][status.MPI_SOURCE];
                if(pakiet.ts < ts_req_warsztatu || (pakiet.ts == ts_req_warsztatu && status.MPI_SOURCE < rank)){
                    //Jak ma albo mniejszy timestamp niż timestamp requesta, albo jak ma taki sam ale mniejszą range
                    sendPacket( 0, status.MPI_SOURCE, ACCEPT_TICKET, id_workshopu);
                }
                else{
                    waiting_queue[id_workshopu][indexes_for_waiting_queue[id_workshopu]] = status.MPI_SOURCE;
                    indexes_for_waiting_queue[id_workshopu] += 1;
                    zaakceptowani[rank] += 1;
                }
            }
            else{
                if(pakiet.id_workshopu != 0 || !on_pyrkon[rank]){
                    sendPacket( 0, status.MPI_SOURCE, ACCEPT_TICKET, pakiet.id_workshopu);
                }
                else if(pakiet.id_workshopu == 0 || on_pyrkon[rank]){
                    waiting_queue[0][indexes_for_waiting_queue[0]] = status.MPI_SOURCE;
                    indexes_for_waiting_queue[0] += 1;
                }
            }
        } else if(status.MPI_TAG == FINISH)
            finished[rank] += 1;
        }
    }
